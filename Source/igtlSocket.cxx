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

#include "igtlSocket.h"

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

#include <string.h>

namespace igtl
{

//-----------------------------------------------------------------------------
Socket::Socket()
{
  this->m_SendTimeoutFlag = 0;
  this->m_ReceiveTimeoutFlag = 0;
}

//-----------------------------------------------------------------------------
Socket::~Socket()
{
}

//-----------------------------------------------------------------------------
int Socket::SetTimeout(int timeout)
{
  if (SetReceiveTimeout(timeout) && SetSendTimeout(timeout))
    {
    return 1;
    }
  else
    {
    return 0;
    }
}

//-----------------------------------------------------------------------------
int Socket::SetReceiveTimeout(int timeout)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  
#if defined(_WIN32) && !defined(__CYGWIN__)
  this->m_ReceiveTimeout = timeout;
  int len;
#else
  this->m_ReceiveTimeout.tv_sec  = timeout/1000;          /* second */
  this->m_ReceiveTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
  socklen_t len;
#endif
  if ( timeout > 0 )
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_ReceiveTimeout), sizeof(this->m_ReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 1;
    }
  else if (this->m_ReceiveTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), sizeof(this->m_OrigReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 0;
    }

  return timeout;
}

//-----------------------------------------------------------------------------
int Socket::SetSendTimeout(int timeout)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  
#if defined(_WIN32) && !defined(__CYGWIN__)
  this->m_SendTimeout = timeout;
  int len;
#else
  this->m_SendTimeout.tv_sec  = timeout/1000;          /* second */
  this->m_SendTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
  socklen_t len;
#endif
  if ( timeout > 0 )
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_SendTimeout), sizeof(this->m_SendTimeout));
    this->m_SendTimeoutFlag = 1;
    }
  else if (this->m_SendTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), sizeof(this->m_OrigSendTimeout));
    this->m_SendTimeoutFlag = 0;
    }

  return timeout;
}

//-----------------------------------------------------------------------------
int Socket::SetReceiveBlocking(int sw)
{
  if (!this->GetConnected())
    {
    return 0;
    }

  // If sw == 1, timeout is set to 0 (wait until it receives message)  
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (sw==0)
    {
    this->m_ReceiveTimeout = 1;
    }
  else
    {
    this->m_ReceiveTimeout = 0;
    }
  int len;
#else
  if (sw==0)
    {
    this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
    this->m_ReceiveTimeout.tv_usec = 1;          /* nanosecond */
    }
  else
    {
    this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
    this->m_ReceiveTimeout.tv_usec = 0;          /* nanosecond */
    }
  socklen_t len;
#endif
  if (sw==0)
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_ReceiveTimeout), sizeof(this->m_ReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 1;
    }
  else if (this->m_ReceiveTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), sizeof(this->m_OrigReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 0;
    }

  return sw;
}

//-----------------------------------------------------------------------------
int Socket::SetSendBlocking(int sw)
{
  if (!this->GetConnected())
    {
    return 0;
    }

  // If sw == 1, timeout is set to 0 (wait until it receives message)
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (sw==0)
    {
    this->m_ReceiveTimeout = 1;
    }
  else
    {
    this->m_ReceiveTimeout = 0;
    }
  int len;
#else
  if (sw==0)
    {
    this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
    this->m_ReceiveTimeout.tv_usec = 1;          /* nanosecond */
    }
  else
    {
    this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
    this->m_ReceiveTimeout.tv_usec = 0;          /* nanosecond */
    }
  socklen_t len;
#endif
  if (sw==0)
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_SendTimeout), sizeof(this->m_SendTimeout));
    this->m_SendTimeoutFlag = 1;
    }
  else if (this->m_SendTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), sizeof(this->m_OrigSendTimeout));
    this->m_SendTimeoutFlag = 0;
    }

  return sw;
}

//-----------------------------------------------------------------------------
void Socket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace
