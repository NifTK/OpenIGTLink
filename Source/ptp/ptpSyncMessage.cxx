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

#include "ptpSyncMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

SyncMessage::SyncMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = SYNC;
}

SyncMessage::~SyncMessage()
{
}

int SyncMessage::GetBodyPackSize()
{
  return SYNC_LENGTH;
}

int SyncMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[SYNC_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);
  
	/* changes in header */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
	/* RAZ messageType */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x00;
	/* Table 19 */
	*(UInteger16 *) (packed_msg + 2)  = flip16(SYNC_LENGTH);
	*(UInteger16 *) (packed_msg + 30) = flip16(m_sequenceId);
	*(UInteger8 *)  (packed_msg + 32) = 0x00;
	/* Table 23 */
	*(Integer8 *) (packed_msg + 33) = m_logMessageInterval;
  memset((packed_msg + 8), 0, 8);

	/* Sync message */
	*(UInteger16 *) (packed_msg + 34) = flip16(m_originTimestamp.secondsField.msb);
	*(UInteger32 *) (packed_msg + 36) = flip32(m_originTimestamp.secondsField.lsb);
	*(UInteger32 *) (packed_msg + 40) = flip32(m_originTimestamp.nanosecondsField);

  memcpy(m_Header, packed_msg, SYNC_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

int SyncMessage::UnpackBody()
{
  unsigned char * buf = m_Header;

	m_originTimestamp.secondsField.msb = flip16(*(UInteger16 *) (buf + 34));
	m_originTimestamp.secondsField.lsb = flip32(*(UInteger32 *) (buf + 36));
	m_originTimestamp.nanosecondsField = flip32(*(UInteger32 *) (buf + 40));

  return 1;
}

void SyncMessage::SetClockQuality(ClockQuality cq)
{
  m_clockQuality = cq;
}

void SyncMessage::GetClockQuality(ClockQuality &cq)
{
  cq = m_clockQuality;
}

void SyncMessage::SetOriginTimeStamp(Timestamp ts)
{
   m_originTimestamp = ts;
}

void SyncMessage::GetOriginTimeStamp(Timestamp &ts)
{
  ts = m_originTimestamp;
}

void SyncMessage::SetSyncSequenceId(UInteger16 id)
{
  m_sequenceId = id;
}

UInteger16 SyncMessage::GetSyncSequenceId(void)
{
  return m_sequenceId;
}

void SyncMessage::SetLogAnnounceInterval(Integer8 interval)
{
  m_logMessageInterval = interval;
}

Integer8 SyncMessage::GetLogAnnounceInterval(void)
{
  return m_logMessageInterval;
}

} // namespace ptp





