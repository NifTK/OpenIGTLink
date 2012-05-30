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
#ifndef __ptpdaemon_h
#define __ptpdaemon_h

#define NUM_THREADS  4
#define PTP_GROUP "225.0.0.37"

#include "ptp/ptpMessageBase.h"
#include "ptp/ptpAnnounceMessage.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"
#include "igtlMulticastSocket.h"
#include "igtlFastMutexLock.h"

#include "ptpSenderThread.h"
#include "ptpReceiverThread.h"
#include "igtlWin32Header.h"

namespace ptp
{

//typedef void *(SenderThread::*SenderMemFn)(void *ptr);


class IGTLCommon_EXPORT PTPDaemon: public igtl::Object
{
public:

  typedef ptp::PTPDaemon                  Self;
  typedef igtl::Object                    Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(ptp::PTPDaemon, igtl::Object)
  igtlNewMacro(ptp::PTPDaemon);

public:
  bool initialize(void);
  bool shutdown(void);

  bool startSync(void);
  
protected:
  PTPDaemon();
  ~PTPDaemon();

private:
  PTPDaemon(const PTPDaemon&);      // Not implemented.
  void operator=(const PTPDaemon&); // Not implemented.
  
private:
  int           m_port;
  bool          m_running;
  bool          m_initialized;
  unsigned long m_messageCounter;

  ptp::SenderThread::Pointer   m_sender;
  ptp::ReceiverThread::Pointer m_receiver;

  igtl::MulticastSocket::Pointer m_multiSocket;
  igtl::FastMutexLock::Pointer   m_mutex;

  ptp::MessageBase::Pointer m_msgToSend;
  ptp::AnnounceMessage::Pointer m_announceMsgToSend;
};

}
#endif //__ptpdaemon_h



