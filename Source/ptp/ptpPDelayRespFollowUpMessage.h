/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTransformMessage.h $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ptpPDelayRespFollowUpMessage_h
#define __ptpPDelayRespFollowUpMessage_h

#include "igtlObject.h"
#include "ptpMessageBase.h"

namespace ptp
{

class IGTLCommon_EXPORT PDelayRespFollowUpMessage: public MessageBase
{

public:

  typedef PDelayRespFollowUpMessage      Self;
  typedef MessageBase                    Superclass;
  typedef igtl::SmartPointer<Self>       Pointer;
  typedef igtl::SmartPointer<const Self> ConstPointer;

  igtlTypeMacro(ptp::PDelayRespFollowUpMessage, ptp::MessageBase);
  igtlNewMacro(ptp::PDelayRespFollowUpMessage);

public:
  void SetClockQuality(ClockQuality cq);
  void GetClockQuality(ClockQuality &cq);

  void SetOriginTimeStamp(Timestamp ts);
  void GetOriginTimeStamp(Timestamp &ts);

  void SetPDelayRespFollowUpSequenceId(UInteger16 id);
  UInteger16 GetPDelayRespFollowUpSequenceId(void);

  void SetLogAnnounceInterval(Integer8 interval);
  Integer8 GetLogAnnounceInterval(void);

protected:
  PDelayRespFollowUpMessage();
  ~PDelayRespFollowUpMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  ClockQuality  m_clockQuality;
  Timestamp     m_responseOriginTimestamp;
  PortIdentity  m_sourcePortIdentity;
  PortIdentity  m_requestingPortIdentity;

  //UInteger16    m_sentPDelayRespFollowUpSequenceId;
  //Integer8      m_logPDelayRespFollowUpInterval;
  
};


} // namespace igtl

#endif



