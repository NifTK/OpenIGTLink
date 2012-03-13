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
#include "ptpDaemon.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

namespace ptp
{

PTPDaemon::PTPDaemon()
  :igtl::Object()
{
  m_port = -1;
  m_running = false;
  m_initialized = false;
  m_messageCounter = 0;
}

PTPDaemon::~PTPDaemon()
{
}

bool PTPDaemon::initialize(void)
{
  // Instanciate the mutex and the threads
  m_mutex = igtl::FastMutexLock::New();
  m_sender = ptp::SenderThread::New();
  m_receiver = ptp::ReceiverThread::New();
 
  m_sender->setMutex(m_mutex);
  m_receiver->setMutex(m_mutex);

  m_multiSocket = igtl::MulticastSocket::New();
  m_multiSocket->InitMultiCast(PTP_GROUP, 3900, 1, true);

  m_sender->initialize((igtl::Socket::Pointer)m_multiSocket, 3900);
  m_receiver->initialize((igtl::Socket::Pointer)m_multiSocket, 3900);

  // Set the flag
  if (m_mutex.IsNotNull() && m_sender.IsNotNull() && m_receiver.IsNotNull())
  {
    m_initialized = true;
    m_receiver->startThread();
  }

  return true;
}

bool PTPDaemon::shutdown(void)
{
  return true;
}

bool PTPDaemon::startSync(void)
{
  //m_msgToSend = ptp::MessageBase::New();
  //m_msgToSend->InitPack();

  //m_msgToSend->SetDomainNumber(8);
  //m_msgToSend->SetTwoStepFlag(false);
  //m_msgToSend->SetVersionNumber(1);
  //m_msgToSend->Pack();

  m_announceMsgToSend = ptp::AnnounceMessage::New();
  m_announceMsgToSend->AllocatePack();
  //m_announceMsgToSend->InitPack();

  m_announceMsgToSend->SetDomainNumber(8);
  m_announceMsgToSend->SetTwoStepFlag(false);
  m_announceMsgToSend->SetVersionNumber(1);
  m_announceMsgToSend->SetAnnounceSequenceId(5);
  m_announceMsgToSend->SetCurrentUtcOffset(10);
  m_announceMsgToSend->SetGrandmasterPriority1(1);
  m_announceMsgToSend->SetGrandmasterPriority2(2);
  m_announceMsgToSend->SetLogAnnounceInterval(20);
  m_announceMsgToSend->SetStepsRemoved(1);
  m_announceMsgToSend->SetTwoStepFlag(true);
  m_announceMsgToSend->Pack();

  //memset(m_msgToSend, 1, 34);

  std::vector<ptp::MessageBase::Pointer> m_msgQueue;
  //m_msgQueue.push_back(m_announceMsgToSend);

  m_sender->addMsgToSendQueue((ptp::MessageBase::Pointer)m_announceMsgToSend);

  return true;
}

}


