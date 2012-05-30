/*=========================================================================

  Program:   OpenIGTLink Library -- Example for Thread (Single Method)
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2008/01/13 19:48:38 $
  Version:   $Revision: 1.142 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "ptpReceiverThread.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

namespace ptp
{

void* ReceiverThread::ReceiveFunction(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  int id      = info->ThreadID;
  int nThread = info->NumberOfThreads;
  ThreadDataR* data = static_cast<ThreadDataR*>(info->UserData);

  bool running = true;

  //------------------------------------------------------------
  // Get user data
  int nloop = data->nloop;
  igtl::FastMutexLock::Pointer mutex = data->mutex;
  ReceiverThread::Pointer host = data->host;
  igtl::Socket::Pointer socket = data->socket;

  unsigned char * buffer = new unsigned char[1000];

  // Start processing the message queue
  while (running == true)// && m_clientConnected == true)
  {
    if (!host->isThreadRunning())
      break;

    // Check if anything has arrived
    mutex->Lock();
    int bytesPending = socket->CheckPendingData();
    mutex->Unlock();

    std::cerr <<"PTPD-L::run() bytes pending: " <<bytesPending <<std::endl;
   
    if (bytesPending <= 0)
      igtl::Sleep(100);
    else
    {
      memset(buffer, 0, 1000);
      // Receive generic header from the socket
      mutex->Lock();
      int r = socket->ReceiveMulticast(buffer);
      mutex->Unlock();

      std::cerr <<"PTPD-L::run() bytes received: " <<r <<std::endl;
      
      ptp::MessageHeader::Pointer header = ptp::MessageHeader::New();
      header->InitPack();
      memcpy(header->GetPackPointer(), buffer, HEADER_LENGTH);
      
      header->Unpack();
      std::cerr <<header->GetMessageType();
    }
  }
  
  delete buffer;
  return NULL;
}



ReceiverThread::ReceiverThread()
  :igtl::MultiThreader()
{
  m_port = -1;
  m_running = false;
  m_initialized = false;
  m_messageCounter = 0;
  m_groupname.clear();
  m_msgQueue.clear();

  m_currThreadID = -1;

  m_qMutex   = igtl::FastMutexLock::New();
  m_threader = igtl::MultiThreader::New();
}

ReceiverThread::~ReceiverThread()
{
}

bool ReceiverThread::initialize(igtl::Socket::Pointer socket, int port)
{
  if (socket.IsNull() || (socket.IsNotNull() && !socket->IsValid()) )
  {
    std::cerr << "Cannot create a ptp Receiver socket, invalid external socket specified" << std::endl;
    return false;
  }

  if (m_initialized == true)
  {
    std::cerr << "Receiver already in use!" << std::endl;
    return false;
  }

  m_extSocket.operator =(socket);
  m_port = port;

  if (!activate())
    return false;

  return true;
}

void ReceiverThread::setMutex(igtl::FastMutexLock::Pointer mtx)
{
  m_mutex.operator =(mtx);
}


void ReceiverThread::addMsgToRecievedQueue(MessageBase::Pointer msgPointer)
{
  m_qMutex->Lock();
  m_msgQueue.push_back(msgPointer);
  m_qMutex->Unlock();
}

MessageBase::Pointer ReceiverThread::getFirstMsgFromQueue(void)
{
  MessageBase::Pointer firstP;
  firstP.operator =(NULL);
  m_qMutex->Lock();
  if (!m_msgQueue.empty())
    firstP.operator =(m_msgQueue.front());
  m_msgQueue.erase(m_msgQueue.begin());
  m_qMutex->Unlock();

  return firstP;
}

int ReceiverThread::getRecievedQueueCount(void)
{
  return m_msgQueue.size();
}

void ReceiverThread::startThread(void)
{
  ThreadDataR * td = new ThreadDataR;
  td->nloop = 20;
  td->mutex.operator =(m_mutex);
  td->socket.operator =(m_extSocket);
  td->host.operator =(this);

  this->SetNumberOfThreads(1);

  //ReceiverMemFn p = &ReceiverThread::Run;
  //m_threader->SetSingleMethod((igtl::ThreadFunctionType) &ReceiveFunction, &td);
  m_qMutex->Lock();
  m_running = true;
  m_currThreadID = this->SpawnThread((igtl::ThreadFunctionType) &ReceiveFunction, td);
  //m_threader->SingleMethodExecute();
  m_qMutex->Unlock();
}

void ReceiverThread::stopThread(void)
{
  this->TerminateThread(m_currThreadID);
  m_running = false;
}

bool ReceiverThread::activate(void)
{
  if (m_mutex.IsNull())
  {
    std::cerr <<"Cannot activate ptp Receiver, mutex not set" <<std::endl;
    return false;
  }

  if (m_extSocket.IsNull() || (m_extSocket.IsNotNull() && !m_extSocket->IsValid()) )
  {
    std::cerr <<"Cannot activate ptp Receiver, socket is invalid" <<std::endl;
    return false;
  }

  m_initialized = true;

  std::cout <<"PTP Receiver successfully activated" <<std::endl;

  return true;
}

bool ReceiverThread::isThreadRunning()
{
  return m_running;
}

void ReceiverThread::setThreadFinished()
{
  m_qMutex->Lock();
  m_running = false;
  m_qMutex->Unlock();
}


void* ReceiverThread::Run(void* ptr)
{
  return NULL;
}

}


