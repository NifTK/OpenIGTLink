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
  Module:    $RCSfile: vtkClientSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "igtlClientSocket.h"

#include <cstdio>

namespace igtl
{

//-----------------------------------------------------------------------------
ClientSocket::ClientSocket()
{
}

//-----------------------------------------------------------------------------
ClientSocket::~ClientSocket()
{
}

//-----------------------------------------------------------------------------
int ClientSocket::ConnectToServer(const char* hostName, int port)
{
  if (this->GetConnected())
    {
    igtlWarningMacro("Client connection already exists on " << hostName << ":" << port << ". Closing it.");
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = INVALID_SOCKET;
    }
  
  this->m_SocketDescriptor = this->CreateSocket();
  if (!this->GetConnected())
    {
    igtlSocketErrorMacro(<< "Failed to create socket.");
    return -1;
    }

  if (this->Connect(this->m_SocketDescriptor, hostName, port) == -1)
    {
    igtlSocketErrorMacro(<< "Failed to connect to server " << hostName << ":" << port);
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = INVALID_SOCKET;
    return -1;
    }
  return 0;
}

//-----------------------------------------------------------------------------
void ClientSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace

