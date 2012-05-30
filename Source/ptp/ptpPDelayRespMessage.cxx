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

#include "ptpPDelayRespMessage.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>


namespace ptp 
{

PDelayRespMessage::PDelayRespMessage():
  MessageBase()
{
  AllocatePack();
  m_messageType = PDELAY_RESP;
}

PDelayRespMessage::~PDelayRespMessage()
{
}

int PDelayRespMessage::GetBodyPackSize()
{
  return PDELAY_RESP_LENGTH;
}

int PDelayRespMessage::PackBody()
{
  unsigned char * buf = m_Header;
  unsigned char * packed_msg = new unsigned char[PDELAY_RESP_LENGTH];
  memcpy(packed_msg, buf, HEADER_LENGTH);
  
	/* changes in header */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) & 0xF0;
	/* RAZ messageType */
	*(char *)(packed_msg + 0) = *(char *)(buf + 0) | 0x03;
	/* Table 19 */
	*(UInteger16 *) (packed_msg + 2) = flip16(PDELAY_RESP_LENGTH);
	*(UInteger8 *) (packed_msg + 4) = m_domainNumber;
	memset((packed_msg + 8), 0, 8);


	*(UInteger16 *) (packed_msg + 30) = flip16(m_sequenceId);

	*(UInteger8 *) (packed_msg + 32) = 0x05;
	/* Table 23 */
	*(Integer8 *) (packed_msg + 33) = 0x7F;
	/* Table 24 */

	/* Pdelay_resp message */
	*(UInteger16 *) (packed_msg + 34) = flip16(m_requestReceiptTimestamp.secondsField.msb);
	*(UInteger32 *) (packed_msg + 36) = flip32(m_requestReceiptTimestamp.secondsField.lsb);
	*(UInteger32 *) (packed_msg + 40) = flip32(m_requestReceiptTimestamp.nanosecondsField);
	memcpy((packed_msg + 44), m_sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);
	*(UInteger16 *) (packed_msg + 52) = flip16(m_sourcePortIdentity.portNumber);

  memcpy(m_Header, packed_msg, PDELAY_RESP_LENGTH);
  delete packed_msg;
  packed_msg = NULL;
  
  return 1;
}

int PDelayRespMessage::UnpackBody()
{
  unsigned char * buf = m_Header;
  m_requestReceiptTimestamp.secondsField.msb = flip16(*(UInteger16 *) (buf + 34));
	m_requestReceiptTimestamp.secondsField.lsb = flip32(*(UInteger32 *) (buf + 36));
	m_requestReceiptTimestamp.nanosecondsField = flip32(*(UInteger32 *) (buf + 40));
	memcpy(m_requestingPortIdentity.clockIdentity, (buf + 44), CLOCK_IDENTITY_LENGTH);
	m_requestingPortIdentity.portNumber = flip16(*(UInteger16 *) (buf + 52));
  return 1;
}

void PDelayRespMessage::SetReceiveTimeStamp(Timestamp ts)
{
}
void PDelayRespMessage::GetReceiveTimeStamp(Timestamp &ts)
{
}

void PDelayRespMessage::SetRequestingPortIdentity(PortIdentity pi)
{
}

void PDelayRespMessage::GetRequestingPortIdentity(PortIdentity &pi)
{
}

} // namespace ptp





