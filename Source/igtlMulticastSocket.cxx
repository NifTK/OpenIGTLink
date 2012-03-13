/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlMulticastSocket.cxx $
  Language:  C++
  Date:      $Date: 2010-06-09 16:16:36 -0400 (Wed, 09 Jun 2010) $
  Version:   $Revision: 6525 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkClientSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "igtlMulticastSocket.h"

namespace igtl
{

//-----------------------------------------------------------------------------
MulticastSocket::MulticastSocket()
{
  m_port = -1;
  m_address = new char[200];
}

//-----------------------------------------------------------------------------
MulticastSocket::~MulticastSocket()
{
}

//-----------------------------------------------------------------------------
int MulticastSocket::InitMultiCast(const char* multicast_group, int port, int ttl, bool loop)
{
  if (this->m_SocketDescriptor != -1)
  {
    igtlWarningMacro("Client connection already exists. Closing it.");
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
  }
  
  // Create a multicast capable socket
  this->m_SocketDescriptor = this->CreateSocket(P_TCP_MULTI);
  if (!this->m_SocketDescriptor)
  {
    igtlErrorMacro("Failed to create socket.");
    return -1;
  }
  
  // Bind socket
  if (BindSocket(this->m_SocketDescriptor, port) < 0)
    return -1;
  
  // Select local interface for sending the multicast messages
  struct sockaddr_in address;
  socklen_t length = sizeof(address);
  getsockname(this->m_SocketDescriptor, (struct sockaddr *)&address, &length);
  
  int status = -1;
  
  #if defined(_WIN32) && !defined(__CYGWIN__)
    status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_IF, (char *)&address.sin_addr, sizeof(address.sin_addr));
  #else
    status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_IF, &address.sin_addr, sizeof(address.sin_addr));
  #endif

  if (status < 0)
  {
    handle_error("setsockopt: ");
    std::cerr <<"Cannot init multicast send." <<std::endl;
    return status;
  }

  struct ip_mreq imreq;

  // Set up multicast-send on the default interface
  imreq.imr_multiaddr.s_addr = inet_addr(multicast_group);
  imreq.imr_interface.s_addr = INADDR_ANY; // use DEFAULT interface

  // JOIN multicast group (for receiving) on default interface
  #if defined(_WIN32) && !defined(__CYGWIN__)
    status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP,(const char *)&imreq, sizeof(struct ip_mreq));
  #else
    status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP,(const void *)&imreq, sizeof(struct ip_mreq));
  #endif

  if (status < 0)
  {
    std::cerr <<"Cannot join multicast group (receive)." <<std::endl;
    return status;
  }

  // Set socket time-to-live
  #if defined(_WIN32) && !defined(__CYGWIN__)
	  status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&ttl, sizeof(int));
  #else
    status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(int));
  #endif

  if (status < 0)
  {
    std::cerr <<"Failed to set the multi-cast time-to-live";
		return status;
  }

  // Set up loopback
  if (loop)
  {
    int temp = 1;
    #if defined(_WIN32) && !defined(__CYGWIN__)
      status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *)&temp, sizeof(int));
    #else
      status = setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_LOOP, &temp, sizeof(int));
    #endif
	  if (status < 0) 
    {
		  std::cerr <<"Failed to enable multi-cast loopback";
		  return status;
	  }
  }

  m_port = port;
  strcpy(m_address, multicast_group); 
  return 0;
}

int MulticastSocket::SendMulticast(const void* data, int length)
{
  struct sockaddr_in stDestAddr;
  stDestAddr.sin_family = PF_INET; 
  stDestAddr.sin_addr.s_addr = inet_addr(m_address);
  stDestAddr.sin_port = htons(m_port); 
  

  int status = -1;
  //status = sendto(this->m_SocketDescriptor, (char *)data, length, 0, (struct sockaddr *) &stDestAddr, sizeof(struct sockaddr)); 
  status = sendto(this->m_SocketDescriptor, (char *)data, length, 0, (struct sockaddr *) &stDestAddr, sizeof(struct sockaddr)); 
  
  if (status < 0)
  {
    handle_error("setsockopt: ");
    std::cerr <<"Failed to send data." <<std::endl;
    return status;
  }

  return status;
}

int MulticastSocket::ReceiveMulticast(void* data, int readFully)
{
  char* buffer = reinterpret_cast<char*>(data);
  int total       = 0;
  int bytesRead   = 0;
  int flags       = 0;
  int trys        = 0;

  unsigned int dwSenderSize = 0;
  sockaddr_in sender;

  int rVal     = 0;
  u_long iMode = 1;
  
  #if defined(_WIN32) && !defined(__CYGWIN__)
    u_long bytesToRead = 0;
    rVal  = ioctlsocket(this->m_SocketDescriptor, FIONBIO, &iMode);
    rVal |= ioctlsocket(this->m_SocketDescriptor, FIONREAD, &bytesToRead);
  #elif defined(__linux__)
    flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
  #elif defined(__APPLE__)
    int bytesToRead = 0;
    rVal  = ioctl(this->m_SocketDescriptor, FIONBIO, &iMode);
    rVal |= ioctl(this->m_SocketDescriptor, FIONREAD, &bytesToRead);
  #endif

  std::cerr <<"Num of bytes to read.. " <<bytesToRead;

  // Receive a generic message
  do
  {
    bytesRead   = 0;

    //std::cerr <<"Num of bytes to read.. " <<length;
    // Try reading from the socket
    try
    {
      bytesRead = recv(this->m_SocketDescriptor, buffer+total, bytesToRead-total, flags);
      //bytesRead = recvfrom(this->m_SocketDescriptor, buffer+total, length-total, flags, (sockaddr *)&sender, &dwSenderSize);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return -2;
    }

    if (bytesRead < 1)
    {
      #if defined(_WIN32) && !defined(__CYGWIN__)
        int error = WSAGetLastError();
        trys++;

        if (((error == WSAENOBUFS) && (trys < 1000)) || ((error == WSAEWOULDBLOCK) && (trys < 1000)))
        {
          Sleep(5);
          continue;
        }
      #elif defined(__APPLE__)
        int error = errno;

        if ((error == EWOULDBLOCK) && (++trys < 1000))
        {
          Sleep(5);
          continue;
        }
      #endif

      // Error in recv()
      if (bytesRead < 0)
        handle_error("recv: ");

      return bytesRead;
    }

    total += bytesRead;

  } while(readFully && total < bytesToRead);

  //ULONGLONG time_after = gethectonanotime_last();
  //std::cerr <<"Time delta: " <<time_after - time_before <<std::endl;

  //std::cerr <<"Number of bytes actually read: " <<bytesRead <<" Total: " <<total <<std::endl;

  //std::cerr <<"First 15 char of the message: ";
  //for (int oo = 0; oo < 30; oo++)
  //  std::cerr <<buffer[oo];
  //std::cerr <<" -END\n";

  return total;
}

//-----------------------------------------------------------------------------
void MulticastSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace

