/*=========================================================================

Program:   Open IGT Link Library
Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlPTPMessageBase.cxx $
Language:  C++
Date:      $Date: 2010-01-17 13:38:05 -0500 (Sun, 17 Jan 2010) $
Version:   $Revision: 5577 $

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ptpMessageBase.h"
#include "igtl_util.h"
#include "ptp_header.h"

#include <string>

namespace ptp 
{

MessageBase::MessageBase()
  : igtl::Object()
{
  m_PackSize       = 0;
  m_Header         = NULL;
  m_Body           = NULL;

  m_BodySizeToRead = 0;

  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;

  //m_BodyType         = "";
  //m_DefaultBodyType  = "";
  //m_DeviceName     = "";

  m_messageType   = UNDEFINED;
  m_versionPTP    = VERSION_PTP;
  m_twoStepFlag   = false;
  m_domainNumber  = DEFAULT_DOMAIN_NUMBER;

}

MessageBase::~MessageBase()
{
  if (this->m_PackSize > 0 && this->m_Header != NULL)
  {
    delete [] m_Header;

    m_PackSize = 0;
    m_Header = NULL;
    //m_Body   = NULL;
  }
}

//MessageBase::MessageBase(const MessageBase& other)
//{
//  m_PackSize = other.m_PackSize;
//  AllocatePack(m_PackSize - HEADER_LENGTH);
//
//  memcpy(m_Header, other.m_Header, m_PackSize);
//  m_BodySizeToRead = other.m_BodySizeToRead;
//
//  m_TimeStampSec = other.m_TimeStampSec;
//  m_TimeStampSecFraction = other.m_TimeStampSecFraction;
//
//  m_IsHeaderUnpacked = other.m_IsHeaderUnpacked;
//  m_IsBodyUnpacked = other.m_IsBodyUnpacked;
//
//  m_twoStepFlag = other.m_twoStepFlag;
//  m_versionNumber = other.m_versionNumber;
//  m_domainNumber = other.m_domainNumber;
//  m_portID = other.m_portID;
//  m_msgType = other.m_msgType;
//}

//void MessageBase::operator=(const MessageBase&)
//{
//}


//void MessageBase::SetDeviceName(const char* name)
//{
//  m_DeviceName = std::string(name);
//}
//
//const char* MessageBase::GetDeviceName()
//{
//  return m_DeviceName.c_str();
//}
//
//const char* MessageBase::GetDeviceType()
//{
//  if (m_DefaultBodyType.length() > 0)
//  {
//    return m_DefaultBodyType.c_str();
//  }
//  else
//  {
//    return m_BodyType.c_str();
//  }
//}


int MessageBase::SetTimeStamp(unsigned int sec, unsigned int frac)
{
  m_TimeStampSec         = sec;
  m_TimeStampSecFraction = frac;
  return 1;
}

int MessageBase::GetTimeStamp(unsigned int* sec, unsigned int* frac)
{
  *sec  = m_TimeStampSec;
  *frac = m_TimeStampSecFraction;
  return 1;
}

void MessageBase::SetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  m_TimeStampSec = ts->GetSecond();
  m_TimeStampSecFraction = igtl_nanosec_to_frac(ts->GetNanosecond());

}

void MessageBase::GetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  ts->SetTime(m_TimeStampSec, igtl_frac_to_nanosec(m_TimeStampSecFraction));
}


int MessageBase::Pack()
{
  memset(m_Header, 0, HEADER_LENGTH);

  Octet * buf = (Octet *)m_Header;
  
  Nibble transport = 0x80;

	//* (spec annex D) */
	*(UInteger8 *) (buf + 0) = transport;
	*(UInteger4 *) (buf + 1) = m_versionPTP;
	*(UInteger8 *) (buf + 4) = m_domainNumber;

	//* TODO: this bit should have been active only for sync and PdelayResp */
	if (m_twoStepFlag)
		*(UInteger8 *) (buf + 6) = PTP_TWO_STEP;

	memset((buf + 8), 0, 8);
  memcpy((buf + 20), m_sourcePortIdentity.clockIdentity, CLOCK_IDENTITY_LENGTH);
  *(UInteger16 *) (buf + 28) = flip16(m_sourcePortIdentity.portNumber);
	*(UInteger8 *) (buf + 33) = 0x7F;
	/* Default value(spec Table 24) */

  m_IsHeaderUnpacked = 0;

  PackBody();
  m_IsBodyUnpacked   = 0;

  return 1;
}

