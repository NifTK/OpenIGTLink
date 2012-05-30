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
#ifndef __ptpReceiverThread_h
#define __ptpReceiverThread_h

#define NUM_THREADS  4

#include "ptp/ptpMessageBase.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"
#include "igtlSocket.h"
#include "igtlFastMutexLock.h"


namespace ptp
{

//typedef void *(SenderThread::*SenderMemFn)(void *ptr);


class IGTLCommon_EXPORT ReceiverThread: public igtl::MultiThreader
{
public:

  typedef ptp::ReceiverThread             Self;
  typedef igtl::MultiThreader             Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(ptp::ReceiverThread, igtl::MultiThreader)
  igtlNewMacro(ptp::ReceiverThread);

public:
  bool initialize(igtl::Socket::Pointer socket = 0, int port = -1);
  void setMutex(igtl::FastMutexLock::Pointer mtx);
  
  void startThread(void);
  void stopThread(void);
  bool activate(void);

  MessageBase::Pointer getFirstMsgFromQueue(void);
  void addMsgToRecievedQueue(MessageBase::Pointer);

  int getRecievedQueueCount(void);

  bool isThreadRunning();
  void setThreadFinished();

protected:
  ReceiverThread();
  ~ReceiverThread();
  
  void* Run(void* ptr);

private:
  static void* ReceiveFunction(void* ptr);

private:
  std::string m_groupname;
  std::vector<MessageBase::Pointer> m_msgQueue;  

  int           m_port;
  bool          m_running;
  bool          m_initialized;
  unsigned long m_messageCounter;
  int m_currThreadID;
  
  igtl::FastMutexLock::Pointer  m_qMutex;
  igtl::FastMutexLock::Pointer  m_mutex;
  igtl::Socket::Pointer         m_extSocket;
  igtl::MultiThreader::Pointer  m_threader;
};

typedef struct 
{
  int   nloop;
  ReceiverThread::Pointer      host;
  igtl::Socket::Pointer        socket;
  igtl::FastMutexLock::Pointer mutex;
} ThreadDataR;


}
#endif //__ptpReceiverThread_h



