/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlSocket.cxx $
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
  Module:    $RCSfile: igtlSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*=========================================================================

  Program:   Open ITK Link Library
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2010-06-09 16:16:36 -0400 (Wed, 09 Jun 2010) $
  Version:   $Revision: 6525 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlSocket.h"

#include <fcntl.h>
#include <cstdio>
#include <cstring>

#if defined(_WIN32) && !defined(__CYGWIN__)
  #define WSA_VERSION MAKEWORD(1,1)
#endif

namespace igtl
{

void handle_error(const char * msg)
{
  std::cerr <<std::endl;
  std::cerr <<"Socket Error: ";
  #if defined(_WIN32) && !defined(__CYGWIN__)
    std::cerr <<msg <<" " <<WSAGetLastError();
  #else
    perror(msg);
  #endif

  std::cerr <<std::endl;
}


//-----------------------------------------------------------------------------
Socket::Socket()
{
  this->m_SocketDescriptor = -1;
  this->m_TimeoutFlag = 0;
  this->m_ConnectionTimeout.tv_sec = 5;
  this->m_ConnectionTimeout.tv_usec = 0;
}

//-----------------------------------------------------------------------------
Socket::~Socket()
{
  if (this->m_SocketDescriptor != -1)
  {
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
  }
}

//-----------------------------------------------------------------------------
int Socket::CreateSocket()
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
int Socket::BindSocket(int socketdescriptor, int port)
{
  if (!this->IsValid())
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
//#elif defined(VTK_HAVE_SO_REUSEADDR)
#else
  int opt=1;
  setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, sizeof(int));
#endif

  int err = bind(socketdescriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server));
  if (err != 0)
  {
    handle_error("bind");
    return err;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int Socket::Accept(int socketdescriptor)
{
  if (!this->IsValid())
  {
    return -1;
  }

  return accept(socketdescriptor, 0, 0);
}

//-----------------------------------------------------------------------------
int Socket::Listen(int socketdescriptor)
{
  if (!this->IsValid())
  {
    return -1;
  }

  int err = listen(socketdescriptor, 1);
  if (err != 0)
  {
    handle_error("listen");
    return err;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int Socket::SelectSocket(int socketdescriptor, unsigned long msec)
{
  if (!this->IsValid())
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

  if (res == 0)
  {
    return 0;//for time limit expire
  }
  else if (res < 0 || !(FD_ISSET(socketdescriptor, &rset)) )
  {
    // Some error.
    return res;
  }

  // The indicated socket has some activity on it.
  return 1;
}

//-----------------------------------------------------------------------------
int Socket::SelectSockets(const int* sockets_to_select, int size, unsigned long msec, int* selected_index)
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
		return 0; //Timeout
	}
	else if (res < 0)
	{
		// SelectSocket error.
		return res;
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
int Socket::Connect(int socketdescriptor, const char* hostName, int port)
{
  if (!this->IsValid())
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
    // vtkErrorMacro("Unknown host: " << hostName);
    return -1;
  }

	struct sockaddr_in name;
	name.sin_family = AF_INET;
	memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
	name.sin_port = htons(port);

	int r = connect(socketdescriptor, reinterpret_cast<sockaddr*>(&name), sizeof(name));

	return r;
}

int Socket::ConnectNonBlocking(int soc, const char* hostName, int port)
{
  if (soc < 0)
  {
    return -1;
  }

  int res; 
  struct sockaddr_in addr; 
  fd_set myset; 
  int valopt; 

  #if defined(_WIN32) && !defined(__CYGWIN__)
    int lon;
    int iResult;
    u_long iMode = 0;
  #else
    socklen_t lon;
    long arg; 
  #endif

	struct hostent* hp;
	hp = gethostbyname(hostName);
	if (!hp)
	{
		unsigned long addr = inet_addr(hostName);
		hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
	}

	if (!hp)
		return -2;

	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

	// Set non-blocking 
  #if defined(_WIN32) && !defined(__CYGWIN__)
    iMode = 1;
    iResult = ioctlsocket(soc, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
    {
      fprintf(stderr, "ioctlsocket failed with error: %ld\n", iResult);
      return -3;
    }
  #else
    if( (arg = fcntl(soc, F_GETFL, NULL)) < 0) 
    { 
      fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
      return -3; 
    } 
    arg |= O_NONBLOCK; 
    if( fcntl(soc, F_SETFL, arg) < 0) 
    { 
      fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
      return -3; 
    } 
  #endif
	

	// Trying to connect with timeout 
	res = connect(soc, (struct sockaddr *)&addr, sizeof(addr)); 
	if (res < 0) 
	{ 
		bool busy = false;

		#if defined(_WIN32) && !defined(__CYGWIN__)
      int serr = WSAGetLastError();
      if (serr == WSAEWOULDBLOCK || serr == WSAEINPROGRESS) 
				busy = true;
		#else
			if (errno == EINPROGRESS)
				busy = true;
		#endif
		
		if (busy == true) 
		{ 
			do 
			{ 
				FD_ZERO(&myset); 
				FD_SET(soc, &myset); 
				
				res = select(soc+1, NULL, &myset, NULL, &m_ConnectionTimeout); 

				bool selectErr = false;

				#if defined(_WIN32) && !defined(__CYGWIN__)
          int serr = WSAGetLastError();
					if (serr == WSAEINTR)
						selectErr = true;
				#else
					if (errno == EINTR)
						selectErr = true;
				#endif

				if (res < 0 && selectErr == true) 
				{ 
          handle_error("connect");
					return -4; 
				} 
				else if (res > 0) 
				{ 
					// Socket selected for write 
					lon = sizeof(int); 

					bool sockoptErr = 0;

					#if defined(_WIN32) && !defined(__CYGWIN__)
						if (getsockopt(soc, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &lon) < 0)
							sockoptErr = true;
					#else
						if (getsockopt(soc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
							sockoptErr = true;
					#endif

					if (sockoptErr == true) 
					{ 
            handle_error("getsockopt");
						return -5; 
					} 
					
					// Check the value returned... 
					if (valopt) 
					{ 
						fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt)); 
						return -6; 
					} 

          //Successfully connected to remote
					break; 
				} 
				else 
				{ 
					fprintf(stderr, "Connection timeout - Cancelling!\n"); 
					return -7; 
				} 
			} while (1); 
		} 
		else 
		{ 
      handle_error("connect");
			return -8; 
		} 
	} 

	// Set to blocking mode again... 
	#if defined(_WIN32) && !defined(__CYGWIN__)
		iMode = 0;
		iResult = ioctlsocket(soc, FIONBIO, &iMode);
		if (iResult != NO_ERROR)
		{
			fprintf(stderr, "ioctlsocket failed with error: %ld\n", iResult);
			return -9;
		}
	#else
		if( (arg = fcntl(soc, F_GETFL, NULL)) < 0) 
		{ 
			fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
			return -9; 
		} 

		arg &= (~O_NONBLOCK); 
		if( fcntl(soc, F_SETFL, arg) < 0) 
		{ 
			fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
			return -9; 
		}
	#endif

	return 0;
}

//-----------------------------------------------------------------------------
int Socket::GetPort(int sock)
{
  struct sockaddr_in sockinfo;
  memset(&sockinfo, 0, sizeof(sockinfo));

  #if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
    socklen_t sizebuf = sizeof(sockinfo);
  #else
    int sizebuf = sizeof(sockinfo);
  #endif
  
  // FIXME: Setup configuration for VTK_HAVE_GETSOCKNAME_WITH_SOCKLEN_T so we can uncomment these lines
  if(getsockname(sock, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf) != 0)
  {
    return 0;
  }
  return ntohs(sockinfo.sin_port);
}
//-----------------------------------------------------------------------------
int Socket::CloseSocket() 
{
  if (!this->IsValid())
  {
    return -1;
  }
  int ret = this->CloseSocket(this->m_SocketDescriptor);
  this->m_SocketDescriptor = -1;

  return ret;
}


//-----------------------------------------------------------------------------
int Socket::CloseSocket(int socketdescriptor)
{
  //"Not checking the return value of close() is a common but nevertheless serious programming error."

  int err = 0;

  if (socketdescriptor < 0)
  {
    return -1;
  }

  #if defined(_WIN32) && !defined(__CYGWIN__)
    err = closesocket(socketdescriptor);
  #else
    err = close(socketdescriptor);
  #endif

  if (err != 0)
  {
    #if defined(_WIN32) && !defined(__CYGWIN__)
      int serr = WSAGetLastError();
      if (serr == WSAENOTSOCK)
      {
        std::cerr <<"Cannot close socket, handler is invalid." <<std::endl;
        return 0;
      }
      else
      {
        handle_error("close");
        return err;
      }
    #else
    	if (errno == EINVAL || errno == EBADF)
      {
        std::cerr <<"Cannot close socket, handler is invalid." <<std::endl;
        return 0;
      }
      else
      {
        handle_error("close");
        return err;
      }

    #endif
  }

  return err;
}

//-----------------------------------------------------------------------------
int Socket::Send(const void* data, int length)
{
  if (!this->IsValid())
  {
    return 0;
  }

  if (length == 0)
  {
    // nothing to send.
    return 1;
  }
  //std::cerr <<"Preparing to send...";
  const char* buffer = reinterpret_cast<const char*>(data);
  int total = 0;
  int n = 0;

  int flags;
  #if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0; //disable signal on Win boxes.
  #elif defined(__linux__)
    flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
  #elif defined(__APPLE__)
    int opt=1;
    //int sock_buf_size = 47500;
    int ret = 0;
    ret = setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_NOSIGPIPE, (char*) &opt, sizeof(int));
    //ret = setsockopt(this->m_SocketDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(int) );
    //ret = setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDBUF,(char *)&sock_buf_size, sizeof(sock_buf_size) );
    flags = SO_NOSIGPIPE; //disable signal on Mac boxes.
  #endif
  
  do
  {
    n = 0;

    try
    {
      n = send(this->m_SocketDescriptor, buffer+total, length-total, flags);
    }

    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return -1;
    }

    if (n <= 0)
      return -1;

    total += n;

  } while(total < length);

  //std::cerr <<"Number of bytes sent: " <<total <<std::endl;

  return 1;
}

//-----------------------------------------------------------------------------
int Socket::Receive(void* data, int length, int readFully/*=1*/)
{
  if (!this->IsValid())
  {
    return -1;
  }

  char* buffer = reinterpret_cast<char*>(data);
  int total       = 0;
  int bytesRead   = 0;
  int rVal        = 0;
  int flags       = 0;

  #if defined(_WIN32) && !defined(__CYGWIN__)
    u_long bytesToRead = 0;
  #else
    int bytesToRead = 0;
  #endif

  // Take a look at the buffer to find out the number of bytes that arrived (if any)
  try
  {
    #if defined(_WIN32) && !defined(__CYGWIN__)
      u_long iMode = 1;
      rVal  = ioctlsocket(this->m_SocketDescriptor, FIONBIO, &iMode);
      rVal &= ioctlsocket(this->m_SocketDescriptor, FIONREAD, &bytesToRead);
    #else
      rVal = ioctl(this->m_SocketDescriptor, FIONREAD, &bytesToRead);
    #endif
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -2;
  }

  if (rVal < 0)
  {
    handle_error("ioctl: ");
    return -3;
  }
  
  //std::cerr <<"Number of bytes received: " <<bytesToRead <<" Total to read: " <<length <<std::endl;

  // SET BLOCKING MODE - NOT REQUIRED ON WIN
  //#if defined(_WIN32) && !defined(__CYGWIN__)
  //  u_long iMode = 0;
  //  rVal  = ioctlsocket(this->m_SocketDescriptor, FIONBIO, &iMode);
  //#endif
  
  if (bytesToRead == 0)       // Nothing to do, return
  {
    return 0;
  }
  else if (bytesToRead == 2)  // Receive a keepalive message
  {
    total = 0;
    length = 2;
    memset(buffer, 0, length);

    // Try reading from the socket
    try
    {
      bytesRead = recv(this->m_SocketDescriptor, buffer+total, length-total, flags);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return -2;
    }

    // recv() returned with error
    if (bytesRead < 0)
    {
      handle_error("recv: ");
      return bytesRead;
    }
    
    //std::cerr <<"Number of bytes actually read: " <<bytesRead <<std::endl;
    //std::cerr <<"Chars of the message: " <<buffer[0] <<buffer[1] <<std::endl;
    
    //otherwise return the number of bytes
    return bytesRead;
  }

  memset(buffer, 0, length);
  flags = 0;

  #if defined(_WIN32) && !defined(__CYGWIN__)
    int trys  = 0;
  #endif

  // Receive a generic message
  do
  {
    bytesRead   = 0;
    rVal        = 0;

    // Try reading from the socket
    try
    {
      bytesRead = recv(this->m_SocketDescriptor, buffer+total, length-total, flags);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return -2;
    }

    if (bytesRead < 1)
    {
      #if defined(_WIN32) && !defined(__CYGWIN__)
        // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
        // will work if you try again.
        
        trys++;
        
        int error = WSAGetLastError();
        if (((error == WSAENOBUFS) && (trys < 1000)) || ((error == WSAEWOULDBLOCK) && (trys < 1000)))
        {
          Sleep(1);
          continue;
        }
      #endif
      
      // Error in recv()
      if (bytesRead < 0)
        handle_error("recv: ");
      
      return bytesRead;
    }

    total += bytesRead;

  } while(readFully && total < length);

  //std::cerr <<"Number of bytes actually read: " <<bytesRead <<" Total: " <<total <<std::endl;

  //std::cerr <<"First 15 char of the message: ";
  //for (int oo = 0; oo < 30; oo++)
  //  std::cerr <<buffer[oo];
  //std::cerr <<" -END\n";
  
  return total;
}


//-----------------------------------------------------------------------------
int Socket::SetTimeout(int timeout)
{
  if (!this->IsValid())
  {
    return 0;
  }

  #if defined(_WIN32) && !defined(__CYGWIN__)
    this->m_Timeout = timeout;
    int len;
  #else
    this->m_Timeout.tv_sec  = timeout/1000;          /* second */
    this->m_Timeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
    socklen_t len;
  #endif
  if ( timeout > 0 )
  {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,(char*)&(this->m_OrigTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,(char*)&(this->m_Timeout), sizeof(this->m_Timeout));
    this->m_TimeoutFlag = 1;
  }
  else if (this->m_TimeoutFlag)
  {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,(char*)&(this->m_OrigTimeout), sizeof(this->m_OrigTimeout));
    this->m_TimeoutFlag = 0;
  }

  return timeout;
}

//-----------------------------------------------------------------------------
int Socket::SetConnectionTimeout(int timeout)
{
  if (!this->IsValid())
  {
    return -1;
  }

  if (timeout < 0)
  {
    return 0;
  }

  this->m_ConnectionTimeout.tv_sec  = timeout/1000;          /* second */
  this->m_ConnectionTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */

  return timeout;
}


//-----------------------------------------------------------------------------
int Socket::Skip(int length, int skipFully/*=1*/)
{
  if (!this->IsValid())
  {
    return -1;
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
bool Socket::IsValid()
{
  // This is just a silly sanity check
  if (this == NULL)
    return false;

  bool valid = false;
  try
  {
    if (this->m_SocketDescriptor == INVALID_SOCKET)
      valid = false;
    else
      valid = true;
  }

  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return false;
  }

  try
  {
    if (this->m_SocketDescriptor < 0)
      valid &= false;
    else
      valid &= true;
  }

  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return false;
  }

  return valid;
}

//-----------------------------------------------------------------------------
bool Socket::Poke()
{
  if (!this->IsValid())
  {
    return false;
  }
  
  int total  = 0;
  int n      = 0;
  int length = 2;
  char buff[2];

  //Going to send "??" 
  buff[0] = 63;
  buff[1] = 63;
  
  int flags;
  #if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0; //disable signal on Win boxes.
    u_long iMode = 1;
    ioctlsocket(this->m_SocketDescriptor, FIONBIO, &iMode);   // Set Non-Blocking mode
  #elif defined(__linux__)
    flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
  #elif defined(__APPLE__)
    int opt=1;
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_NOSIGPIPE, (char*) &opt, sizeof(int));
    flags = SO_NOSIGPIPE; //disable signal on Mac boxes.
  #endif

  // TEST WRITE
  do
  {
    n = 0;

    try
    {
      //std::cerr <<"Sending keepalive..." <<std::endl;
      n = send(this->m_SocketDescriptor, buff+total, length-total, flags);
    }

    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      return false;
    }

    if (n < 0)
      return false;

    total += n;

  } while(total < length);

  //std::cerr <<"Number of bytes sent: " <<total <<std::endl;

  return true;
}

//-----------------------------------------------------------------------------
void Socket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


} // end of igtl namespace
