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

#ifndef __ptpAnnounceMessage_h
#define __ptpAnnounceMessage_h

#include "igtlObject.h"
#include "ptpMessageBase.h"

namespace ptp
{

class IGTLCommon_EXPORT AnnounceMessage: public MessageBase
{

public:

  typedef AnnounceMessage                Self;
  typedef MessageBase                    Superclass;
  typedef igtl::SmartPointer<Self>       Pointer;
  typedef igtl::SmartPointer<const Self> ConstPointer;

  igtlTypeMacro(ptp::AnnounceMessage, ptp::MessageBase);
  igtlNewMacro(ptp::AnnounceMessage);


public:
  void SetClockQuality(ClockQuality cq);
  void GetClockQuality(ClockQuality &cq);

  void SetOriginTimeStamp(Timestamp ts);
  void GetOriginTimeStamp(Timestamp &ts);

  void SetCurrentUtcOffset(Integer16 offs);
  Integer16 GetCurrentUtcOffset(void);

  void SetGrandmasterPriority1(UInteger8 pri);
  void SetGrandmasterPriority2(UInteger8 pri);

  UInteger8 GetGrandmasterPriority1();
  UInteger8 GetGrandmasterPriority2();

  void SetGrandmasterClockQuality(ClockQuality cq);
  void GetGrandmasterClockQuality(ClockQuality &cq);

  void SetGrandmasterIdentity(ClockIdentity clockId);
  void GetGrandmasterIdentity(ClockIdentity &clockId);

  void SetStepsRemoved(UInteger16 steps);
  UInteger16 GetStepsRemoved(void);

  void SetTimeSource(Enumeration8 source);
  void GetTimeSource(Enumeration8 &source);

  void SetAnnounceSequenceId(UInteger16 id);
  UInteger16 GetAnnounceSequenceId(void);

  void SetLogAnnounceInterval(Integer8 interval);
  Integer8 GetLogAnnounceInterval(void);

protected:
  AnnounceMessage();
  ~AnnounceMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  ClockQuality  m_clockQuality;
  Timestamp     m_originTimestamp;
	Integer16     m_currentUtcOffset;
	UInteger8     m_grandmasterPriority1;
	ClockQuality  m_grandmasterClockQuality;
	UInteger8     m_grandmasterPriority2;
	ClockIdentity m_grandmasterIdentity;
	UInteger16    m_stepsRemoved;
	Enumeration8  m_timeSource; 
};


} // namespace igtl

#endif