int MessageBase::Unpack(int crccheck)
{
  int r = UNPACK_UNDEF;

  // Check if the pack exists and if it has not been unpacked.
  if (m_Header != NULL && m_PackSize >= HEADER_LENGTH && m_IsHeaderUnpacked == 0)
  {
    // Unpack (deserialize) the header
    Octet * buf = (Octet *)m_Header;

    m_transportSpecific = (*(Nibble *) (buf + 0)) >> 4;
	  m_messageType = (*(Enumeration4 *) (buf + 0)) & 0x0F;
	  m_versionPTP = (*(UInteger4 *) (buf + 1)) & 0x0F;
	  /* force reserved bit to zero if not */
	  m_messageLength = flip16(*(UInteger16 *) (buf + 2));
	  m_domainNumber = (*(UInteger8 *) (buf + 4));
	  memcpy(&m_flagField, (buf + 6), FLAG_FIELD_LENGTH);
	  memcpy(&m_correctionfield.msb, (buf + 8), 4);
	  memcpy(&m_correctionfield.lsb, (buf + 12), 4);
	  m_correctionfield.msb = flip32(m_correctionfield.msb);
	  m_correctionfield.lsb = flip32(m_correctionfield.lsb);
	  memcpy(&(m_sourcePortIdentity.clockIdentity), (buf + 20), CLOCK_IDENTITY_LENGTH);
	  m_sourcePortIdentity.portNumber = flip16(*(UInteger16 *) (buf + 28));
	  m_sequenceId = flip16(*(UInteger16 *) (buf + 30));
	  m_controlField = (*(UInteger8 *) (buf + 32));
	  m_logMessageInterval = (*(Integer8 *) (buf + 33));

    m_BodySizeToRead = m_messageLength;
    m_twoStepFlag =((int)(m_flagField[0]) == (int) PTP_TWO_STEP) ? true : false;

    // Mark as unpacked.
    m_IsHeaderUnpacked = 1;
    r |= UNPACK_HEADER;
  
  }

  // Check if the body exists and it has not been unpacked
  if (GetPackBodySize() > 0 && m_IsBodyUnpacked == 0)
  {
    //ptp_header* h   = (ptp_header*) m_Header;
    // Unpack (deserialize) the Body
    UnpackBody();
    m_IsBodyUnpacked = 1;
    r |= UNPACK_BODY;
  }

  return r;
}

void* MessageBase::GetPackPointer()
{
  return (void*) m_Header;
}

void* MessageBase::GetPackBodyPointer()
{
  return (void*) m_Body;
}

int MessageBase::GetPackSize()
{
  return m_PackSize;
}

int MessageBase::GetPackBodySize()
{
  return (m_PackSize - HEADER_LENGTH);
}

void MessageBase::AllocatePack()
{
  if (m_BodySizeToRead > 0)
  {
    // called after receiving general header
    AllocatePack(m_BodySizeToRead);
  }
  else
  {
    // called for creating pack to send
    AllocatePack(GetBodyPackSize());
  }
}

void MessageBase::InitPack()
{
  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;
  m_BodySizeToRead   = 0;

  m_messageType   = UNDEFINED;
  m_versionPTP    = VERSION_PTP;
  m_twoStepFlag   = false;
  m_domainNumber  = DEFAULT_DOMAIN_NUMBER;

  // Re-allocate header area
  AllocatePack(0);
}

