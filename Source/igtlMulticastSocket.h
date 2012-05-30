/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlClientSocket.h $
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
  Module:    $RCSfile: vtkClientSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlClientSocket - Encapsulates a client socket.

#ifndef __igtlMulticastSocket_h
#define __igtlMulticastSocket_h

#include "igtlSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class ServerSocket;
class ClientSocket;

class IGTLCommon_EXPORT MulticastSocket : public Socket
{
public:
  typedef MulticastSocket           Self;
  typedef Socket                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MulticastSocket, igtl::Socket)
  igtlNewMacro(igtl::MulticastSocket);

  // Description:
  // Sets up multicast on a particular port and multicast address.
  // Returns 0 on success other -1 is returned.
  int InitMultiCast(const char* multicast_group, int port, int ttl = 1, bool loop = false);
  
  // Description:
  // This methods multicasts data over the socket.
  // Returns 1 on success, 0 on error and raises vtkCommand::ErrorEvent.
  virtual int SendMulticast(const void* data, int length);

  virtual int ReceiveMulticast(void* data, int readFully=1);


protected:
  MulticastSocket();
  ~MulticastSocket();

  void PrintSelf(std::ostream& os) const;

//BTX
  friend class ServerSocket;
  friend class ClientSocket;
//ETX
private:
  MulticastSocket(const MulticastSocket&); // Not implemented.
  void operator=(const MulticastSocket&); // Not implemented.

  int m_port;
  char * m_address;
};

}

#endif

