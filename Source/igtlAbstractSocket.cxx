/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: igtlSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "igtlAbstractSocket.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
  #include <winsock2.h> 
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <sys/time.h>
#endif

#include <stdio.h>
#include <string.h>

namespace igtl
{

//-----------------------------------------------------------------------------
AbstractSocket::AbstractSocket()
{
  this->m_SocketDescriptor = INVALID_SOCKET;
}

//-----------------------------------------------------------------------------
AbstractSocket::~AbstractSocket()
{
  if (this->GetConnected())
    {
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = INVALID_SOCKET;
    }
}

//-----------------------------------------------------------------------------
bool AbstractSocket::GetConnected() const
{
  return (this->m_SocketDescriptor >=0 && this->m_SocketDescriptor != INVALID_SOCKET);
}

//-----------------------------------------------------------------------------
bool AbstractSocket::IsValid(int socketdescriptor)
{
  return (socketdescriptor >= 0 && socketdescriptor != INVALID_SOCKET);
}

//-----------------------------------------------------------------------------
int AbstractSocket::CreateSocket()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  // Declare variables
  WSADATA wsaData;
  //SOCKET ListenSocket;
  //sockaddr_in service;

  //---------------------------------------
  // Initialize Winsock
  int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
  if( iResult != NO_ERROR )
    {
    std::cerr << "Error at WSAStartup" << std::endl;
    return -1;
    }
#endif

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  // Elimate windows 0.2 second delay sending (buffering) data.
  int on = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)))
    {
    return -1;
    }
  return sock;
}

//-----------------------------------------------------------------------------
int AbstractSocket::BindSocket(int socketdescriptor, int port)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
  {
    return -1;
  }

  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  // Allow the socket to be bound to an address that is already in use
#ifdef _WIN32
  int opt=1;
  setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
#elif defined(VTK_HAVE_SO_REUSEADDR)
  int opt=1;
  setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, sizeof(int));
#endif

  int err = bind(socketdescriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server));
  if (err != 0)
    {
    igtlSocketErrorMacro(<< "BindSocket");
    return -1;
    }

  return 0;
}

//-----------------------------------------------------------------------------
int AbstractSocket::Accept(int socketdescriptor)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
    {
    return -1;
    }

  int res = accept(socketdescriptor, 0, 0);
  if (res == -1)
  {
    igtlSocketErrorMacro(<< "Accept");
    return -1;
  }

  return res;
}

//-----------------------------------------------------------------------------
int AbstractSocket::Listen(int socketdescriptor)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
    {
    return -1;
    }

  int err = listen(socketdescriptor, 1);
  if (err != 0)
  {
    igtlSocketErrorMacro(<< "Listen");
    return -1;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int AbstractSocket::SelectSocket(int socketdescriptor, unsigned long msec)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
    {
    return -1;
    }
  
  fd_set rset;
  struct timeval tval;
  struct timeval* tvalptr = 0;
  if ( msec > 0 )
    {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = (msec % 1000)*1000;
    tvalptr = &tval;
    }
  FD_ZERO(&rset);
  FD_SET(socketdescriptor, &rset);
  int res = select(socketdescriptor + 1, &rset, 0, 0, tvalptr);
  if(res == 0)
    {
    return 0; // For time limit expire
    }
  else if ( res < 0 || !(FD_ISSET(socketdescriptor, &rset)) )
    {
    igtlSocketErrorMacro(<< "SelectSocket");
    return -1;
    }

  // The indicated socket has some activity on it.
  return 1;
}

//-----------------------------------------------------------------------------
int AbstractSocket::SelectSockets(const int* sockets_to_select, int size,
    unsigned long msec, int* selected_index)
{
  int i;
  int max_fd = -1;
  *selected_index = -1;
  if (size <  0)
    {
    return -1;
    }
  
  fd_set rset;
  struct timeval tval;
  struct timeval* tvalptr = 0;
  if ( msec > 0 )
    {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = msec % 1000;
    tvalptr = &tval;
    }
  FD_ZERO(&rset);
  for (i=0; i<size; i++)
    {
    FD_SET(sockets_to_select[i],&rset);
    max_fd = (sockets_to_select[i] > max_fd)? sockets_to_select[i] : max_fd;
    }
  
  int res = select(max_fd + 1, &rset, 0, 0, tvalptr);
  if (res == 0)
    {
    return 0; // Timeout
    }
  if (res < 0)
    {
    // SelectSocket error.
    return -1;
    }
  
  //check which socket has some activity.
  for (i=0; i<size; i++)
    {
    if ( FD_ISSET(sockets_to_select[i],&rset) )
      {
      *selected_index = i;
      return 1;
      }
    }
  return -1; 
}

