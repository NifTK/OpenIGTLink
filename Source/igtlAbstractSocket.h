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

/** \class AbstractSocket
 *  \brief Base class for BSD socket encapsulation.
 *
 * This abstract class encapsulates a BSD socket. 
 * It provides an API for basic socket operations.
 *
 * This class was largely based on the igtlSocket class
 * from the Visualization Toolkit VTK. As of issue #46,
 * this base class was added, so that original
 * (i.e. pre issue 46) OpenIGTLink functionality could
 * be split across this base class, and the derived
 * igtlSocket class, and then a new igtlNonBlockingSocket
 * could be added from the NifTK project.
 */

#ifndef __igtlAbstracSocket_h
#define __igtlAbstracSocket_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"
#include "igtlWin32Header.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
#define igtlSocketErrorMacro(x) \
  { \
  ::igtl::OStringStream igtlmsg; \
  igtlmsg << "Socket ERROR: In " __FILE__ ", line " << __LINE__ << "\n" \
          << this->GetNameOfClass() << " (" << this << "): " x   \
  std::cerr << igtlmsg.str() << " : " << WSAGetLastError() << "\n\n"; \
  }
#else
#define igtlSocketErrorMacro(x) \
  { \
  ::igtl::OStringStream igtlmsg; \
  igtlmsg << "Socket ERROR: In " __FILE__ ", line " << __LINE__ << "\n" \
          << this->GetNameOfClass() << " (" << this << "): " x << "\n";  \
  perror(igtlmsg.str().c_str()); \
  }
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#else
  #include <sys/time.h>
  #ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
  #endif
#endif

namespace igtl
{

class SocketCollection;

/// class IGTL_EXPORT AbstractSocket
class IGTLCommon_EXPORT AbstractSocket : public Object
{
 public:
  typedef AbstractSocket            Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::AbstractSocket, igtl::Object)

public:

  /// Check is the socket is alive.
  virtual bool GetConnected() const;
  
  /// Check if the supplied socket descriptor is valid.
  static bool IsValid(int socketdescriptor);

  /// Close the socket.
  /// Returns -1 on error, 0 otherwise.
  virtual int CloseSocket();
 
  /// These methods send data over the socket.
  /// Returns 1 on success, 0 on error.
  virtual int Send(const void* data, int length);

  /// Receive data from the socket.
  /// This call blocks until some data is read from the socket.
  /// When the readFully flag is set, this call will block until all the requested data is
  /// read from the socket. The readFully flag will be ignored if any timeout is active.
  /// 0 on error, -1 on timeout, else number of bytes read is returned.
  virtual int Receive(void* data, int length, int readFully=1);

  /// Set sending/receiving timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  virtual int SetTimeout(int timeout) = 0;

  /// Get socket address and port.
  /// Returns 1 on success, 0 on error.
  virtual int GetSocketAddressAndPort(std::string& address, int & port);

  /// Skip reading data from the socket.
  /// The Skip() call has been newly introduced to the igtlSocket,
  /// after the class is imported from VTK, thus the call is
  /// not available in vtkSocket class.
  virtual int Skip(int length, int skipFully=1);

protected:

  AbstractSocket();
  ~AbstractSocket();

  void PrintSelf(std::ostream& os) const;

  int m_SocketDescriptor;
  igtlGetMacro(SocketDescriptor, int);

  //BTX
  friend class vtkSocketCollection;
  //ETX
 
  /// Creates an endpoint for communication and returns the descriptor.
  /// Returns -1 on error, descriptor number otherwise.
  virtual int CreateSocket();

  /// Close the socket.
  /// Returns -1 on error, 0 otherwise.
  virtual int CloseSocket(int socketdescriptor);

  /// Binds socket to a particular port.
  /// Returns -1 on error, 0 otherwise.
  virtual int BindSocket(int socketdescriptor, int port);

  /// Selects a socket ie. waits for it to change status.
  /// Returns 1 on success; 0 on timeout; -1 on error. msec=0 implies no timeout.
  virtual int SelectSocket(int socketdescriptor, unsigned long msec);

  /// Accept a connection on a socket.
  /// Returns -1 on error. Otherwise the descriptor of the accepted socket.
  virtual int Accept(int socketdescriptor);

  /// Listen for connections on a socket.
  /// Returns -1 on error, 0 otherwise.
  virtual int Listen(int socketdescriptor);

  /// Connect to a server socket.
  /// Returns -1 on error, 0 otherwise.
  virtual int Connect(int socketdescriptor, const char* hostname, int port);

  /// Returns the port to which the socket is connected.
  /// 0 on error.
  virtual int GetPort(int socketdescriptor);

  /// Selects set of sockets. Returns 0 on timeout, -1 on error.
  /// 1 on success. Selected socket's index is returned thru 
  /// selected_index
  static int SelectSockets(const int* sockets_to_select, int size,
    unsigned long msec, int* selected_index);

private:

  AbstractSocket(const AbstractSocket&); // Not implemented.
  void operator=(const AbstractSocket&); // Not implemented.

};

} // end of igtl namespace

#endif
