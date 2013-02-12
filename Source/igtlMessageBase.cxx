/*=========================================================================

Program:   Open IGT Link Library
Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlMessageBase.cxx $
Language:  C++
Date:      $Date: 2010-01-17 13:38:05 -0500 (Sun, 17 Jan 2010) $
Version:   $Revision: 5577 $

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include "igtlMessageBase.h"
#include "igtl_util.h"
#include "igtl_header.h"

#include <string>

namespace igtl {

MessageBase::MessageBase()
  : Object()
{
  m_PackSize       = 0;
  m_Header         = NULL;
  m_Body           = NULL;

  m_BodySizeToRead = 0;
  m_DeviceName     = "";

  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;

  m_BodyType         = "";
  m_DefaultBodyType  = "";

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

void MessageBase::SetDeviceName(const char* name)
{
  m_DeviceName = std::string(name);
}

const char* MessageBase::GetDeviceName()
{
  return m_DeviceName.c_str();
}

const char* MessageBase::GetDeviceType()
{
  if (m_DefaultBodyType.length() > 0)
  {
    return m_DefaultBodyType.c_str();
  }
  else
  {
    return m_BodyType.c_str();
  }
}


int MessageBase::SetTimeStamp(unsigned int sec, unsigned int nanosec)
{
  m_TimeStampSec     = sec;
  m_TimeStampNanosec = nanosec;
  return 1;
}

int MessageBase::GetTimeStamp(unsigned int* sec, unsigned int* nanosec)
{
  *sec     = m_TimeStampSec;
  *nanosec = m_TimeStampNanosec;
  return 1;
}

void MessageBase::SetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  m_TimeStampSec     = ts->GetSecond();
  m_TimeStampNanosec = ts->GetNanosecond();

}

void MessageBase::GetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  ts->SetTime(m_TimeStampSec, m_TimeStampNanosec);
}


int MessageBase::Pack()
{
  PackBody();
  m_IsBodyUnpacked   = 0;

  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc

  h->version   = IGTL_HEADER_VERSION;
  
  // Our preferred method for timestamping
  h->timestamp = (igtlUint64)m_TimeStampSec * 1000000000 + (igtlUint64)m_TimeStampNanosec;

  h->body_size = GetBodyPackSize();
  h->crc       = crc64((unsigned char*)m_Body, GetBodyPackSize(), crc);


  strncpy(h->name, m_DefaultBodyType.c_str(), 12);
  // TODO: this does not allow creating pack with MessageBase class...

  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

  m_IsHeaderUnpacked = 0;

  return 1;
}

int MessageBase::Unpack(int crccheck)
{
  int r = UNPACK_UNDEF;

  // Check if the pack exists and if it has not been unpacked.
  if (m_Header != NULL && m_PackSize >= IGTL_HEADER_SIZE &&
    m_IsHeaderUnpacked == 0)
  {
    // Unpack (deserialize) the header
    igtl_header* h = (igtl_header*) m_Header;
    igtl_header_convert_byte_order(h);

    m_TimeStampSec = (igtlUint64)h->timestamp / (igtlUint64)1000000000;
    m_TimeStampNanosec = (igtlUint64)h->timestamp % (igtlUint64)1000000000;

    if (h->version == IGTL_HEADER_VERSION)
    {
      m_BodySizeToRead = h->body_size;

      char bodyType[13];
      char deviceName[21];

      bodyType[12]   = '\0';
      deviceName[20] = '\0';
      strncpy(bodyType, h->name, 12);
      strncpy(deviceName, h->device_name, 20);

      m_BodyType   = bodyType;  // TODO: should check if the class is MessageBase...
      m_DeviceName = deviceName;

      // Mark as unpacked.
      m_IsHeaderUnpacked = 1;
      r |= UNPACK_HEADER;
    }
  }

  // Check if the body exists and it has not been unpacked
  if (GetPackBodySize() > 0 && m_IsBodyUnpacked == 0)
  {
    igtl_header* h   = (igtl_header*) m_Header;
    igtl_uint64  crc = crc64(0, 0, 0LL); // initial crc

    if (crccheck)
    {
      // Calculate CRC of the body
      crc = crc64((unsigned char*)m_Body, m_BodySizeToRead, crc);
    }
    else
    {
      crc = h->crc;
    }

    if (crc == h->crc)
    {
      // Unpack (deserialize) the Body
      UnpackBody();
      m_IsBodyUnpacked = 1;
      r |= UNPACK_BODY;
    }
    else
    {
      m_IsBodyUnpacked = 0;
    }
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
  return (m_PackSize - IGTL_HEADER_SIZE);
}

void MessageBase::AllocatePack()
{
  int bodyPackSize = GetBodyPackSize();
 
  if (m_BodySizeToRead > bodyPackSize)
  {
    // called after receiving general header
    AllocatePack(m_BodySizeToRead);
  }
  else
  {
    // called for creating pack to send
    AllocatePack(bodyPackSize);
  }
}

void MessageBase::InitPack()
{
  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;
  m_BodySizeToRead   = 0;

  m_DeviceName       = "";
  m_BodyType         = "";

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

  int s = IGTL_HEADER_SIZE + bodySize;

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

    unsigned char * newHeader = new unsigned char [s];
    memset(newHeader, 0, s);
    memcpy(newHeader, m_Header, IGTL_HEADER_SIZE);

    delete [] m_Header;
    m_Header = newHeader;

    m_IsBodyUnpacked = 0;
  }

  m_Body   = &m_Header[IGTL_HEADER_SIZE];
  m_PackSize = s;
}

int MessageBase::CopyHeader(const MessageBase* mb)
{

  if (m_Header != NULL && mb->m_Header != NULL)
  {
    memcpy(m_Header, mb->m_Header, IGTL_HEADER_SIZE);
    m_Body           = &m_Header[IGTL_HEADER_SIZE];
  }
  m_PackSize             = mb->m_PackSize;
  m_BodyType             = mb->m_BodyType;
  m_DeviceName           = mb->m_DeviceName;
  m_TimeStampSec         = mb->m_TimeStampSec;
  m_TimeStampNanosec     = mb->m_TimeStampNanosec;
  m_IsHeaderUnpacked     = mb->m_IsHeaderUnpacked;
  m_IsBodyUnpacked       = mb->m_IsBodyUnpacked;

  m_BodySizeToRead       = mb->m_BodySizeToRead;

  return 1;
}

int MessageBase::CopyBody(const MessageBase *mb)
{
  int s = m_PackSize - IGTL_HEADER_SIZE;
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
  if ((m_DefaultBodyType.length() == 0 || m_DefaultBodyType == mb->m_BodyType)
    && mb->m_PackSize >= IGTL_HEADER_SIZE)
  {
    int bodySize = mb->m_PackSize - IGTL_HEADER_SIZE;
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

const char* MessageBase::GetBodyType() 
{ 
  return this->m_BodyType.c_str(); 
}

int MessageBase::SetMessageHeader(const MessageHeader* mb)
{ 
  return Copy(mb); 
}

int MessageBase::GetBodySizeToRead()
{ 
  return m_BodySizeToRead; 
}


}
