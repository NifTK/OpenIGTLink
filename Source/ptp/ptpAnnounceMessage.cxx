/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTransformMessage.cxx $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ptpAnnounceMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

AnnounceMessage::AnnounceMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = ANNOUNCE;
}

AnnounceMessage::~AnnounceMessage()
{
}

int AnnounceMessage::GetBodyPackSize()
{
  return ANNOUNCE_LENGTH;
}

int AnnounceMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[ANNOUNCE_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);

  /* changes in header */
  *(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
  /* RAZ messageType */
  *(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x0B;
  /* Table 19 */
  *(UInteger16 *) (packed_msg + 2)    = BYTE_SWAP_INT16(ANNOUNCE_LENGTH);
  *(UInteger16 *) (packed_msg + 30)   = BYTE_SWAP_INT16(m_sequenceId);
  *(UInteger8 *) (packed_msg + 32)    = 0x05;
  /* Table 23 */
  *(Integer8 *) (packed_msg + 33)     = m_logMessageInterval;

  /* Announce message */
  memset((packed_msg + 34), 0, 10);
  *(Integer16 *) (packed_msg + 44)    = BYTE_SWAP_INT16(m_currentUtcOffset);
  *(UInteger8 *) (packed_msg + 47)    = m_grandmasterPriority1;
  *(UInteger8 *) (packed_msg + 48)    = m_clockQuality.clockClass;
  *(Enumeration8 *) (packed_msg + 49) = m_clockQuality.clockAccuracy;
  *(UInteger16 *) (packed_msg + 50)   = BYTE_SWAP_INT16(m_clockQuality.offsetScaledLogVariance);
  *(UInteger8 *) (packed_msg + 52)    = m_grandmasterPriority2;
  memcpy((packed_msg + 53), m_grandmasterIdentity, CLOCK_IDENTITY_LENGTH);
  *(UInteger16 *) (packed_msg + 61)   = BYTE_SWAP_INT16(m_stepsRemoved);
  *(Enumeration8 *) (packed_msg + 63) = m_timeSource;

  memcpy(m_Header, packed_msg, ANNOUNCE_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

//int AnnounceMessage::UnpackBody()
//{
//  unsigned char * buf = m_Header;
//  ptp_announce * announce = new ptp_announce;
//
//  announce->originTimestamp.secondsField.msb = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 34));
//	announce->originTimestamp.secondsField.lsb = BYTE_SWAP_INT32(*(UInteger32 *) (buf + 36));
//	announce->originTimestamp.nanosecondsField = BYTE_SWAP_INT32(*(UInteger32 *) (buf + 40));
//	announce->currentUtcOffset = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 44));
//	announce->grandmasterPriority1 = *(UInteger8 *) (buf + 47);
//	announce->grandmasterClockQuality.clockClass = *(UInteger8 *) (buf + 48);
//	announce->grandmasterClockQuality.clockAccuracy = *(Enumeration8 *) (buf + 49);
//	announce->grandmasterClockQuality.offsetScaledLogVariance = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 50));
//	announce->grandmasterPriority2 = *(UInteger8 *) (buf + 52);
//	memcpy(announce->grandmasterIdentity, (buf + 53), CLOCK_IDENTITY_LENGTH);
//	announce->stepsRemoved = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 61));
//	announce->timeSource = *(Enumeration8 *) (buf + 63);
//
//  memcpy(m_Header, announce, ANNOUNCE_LENGTH);
//  delete announce;
//  announce = NULL;
//
//  return 1;
//}
int AnnounceMessage::UnpackBody()
{
  unsigned char * buf = m_Header;

  m_originTimestamp.secondsField.msb = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 34));
	m_originTimestamp.secondsField.lsb = BYTE_SWAP_INT32(*(UInteger32 *) (buf + 36));
	m_originTimestamp.nanosecondsField = BYTE_SWAP_INT32(*(UInteger32 *) (buf + 40));
	m_currentUtcOffset = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 44));
	m_grandmasterPriority1 = *(UInteger8 *) (buf + 47);
	m_grandmasterClockQuality.clockClass = *(UInteger8 *) (buf + 48);
	m_grandmasterClockQuality.clockAccuracy = *(Enumeration8 *) (buf + 49);
	m_grandmasterClockQuality.offsetScaledLogVariance = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 50));
	m_grandmasterPriority2 = *(UInteger8 *) (buf + 52);
	memcpy(&m_grandmasterIdentity, (buf + 53), CLOCK_IDENTITY_LENGTH);
	m_stepsRemoved = BYTE_SWAP_INT16(*(UInteger16 *) (buf + 61));
	m_timeSource = *(Enumeration8 *) (buf + 63);

  //memcpy(m_Header, announce, ANNOUNCE_LENGTH);
  // delete announce;
  //announce = NULL;

  return 1;
}

void AnnounceMessage::SetClockQuality(ClockQuality cq)
{
  m_clockQuality = cq;
}

void AnnounceMessage::GetClockQuality(ClockQuality &cq)
{
  cq = m_clockQuality;
}

void AnnounceMessage::SetOriginTimeStamp(Timestamp ts)
{
   m_originTimestamp = ts;
}

void AnnounceMessage::GetOriginTimeStamp(Timestamp &ts)
{
  ts = m_originTimestamp;
}

void AnnounceMessage::SetCurrentUtcOffset(Integer16 offs)
{
  m_currentUtcOffset = offs;
}

Integer16 AnnounceMessage::GetCurrentUtcOffset(void)
{
  return m_currentUtcOffset;
}

void AnnounceMessage::SetGrandmasterPriority1(UInteger8 pri)
{
   m_grandmasterPriority1 = pri;
}

void AnnounceMessage::SetGrandmasterPriority2(UInteger8 pri)
{
  m_grandmasterPriority2 = pri;
}

UInteger8 AnnounceMessage::GetGrandmasterPriority1()
{
  return m_grandmasterPriority1;
}

UInteger8 AnnounceMessage::GetGrandmasterPriority2()
{
  return m_grandmasterPriority2;
}

void AnnounceMessage::SetGrandmasterClockQuality(ClockQuality cq)
{
  m_grandmasterClockQuality = cq;
}

void AnnounceMessage::GetGrandmasterClockQuality(ClockQuality &cq)
{
  cq = m_grandmasterClockQuality;
}

void AnnounceMessage::SetGrandmasterIdentity(ClockIdentity clockId)
{
  for (int i = 0; i < 9; i++)
    m_grandmasterIdentity[i] = clockId[i];
}

void AnnounceMessage::GetGrandmasterIdentity(ClockIdentity &clockId)
{
  for (int i = 0; i < 9; i++)
    clockId[i] = m_grandmasterIdentity[i];
}

void AnnounceMessage::SetStepsRemoved(UInteger16 steps)
{
  m_stepsRemoved = steps;
}

UInteger16 AnnounceMessage::GetStepsRemoved(void)
{
  return m_stepsRemoved;
}

void AnnounceMessage::SetTimeSource(Enumeration8 source)
{
  m_timeSource = source;
}

void AnnounceMessage::GetTimeSource(Enumeration8 &source)
{
  source = m_timeSource;
}

void AnnounceMessage::SetAnnounceSequenceId(UInteger16 id)
{
  m_sequenceId = id;
}

UInteger16 AnnounceMessage::GetAnnounceSequenceId(void)
{
  return m_sequenceId;
}

void AnnounceMessage::SetLogAnnounceInterval(Integer8 interval)
{
  m_logMessageInterval = interval;
}

Integer8 AnnounceMessage::GetLogAnnounceInterval(void)
{
  return m_logMessageInterval;
}

} // namespace ptp