//-----------------------------------------------------------------------------
int AbstractSocket::Connect(int socketdescriptor, const char* hostName, int port)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
    {
    return -1;
    }

  struct hostent* hp;
  hp = gethostbyname(hostName);
  if (!hp)
    {
    unsigned long addr = inet_addr(hostName);
    hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    }
 
  if (!hp)
    {
    igtlSocketErrorMacro(<< "Connect failed to get hostent struct.");
    return -1;
    }

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
  name.sin_port = htons(port);

  int err = connect(socketdescriptor, reinterpret_cast<sockaddr*>(&name),
                 sizeof(name));
  if (err != 0)
    {
    igtlSocketErrorMacro(<< "Connect failed");
    return -1;
    }

  return 0;
}

//-----------------------------------------------------------------------------
int AbstractSocket::GetPort(int sock)
{
  struct sockaddr_in sockinfo;
  memset(&sockinfo, 0, sizeof(sockinfo));
#if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
  socklen_t sizebuf = sizeof(sockinfo);
#else
  int sizebuf = sizeof(sockinfo);
#endif

  int err = getsockname(sock, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf);
  if(err != 0)
    {
    igtlSocketErrorMacro(<< "GetPort");
    return 0;
    }
  int res = ntohs(sockinfo.sin_port);
  if (res == 0)
    {
    igtlSocketErrorMacro(<< "GetPort");
    return 0;
    }
  return res;
}

//-----------------------------------------------------------------------------
int AbstractSocket::CloseSocket()
{
  if (!this->GetConnected())
    {
    return -1;
    }

  int ret = this->CloseSocket(this->m_SocketDescriptor);
  this->m_SocketDescriptor = INVALID_SOCKET;
  return ret;
}

//-----------------------------------------------------------------------------
int AbstractSocket::CloseSocket(int socketdescriptor)
{
  if (!this->GetConnected() || !this->IsValid(socketdescriptor))
  {
    return -1;
  }

  int err = 0;

  #if defined(_WIN32) && !defined(__CYGWIN__)
    err = closesocket(socketdescriptor);
  #else
    err = close(socketdescriptor);
  #endif

  if (err != 0)
  {
    igtlSocketErrorMacro(<< "CloseSocket");
    return -1;
  }

  this->m_SocketDescriptor = INVALID_SOCKET;
  return 0;
}

//-----------------------------------------------------------------------------
int AbstractSocket::Send(const void* data, int length)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  if (length == 0)
    {
    // nothing to send.
    return 1;
    }
  const char* buffer = reinterpret_cast<const char*>(data);
  int total = 0;
  do
    {
    int flags;
#if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0;
#else
    // disabling, since not present on SUN.
    // flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
    flags = 0;
#endif
    int n = send(this->m_SocketDescriptor, buffer+total, length-total, flags);
    if(n < 0)
      {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Send failed.");
      return 0;
      }
    total += n;
    } while(total < length);
  return 1;
}

//-----------------------------------------------------------------------------
int AbstractSocket::Receive(void* data, int length, int readFully/*=1*/)
{
  if (!this->GetConnected())
    {
    return 0;
    }

  char* buffer = reinterpret_cast<char*>(data);
  int total = 0;
  do
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
    int trys = 0;
#endif

    int n = recv(this->m_SocketDescriptor, buffer+total, length-total, 0);

#if defined(_WIN32) && !defined(__CYGWIN__)
    if(n == 0)
      {
      // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
      // will work if you try again.
      int error = WSAGetLastError();
      if ((error == WSAENOBUFS) && (trys++ < 1000))
        {
        Sleep(1);
        continue;
        }
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
      }
    else if (n < 0)
      {
      // TODO: Need to check if this means timeout.
      return -1;
      }
#else
    if(n == 0) // Disconnected
      {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
      }
    else if (n < 0) // Error (including time out)
      {
      // TODO: If it is time-out, errno == EAGAIN
      return -1;
      }
#endif

    total += n;
    } while(readFully && total < length);
  return total;
}


//-----------------------------------------------------------------------------
int AbstractSocket::GetSocketAddressAndPort(std::string& address, int& port)
{
  struct sockaddr_in sockinfo;

  memset(&sockinfo, 0, sizeof(sockinfo));

#if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
  socklen_t sizebuf = sizeof(sockinfo);
#else
  int sizebuf = sizeof(sockinfo);
#endif
  
  if( getsockname(this->m_SocketDescriptor, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf) != 0)
    {
    return 0;
    }
  const char* a = inet_ntoa(sockinfo.sin_addr);
  if ( a == NULL )
    {
    return 0;
    }
  address = a;
  port = ntohs(sockinfo.sin_port);

  return 1;
}

//-----------------------------------------------------------------------------
int AbstractSocket::Skip(int length, int skipFully/*=1*/)
{
  if (length == 0)
    {
    return 0;
    }

  if (!this->GetConnected())
    {
    return 0;
    }

  unsigned char dummy[256];
  int block  = 256;
  int n      = 0;
  int remain = length;

  do
    {
    if (remain < block)
      {
      block = remain;
      }
    
    n = this->Receive(dummy, block, skipFully);
    if (!skipFully && n <= 0)
      {
      break;
      }
    remain -= n;
    }
  while (remain > 0 || (skipFully && n < block));

  return (length - remain);

}

//-----------------------------------------------------------------------------
void AbstractSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace
