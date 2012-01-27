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
//  #define igtlCloseSocketMacro(sock) (closesocket(sock))
//#else
//  #define igtlCloseSocketMacro(sock) (close(sock))
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
int Socket::TestSocketRW(int socketdescriptor, unsigned long msec)
{
  if (!this->IsValid())
  {
    return -1;
  }

  fd_set rset, wset;
  char outbuff[512];     // Buffer to hold outgoing data
  char inbuff[512];      // Buffer to read incoming data into

  memset(&outbuff,0,sizeof(outbuff));
  memset(&inbuff,0,sizeof(inbuff));

  struct timeval tval;
  struct timeval* tvalptr = 0;
  if ( msec > 0 )
  {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = (msec % 1000)*1000;
    tvalptr = &tval;
  }
  
  FD_ZERO(&rset);
  FD_ZERO(&wset);
  FD_SET(socketdescriptor, &rset);
  if(strlen(outbuff)!=0) FD_SET(socketdescriptor, &wset);

  int res = select(socketdescriptor + 1, &rset, &wset, 0, tvalptr);
  if (res == 0)
  {
    return 0;//for time limit expire
  }
  else if (res < 0)
  {
    // Some error.
    return res;
  }

  int canRead  = 0;
  int canWrite = 0;
  
  if (FD_ISSET(socketdescriptor, &rset))
  { 
    FD_CLR(socketdescriptor, &rset);
    memset(&inbuff,0,sizeof(inbuff));
    if ( recv(socketdescriptor, inbuff, sizeof(inbuff)-1, 0) >= 0) 
      canRead = 1;
    memset(&inbuff, 0, sizeof(inbuff));
  }
  if (FD_ISSET(socketdescriptor, &wset))
  {
      FD_CLR(socketdescriptor, &wset);
      if (send(socketdescriptor, outbuff, strlen(outbuff), 0) >= 0)
        canWrite = 1;
      
      memset(&outbuff,0,sizeof(outbuff));
  }

  // The indicated socket has some activity on it.
  if (canRead && canWrite)
    return 3;
  else if (canRead)
    return 2;
  else if (canWrite)
    return 1;
  else
    return -1;
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
			//fprintf(stderr, "EINPROGRESS in connect() - selecting\n"); 
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
					//fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
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
						//fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
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
			//fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
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
int Socket::Receive(void* data, int length, int readFully/*=1*/)
{
  if (!this->IsValid())
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
    
    if (n < 1)
    {
      #if defined(_WIN32) && !defined(__CYGWIN__)
        // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
        // will work if you try again.
        int error = WSAGetLastError();
        if ((error == WSAENOBUFS) && (trys++ < 1000))
        {
          Sleep(1);
          continue;
        }
      #endif
      
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
    }
    total += n;
  } while(readFully && total < length);
  
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
    std::cout << e.what() << std::endl;
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
    std::cout << e.what() << std::endl;
    return false;
  }

  return valid;
}

//-----------------------------------------------------------------------------
bool Socket::IsAlive()
{
  if (!this->IsValid())
  {
    return false;
  }

  int bytes = 0;

  char buff[2];
  int length = 2;

  memset((void *)&buff, 1, sizeof(buff));

  int flags;
  #if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0; //disable signal on Win boxes.
  #elif defined(__linux__)
    flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
  #elif defined(__APPLE__)
    int opt=1;
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_NOSIGPIPE, (char*) &opt, sizeof(int));
    flags = SO_NOSIGPIPE; //disable signal on Mac boxes.
  #endif

  int total = 0;
  do
  {
    int n = 0;

    try
    {
      n = send(this->m_SocketDescriptor, buff+total, length-total, flags);
    }

    catch (std::exception& e)
    {
      std::cout << e.what() << std::endl;
      break;
    }

    if (n <= 0)
    {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Send failed.");
      return false;
    }

    total += n;

  } while(total < length);

  return true;
}

//-----------------------------------------------------------------------------
int Socket::IsAbleToRW()
{
  if (!this->IsValid())
  {
    return -1;
  }
  
  int ret = this->TestSocketRW(this->m_SocketDescriptor, 100);
  
  if (ret < 0)
  {
    handle_error("test rw: ");
    return -1;
  }
  else 
    return ret;
}

//-----------------------------------------------------------------------------
void Socket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


} // end of igtl namespace
