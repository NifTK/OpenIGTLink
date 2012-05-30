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

#include "ptpPDelayRespFollowUpMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

PDelayRespFollowUpMessage::PDelayRespFollowUpMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = PDELAY_RESP_FOLLOW_UP;
}

PDelayRespFollowUpMessage::~PDelayRespFollowUpMessage()
{
}

int PDelayRespFollowUpMessage::GetBodyPackSize()
{
  return PDELAY_RESP_FOLLOW_UP_LENGTH;
}

int PDelayRespFollowUpMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[PDELAY_RESP_FOLLOW_UP_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);
  
	/* changes in header */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
	/* RAZ messageType */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x0A;
	/* Table 19 */
	*(UInteger16 *) (packed_msg + 2)  = flip16(PDELAY_RESP_FOLLOW_UP_LENGTH);
	*(UInteger16 *) (packed_msg + 30) = flip16(m_sequenceId);
	*(UInteger8 *)  (packed_msg + 32) = 0x05;
	/* Table 23 */
	*(Integer8 *) (packed_msg + 33) = 0x7F;
	/* Table 24 */

	/* Copy correctionField of PdelayReqMessage */
	*(Integer32 *) (packed_msg + 8)  = flip32(m_correctionfield.msb);
	*(Integer32 *) (packed_msg + 12) = flip32(m_correctionfield.lsb);

	/* Pdelay_resp_follow_up message */
	*(UInteger16 *) (packed_msg + 34) = flip16(m_responseOriginTimestamp.secondsField.msb);
	*(UInteger32 *) (packed_msg + 36) = flip32(m_responseOriginTimestamp.secondsField.lsb);
	*(UInteger32 *) (packed_msg + 40) = flip32(m_responseOriginTimestamp.nanosecondsField);
	memcpy((packed_msg + 44), m_sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);
	*(UInteger16 *) (packed_msg + 52) = flip16(m_sourcePortIdentity.portNumber);

  memcpy(m_Header, packed_msg, PDELAY_RESP_FOLLOW_UP_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

int PDelayRespFollowUpMessage::UnpackBody()
{
  unsigned char * buf = m_Header;

	m_responseOriginTimestamp.secondsField.msb = flip16(*(UInteger16 *) (buf + 34));
	m_responseOriginTimestamp.secondsField.lsb = flip32(*(UInteger32 *) (buf + 36));
	m_responseOriginTimestamp.nanosecondsField = flip32(*(UInteger32 *) (buf + 40));
	memcpy(m_requestingPortIdentity.clockIdentity, (buf + 44), CLOCK_IDENTITY_LENGTH);
	m_requestingPortIdentity.portNumber = flip16(*(UInteger16 *) (buf + 52));

  return 1;
}

void PDelayRespFollowUpMessage::SetClockQuality(ClockQuality cq)
{
  m_clockQuality = cq;
}

void PDelayRespFollowUpMessage::GetClockQuality(ClockQuality &cq)
{
  cq = m_clockQuality;
}

void PDelayRespFollowUpMessage::SetOriginTimeStamp(Timestamp ts)
{
   m_responseOriginTimestamp = ts;
}

void PDelayRespFollowUpMessage::GetOriginTimeStamp(Timestamp &ts)
{
  ts = m_responseOriginTimestamp;
}

void PDelayRespFollowUpMessage::SetPDelayRespFollowUpSequenceId(UInteger16 id)
{
  m_sequenceId = id;
}

UInteger16 PDelayRespFollowUpMessage::GetPDelayRespFollowUpSequenceId(void)
{
  return m_sequenceId;
}

void PDelayRespFollowUpMessage::SetLogAnnounceInterval(Integer8 interval)
{
  m_logMessageInterval = interval;
}

Integer8 PDelayRespFollowUpMessage::GetLogAnnounceInterval(void)
{
  return m_logMessageInterval;
}

} // namespace ptp





