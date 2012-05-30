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

#include "ptpFollowUpMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

FollowUpMessage::FollowUpMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = FOLLOW_UP;
}

FollowUpMessage::~FollowUpMessage()
{
}

int FollowUpMessage::GetBodyPackSize()
{
  return FOLLOW_UP_LENGTH;
}

int FollowUpMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[FOLLOW_UP_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);
  
	/* changes in header */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
	/* RAZ messageType */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x08;
	/* Table 19 */

	*(UInteger16 *) (packed_msg + 2) = flip16(FOLLOW_UP_LENGTH);
	*(UInteger16 *) (packed_msg + 30) = flip16(m_sequenceId - 1);
	/* sentSyncSequenceId has already been incremented in "issueSync" */
	*(UInteger8 *) (packed_msg + 32) = 0x02;
	/* Table 23 */
	*(Integer8 *) (packed_msg + 33) = m_logMessageInterval;

	/* Follow_up message */
	*(UInteger16 *) (packed_msg + 34) = flip16(m_preciseOriginTimestamp.secondsField.msb);
	*(UInteger32 *) (packed_msg + 36) = flip32(m_preciseOriginTimestamp.secondsField.lsb);
	*(UInteger32 *) (packed_msg + 40) = flip32(m_preciseOriginTimestamp.nanosecondsField);

  memcpy(m_Header, packed_msg, FOLLOW_UP_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

int FollowUpMessage::UnpackBody()
{
  unsigned char * buf = m_Header;

  m_preciseOriginTimestamp.secondsField.msb = flip16(*(UInteger16 *) (buf + 34));
	m_preciseOriginTimestamp.secondsField.lsb = flip32(*(UInteger32 *) (buf + 36));
  m_preciseOriginTimestamp.nanosecondsField = flip32(*(UInteger32 *) (buf + 40));

  return 1;
}

void FollowUpMessage::SetReceiveTimeStamp(Timestamp ts)
{
}
void FollowUpMessage::GetReceiveTimeStamp(Timestamp &ts)
{
}

void FollowUpMessage::SetRequestingPortIdentity(PortIdentity pi)
{
}

void FollowUpMessage::GetRequestingPortIdentity(PortIdentity &pi)
{
}

} // namespace ptp