void MessageBase::AllocatePack(int bodySize)
{
  if (bodySize <= 0)
  {
    bodySize = 0;
    m_IsBodyUnpacked = 0;
  }

  int s = HEADER_LENGTH + bodySize;

  if (m_Header == NULL)
  {
    // For the first time
    m_Header = new unsigned char [s];
    memset(m_Header, 0, s);
    m_IsHeaderUnpacked = 0;
    m_IsBodyUnpacked = 0;
  }
  else if (m_PackSize != s)
  {
    // If the pack area exists but needs to be reallocated
    // m_IsHeaderUnpacked status is not changed in this case.

    unsigned char* old = m_Header;
    m_Header = new unsigned char [s];
    memset(m_Header, 0, s);
    memcpy(m_Header, old, HEADER_LENGTH);

    delete [] old;
    m_IsBodyUnpacked = 0;
  }

  m_Body   = &m_Header[HEADER_LENGTH];
  m_PackSize = s;
}

int MessageBase::CopyHeader(const MessageBase* mb)
{

  if (m_Header != NULL && mb->m_Header != NULL)
  {
    memcpy(m_Header, mb->m_Header, HEADER_LENGTH);
    m_Body           = &m_Header[HEADER_LENGTH];
  }
  m_PackSize             = mb->m_PackSize;
  //m_BodyType             = mb->m_BodyType;
  //m_DeviceName           = mb->m_DeviceName;
  m_TimeStampSec         = mb->m_TimeStampSec;
  m_TimeStampSecFraction = mb->m_TimeStampSecFraction;
  m_IsHeaderUnpacked     = mb->m_IsHeaderUnpacked;
  m_IsBodyUnpacked       = mb->m_IsBodyUnpacked;

  m_BodySizeToRead       = mb->m_BodySizeToRead;
  
  m_messageType          = mb->m_messageType;
  m_versionPTP           = mb->m_versionPTP;
  m_twoStepFlag          = mb->m_twoStepFlag;
  m_domainNumber         = mb->m_domainNumber;

  return 1;
}

int MessageBase::CopyBody(const MessageBase *mb)
{
  int s = m_PackSize - HEADER_LENGTH;
  if (m_Body != NULL && mb->m_Body != NULL && s > 0)
  {
    memcpy(m_Body, mb->m_Body, s);
    return 1;
  }

  return 0;
}

int MessageBase::Copy(const MessageBase* mb)
{
  if (this == mb)
  {
    return 0;
  }

  // Check if the destination (this class) is MessageBase or
  // the source and destination class arethe same type.
  // The pack size is also checked if it is larger than the header size.
  if ((m_messageType == UNDEFINED || m_messageType == mb->m_messageType) && mb->m_PackSize >= HEADER_LENGTH)
  {
    int bodySize = mb->m_PackSize - HEADER_LENGTH;
    AllocatePack(bodySize);
    CopyHeader(mb);
    if (bodySize > 0)
    {
      CopyBody(mb);
    }
    return 1;
  }
  else
  {
    return 0;
  }
}

int MessageBase::SetMessageHeader(const MessageHeader* mb)
{ 
  return Copy(mb); 
}

int MessageBase::GetBodySizeToRead()
{ 
  return m_BodySizeToRead; 
}

void MessageBase::SetTwoStepFlag(bool flag)
{
  m_twoStepFlag = flag;
}

bool MessageBase::GetTwoStepFlag(void)
{
  return m_twoStepFlag;
}

void MessageBase::SetVersionNumber(UInteger8 vn)
{
  m_versionPTP = vn;
}

UInteger8 MessageBase::GetVersionNumber(void)
{
  return m_versionPTP;
}

void MessageBase::SetDomainNumber(UInteger8 vn)
{
  m_domainNumber = vn;
}

UInteger8 MessageBase::GetDomainNumber(void)
{
  return m_domainNumber;
}

void MessageBase::SetPortIdentity(PortIdentity pid)
{
  m_sourcePortIdentity = pid;
}

void MessageBase::GetPortIdentity(PortIdentity &pid)
{
  pid = m_sourcePortIdentity;
}

Enumeration4 MessageBase::GetMessageType(void)
{
  return m_messageType;
}
}