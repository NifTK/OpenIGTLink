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
  Module:    $RCSfile: igtlSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/** \class Socket
 *  \brief BSD socket encapsulation.
 *
 * This class encapsulates a BSD socket.
 * It provides an API for basic socket operations.
 *
 * This class was largely based on the igtlSocket class
 * from the Visualization Toolkit VTK.
 *
 */

#ifndef __igtlSocket_h
#define __igtlSocket_h

#include "igtlAbstractSocket.h"

namespace igtl
{

/// class IGTL_EXPORT Socket
class IGTLCommon_EXPORT Socket : public AbstractSocket
{
 public:
  typedef Socket                    Self;
  typedef AbstractSocket            Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::Socket, igtl::AbstractSocket)
  igtlNewMacro(igtl::Socket);

public:

  /// Set sending/receiving timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  virtual int SetTimeout(int timeout);

  /// Set reciving timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  int SetReceiveTimeout(int timeout);

  /// Set sending timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  int SetSendTimeout(int timeout);

  /// Set  (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  /// minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for receiving.
  int SetReceiveBlocking(int sw);

  /// Set (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  /// minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for sending.
  int SetSendBlocking(int sw);

protected:
  Socket();
  ~Socket();

  void PrintSelf(std::ostream& os) const;

private:
  Socket(const Socket&); // Not implemented.
  void operator=(const Socket&); // Not implemented.

#if defined(_WIN32) && !defined(__CYGWIN__)
  DWORD m_SendTimeout;
  DWORD m_ReceiveTimeout;
  DWORD m_OrigSendTimeout;
  DWORD m_OrigReceiveTimeout;
#else
  struct timeval m_SendTimeout;
  struct timeval m_ReceiveTimeout;
  struct timeval m_OrigSendTimeout;
  struct timeval m_OrigReceiveTimeout;
#endif
  int m_SendTimeoutFlag;
  int m_ReceiveTimeoutFlag;
};

}

#endif
