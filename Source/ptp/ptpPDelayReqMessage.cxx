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

#include "ptpPDelayReqMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

PDelayReqMessage::PDelayReqMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = PDELAY_REQ;
}

PDelayReqMessage::~PDelayReqMessage()
{
}

int PDelayReqMessage::GetBodyPackSize()
{
  return PDELAY_REQ_LENGTH;
}

int PDelayReqMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[PDELAY_REQ_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);
  
	/* changes in header */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
	/* RAZ messageType */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x02;
	/* Table 19 */
	*(UInteger16 *) (packed_msg + 2) = flip16(PDELAY_REQ_LENGTH);
	*(UInteger16 *) (packed_msg + 30) = flip16(m_sequenceId);
	*(UInteger8 *) (packed_msg + 32) = 0x05;
	/* Table 23 */
	*(Integer8 *) (packed_msg + 33) = 0x7F;
	/* Table 24 */
	memset((packed_msg + 8), 0, 8);

	/* Pdelay_req message */
	*(UInteger16 *) (packed_msg + 34) = flip16(m_originTimestamp.secondsField.msb);
	*(UInteger32 *) (packed_msg + 36) = flip32(m_originTimestamp.secondsField.lsb);
  *(UInteger32 *) (packed_msg + 40) = flip32(m_originTimestamp.nanosecondsField);

  memcpy(m_Header, packed_msg, PDELAY_REQ_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

int PDelayReqMessage::UnpackBody()
{
  unsigned char * buf = m_Header;

  m_originTimestamp.secondsField.msb = flip16(*(UInteger16 *) (buf + 34));
  m_originTimestamp.secondsField.lsb = flip32(*(UInteger32 *) (buf + 36));
  m_originTimestamp.nanosecondsField = flip32(*(UInteger32 *) (buf + 40));

  return 1;
}

void PDelayReqMessage::SetClockQuality(ClockQuality cq)
{
  m_clockQuality = cq;
}

void PDelayReqMessage::GetClockQuality(ClockQuality &cq)
{
  cq = m_clockQuality;
}

void PDelayReqMessage::SetOriginTimeStamp(Timestamp ts)
{
   m_originTimestamp = ts;
}

void PDelayReqMessage::GetOriginTimeStamp(Timestamp &ts)
{
  ts = m_originTimestamp;
}

void PDelayReqMessage::SetPDelayReqSequenceId(UInteger16 id)
{
  m_sequenceId = id;
}

UInteger16 PDelayReqMessage::GetPDelayReqSequenceId(void)
{
  return m_sequenceId;
}

void PDelayReqMessage::SetLogAnnounceInterval(Integer8 interval)
{
  m_logMessageInterval = interval;
}

Integer8 PDelayReqMessage::GetLogAnnounceInterval(void)
{
  return m_logMessageInterval;
}

} // namespace ptp





