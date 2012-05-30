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
#include "ptpSenderThread.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

namespace ptp
{

void* SenderThread::SendFunction(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  int id      = info->ThreadID;
  int nThread = info->NumberOfThreads;
  ThreadDataS* data = static_cast<ThreadDataS*>(info->UserData);

  bool running = data->host->isThreadRunning();
  
  //------------------------------------------------------------
  // Get user data
  int nloop = data->nloop;
  igtl::FastMutexLock::Pointer mutex = data->mutex;
  SenderThread::Pointer host = data->host;
  igtl::Socket::Pointer socket = data->socket;

  // Start processing the message queue
  while (running == true)
  {
    if (!host->isThreadRunning())
      break;

    if (host->getSendQueueCount() == 0)
    {
      igtl::Sleep(250);
      continue;
    }

    // Take the oldest message in the queue and send it to the remote host
    MessageBase::Pointer msg;
    msg.operator =(host->getFirstMsgFromQueue());

    if (msg.IsNotNull())
    {
      int ret = 0;
      
      mutex->Lock();
      ret = socket->SendMulticast(msg->GetPackPointer(), msg->GetPackSize());
      mutex->Unlock();

      if (ret <=0)
      {
        std::cerr <<"Cannot send message: Send error" <<"\n";
        break;
      }
    }
    else
      std::cerr <<"Cannot send message: igtMsg is NULL" <<"\n";
  }

  host->setThreadFinished();

  return NULL;
}



SenderThread::SenderThread()
  :igtl::MultiThreader()
{
  m_port = -1;
  m_running = false;
  m_initialized = false;
  m_messageCounter = 0;
  m_currThreadID = -1;
  m_groupname.clear();
  m_msgQueue.clear();

  m_qMutex = igtl::FastMutexLock::New();
}

SenderThread::~SenderThread()
{
}

bool SenderThread::initialize(igtl::Socket::Pointer socket, int port)
{
  if (socket.IsNull() || (socket.IsNotNull() && !socket->IsValid()) )
  {
    std::cerr << "Cannot create a ptp sender socket, invalid external socket specified" << std::endl;
    return false;
  }

  if (m_initialized == true)
  {
    std::cerr << "Sender already in use!" << std::endl;
    return false;
  }

  m_extSocket.operator =(socket);
  m_port = port;

  if (!activate())
    return false;

  return true;
}

void SenderThread::setMutex(igtl::FastMutexLock::Pointer mtx)
{
  m_mutex.operator =(mtx);
}

void SenderThread::addMsgToSendQueue(MessageBase::Pointer msgPointer)
{
  m_qMutex->Lock();
  m_msgQueue.push_back(msgPointer);
  m_qMutex->Unlock();

  if (!m_running)
    this->startThread();
}

MessageBase::Pointer SenderThread::getFirstMsgFromQueue(void)
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

int SenderThread::getSendQueueCount(void)
{
  return m_msgQueue.size();
}

void SenderThread::startThread(void)
{
  ThreadDataS * td = new ThreadDataS;
  td->nloop = 20;
  td->mutex.operator = (m_mutex);
  td->socket.operator =(m_extSocket);
  td->host.operator =(this);

  this->SetNumberOfThreads(1);

  //SenderMemFn p = &SenderThread::Run;
  //this->SetSingleMethod((igtl::ThreadFunctionType) &SendFunction, &td);

  m_qMutex->Lock();
  m_running = true;
  m_currThreadID = this->SpawnThread((igtl::ThreadFunctionType) &SendFunction, td);
  //this->SingleMethodExecute();
  m_qMutex->Unlock();
  


}

void SenderThread::stopThread(void)
{
  this->TerminateThread(m_currThreadID);
  m_currThreadID = -1;
  m_running = false;
}

bool SenderThread::activate(void)
{
  if (m_mutex.IsNull())
  {
    std::cerr <<"Cannot activate ptp sender, mutex not set" <<std::endl;
    return false;
  }

  if (m_extSocket.IsNull() || (m_extSocket.IsNotNull() && !m_extSocket->IsValid()) )
  {
    std::cerr <<"Cannot activate ptp sender, socket is invalid" <<std::endl;
    return false;
  }

  m_initialized = true;

  std::cout <<"PTP sender successfully activated" <<std::endl;

  return true;
}


void* SenderThread::Run(void* ptr)
{
  return NULL;
}

bool SenderThread::isThreadRunning()
{
  m_qMutex->Lock();
  return m_running;
  m_qMutex->Unlock();
}

void SenderThread::setThreadFinished()
{
  m_qMutex->Lock();
  m_running = false;
  m_qMutex->Unlock();
}

}


