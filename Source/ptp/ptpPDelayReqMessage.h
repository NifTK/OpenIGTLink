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

#ifndef __ptpPDelayReqMessage_h
#define __ptpPDelayReqMessage_h

#include "igtlObject.h"
#include "ptpMessageBase.h"

namespace ptp
{

class IGTLCommon_EXPORT PDelayReqMessage: public MessageBase
{

public:

  typedef PDelayReqMessage               Self;
  typedef MessageBase                    Superclass;
  typedef igtl::SmartPointer<Self>       Pointer;
  typedef igtl::SmartPointer<const Self> ConstPointer;

  igtlTypeMacro(ptp::PDelayReqMessage, ptp::MessageBase);
  igtlNewMacro(ptp::PDelayReqMessage);


public:
  void SetClockQuality(ClockQuality cq);
  void GetClockQuality(ClockQuality &cq);

  void SetOriginTimeStamp(Timestamp ts);
  void GetOriginTimeStamp(Timestamp &ts);

  void SetPDelayReqSequenceId(UInteger16 id);
  UInteger16 GetPDelayReqSequenceId(void);

  void SetLogAnnounceInterval(Integer8 interval);
  Integer8 GetLogAnnounceInterval(void);

protected:
  PDelayReqMessage();
  ~PDelayReqMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  ClockQuality  m_clockQuality;
  Timestamp     m_originTimestamp;
  //UInteger16    m_sentPDelayReqSequenceId;
  //Integer8      m_logPDelayReqInterval;
  
};


} // namespace igtl

#endif



