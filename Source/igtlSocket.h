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
 * This abstract class encapsulates a BSD socket. 
 * It provides an API for basic socket operations.
 *
 * This class was largely based on the igtlSocket class
 * from the Visualization Toolkit VTK.
 *
 */

#ifndef __igtlSocket_h
#define __igtlSocket_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"
#include "igtlWin32Header.h"
#include "igtlMessageBase.h"
#include "igtlTimeStamp.h"

#include <iostream>
#include <exception>

#if defined(_WIN32) && !defined(__CYGWIN__)
  //#include <windows.h>
    #include <Ws2tcpip.h>
    //#include <winsock2.h> 

  //#include <Ws2ipdef.h>
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <sys/time.h>
  #include <errno.h>
  #include <sys/ioctl.h>
  #ifndef INVALID_SOCKET
    #define INVALID_SOCKET -1
  #endif
#endif

namespace igtl
{

void handle_error(const char * msg);

class SocketCollection;

enum communication_protocol 
{
  P_TCP,
  P_TCP_MULTI,
  P_UDP
};

// class IGTL_EXPORT Socket
class IGTLCommon_EXPORT Socket : public Object
{
 public:
  typedef Socket              Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::Socket, igtl::Object)
  igtlNewMacro(igtl::Socket);

public:

 // ----- Status API ----

  // Description:
  // Close the socket.
  int CloseSocket();
 
  // ------ Communication API ---
  // Description:
  // These methods send data over the socket.
  // Returns 1 on success, 0 on error and raises vtkCommand::ErrorEvent.
  int Send(const void* data, int length);

  virtual int SendMulticast(const void* data, int length) { return -1; }

  virtual int ReceiveMulticast(void* data, int readFully=1) { return -1; }

  // Description:
  // Receive data from the socket.
  // This call blocks until some data is read from the socket.
  // When readFully is set, this call will block until all the
  // requested data is read from the socket.
  // 0 on error, else number of bytes read is returned. On error,
  // vtkCommand::ErrorEvent is raised.
  int Receive(void* data, int length, int readFully=1);

  // Description:
  // Check socket for pending data.
  // This call returns the amount of data that can be read in a single call, 
  // which may not be the same as the total amount of data queued on the socket. 
  int CheckPendingData(void);
  // Description:
  // Set sending/receiving timeout for the existing socket in millisecond.
  // This function should be called after opening the socket.
  int SetTimeout(int timeout);

  // Description:
  // Set connection timeout for the client socket in millisecond.
  // This function should be called before connecting.
  int SetConnectionTimeout(int timeout);
  // Description:
  // Get socket address
  int GetSocketAddressAndPort(std::string& address, int & port);

  // Description:
  // Skip reading data from the socket.
  // The Skip() call has been newly introduced to the igtlSocket,
  // after the class is imported from VTK, thus the call is
  // not available in vtkSocket class.
  int Skip(int length, int skipFully=1);

  // Description:
  // This method is for checking if the socket handler is valid or not.
  // Query should be performed before attempting to request action on the socket (e.g. Close())
  bool IsValid(void);

  // Description:
  // This method is for checking if the socket is OK for writing or not. It sends a 2 byte message through, returns with false if not possible.
  // Query should be performed to detect disconnection, also it can be used for "keepalive".
  bool Poke();

  // Description:
  // Methods for getting the send and receive timestamps from the socket
  inline void GetReceiveTimestamp(igtl::TimeStamp::Pointer &rts) {rts.operator =(m_receiveTimeStamp); }
  inline void GetSendTimestamp(igtl::TimeStamp::Pointer &sts)    {sts.operator =(m_sendTimeStamp); }
  inline igtl::TimeStamp::Pointer GetReceiveTimestamp(void) { return m_receiveTimeStamp; }
  inline igtl::TimeStamp::Pointer GetSendTimestamp(void)    { return m_sendTimeStamp; }

protected:
  Socket();
  ~Socket();

  void PrintSelf(std::ostream& os) const;

  int m_SocketDescriptor;
  igtlGetMacro(SocketDescriptor, int);

  //BTX
  friend class vtkSocketCollection;
  //ETX
 
  // Description:
  // Creates an endpoint for communication and returns the descriptor.
  // -1 indicates error.
  int CreateSocket();

  // Description:
  // Creates an endpoint with the given communication protocol and returns the descriptor.
  // -1 indicates error.
  int CreateSocket(communication_protocol proto);

  // Description:
  // Close the socket.
  int CloseSocket(int socketdescriptor);

  // Description:
  // Binds socket to a particular port.
  // Returns 0 on success other -1 is returned.
  int BindSocket(int socketdescriptor, int port);

  // Description:
  // Selects a socket ie. waits for it to change status.
  // Returns 1 on success; 0 on timeout; -1 on error. msec=0 implies
  // no timeout.
  int SelectSocket(int socketdescriptor, unsigned long msec);

  // Description:
  // Accept a connection on a socket. Returns -1 on error. Otherwise
  // the descriptor of the accepted socket.
  int Accept(int socketdescriptor);

  // Description:
  // Listen for connections on a socket. Returns 0 on success. -1 on error.
  int Listen(int socketdescriptor);

  // Description:
  // Connect to a server socket. Returns 0 on success, -1 on error.
  int Connect(int socketdescriptor, const char* hostname, int port);

  // Description:
  // Connect to a server socket using non-blocking sockets. Returns 0 on success, or the error message.
  int ConnectNonBlocking(int socketdescriptor, const char* hostname, int port);

  // Description:
  // Returns the port to which the socket is connected.
  // 0 on error.
  int GetPort(int socketdescriptor);

  // Description:
  // Selects set of sockets. Returns 0 on timeout, -1 on error.
  // 1 on success. Selected socket's index is returned thru 
  // selected_index
  static int SelectSockets(const int* sockets_to_select, int size, unsigned long msec, int* selected_index);

private:
  Socket(const Socket&); // Not implemented.
  void operator=(const Socket&); // Not implemented.

#if defined(_WIN32) && !defined(__CYGWIN__)
  DWORD m_Timeout;
  DWORD m_OrigTimeout;
#else
  struct timeval m_Timeout;
  struct timeval m_OrigTimeout;
#endif
  int m_TimeoutFlag;
  struct timeval           m_ConnectionTimeout;
  communication_protocol   m_proto;
  igtl::TimeStamp::Pointer m_receiveTimeStamp;
  igtl::TimeStamp::Pointer m_sendTimeStamp;

};

}

#endif