/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlStringMessage.cxx $
  Language:  C++
  Date:      $Date: 2010-01-17 13:04:20 -0500 (Sun, 17 Jan 2010) $
  Version:   $Revision: 5575 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlStringMessage.h"

#include "igtl_header.h"
#include "igtl_string.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {

StringMessage::StringMessage():
  MessageBase()
{
  this->m_DefaultBodyType = "STRING";
  this->m_Encoding = IGTL_STRING_MESSAGE_DEFAULT_ENCODING;
  this->m_String.clear();
}

StringMessage::~StringMessage()
{
}


int StringMessage::SetString(const char* string)
{
  if (strlen(string) > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  this->m_String = string;
  return (int) this->m_String.length();
}


int StringMessage::SetString(std::string & string)
{
  if (string.length() > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  this->m_String = string;
  return (int) this->m_String.length();
}


int StringMessage::SetEncoding(igtlUint16 enc)
{
  // TODO: the argument should be validated before it is substituted
  this->m_Encoding = enc;
  return 1;
}


const char* StringMessage::GetString()
{
  return this->m_String.c_str();
}


igtlUint16 StringMessage::GetEncoding()
{
  return this->m_Encoding;
}


int StringMessage::GetBodyPackSize()
{
  // Body pack size is the sum of ENCODING, LENGTH and STRING fields
  return sizeof(igtlUint16)*2 + this->m_String.length();
}


int StringMessage::PackBody()
{
  // Allocate pack
  AllocatePack();
  igtl_string_header * string_header;
  char * string;

  // Set pointers
  string_header = (igtl_string_header*) this->m_Body;
  string        = (char *) this->m_Body + sizeof(igtlUint16)*2;

  // Copy data
  string_header->encoding = static_cast<igtlUint16>(this->m_Encoding);
  string_header->length   = static_cast<igtlUint16>(this->m_String.length());
  strncpy(string, this->m_String.c_str(), string_header->length);

  // Convert byte order from host to network
  igtl_string_convert_byte_order(string_header);

  return 1;
}

int StringMessage::UnpackBody()
{

  igtl_string_header * string_header;
  char * string;

  string_header = (igtl_string_header*) this->m_Body;
  string        = (char*) (this->m_Body + sizeof(igtlUint16)*2);

  // Convert byte order from network to host
  igtl_string_convert_byte_order(string_header);
  
  // Copy data
  this->m_Encoding = string_header->encoding;
  this->m_String.clear();
  this->m_String.append(string, string_header->length);

  return 1;
}

GetStringMessage::GetStringMessage() 
	: MessageBase() 
{ 
	this->m_DefaultBodyType  = "GET_STRING";
}

GetStringMessage::~GetStringMessage() 
{
}

StartStringMessage::StartStringMessage():
  GetStringMessage()
{
  this->m_DefaultBodyType = "STT_STRING";
  this->m_Resolution = 0;
}


StartStringMessage::~StartStringMessage()
{
}

void StartStringMessage::SetResolution(igtlUint64 res)
{
  this->m_Resolution = res; 
}


igtlUint64 StartStringMessage::GetResolution()
{
  return this->m_Resolution;
}

int StartStringMessage::GetBodyPackSize()
{
  // Only a time stamp field is in the message
  return Superclass::GetBodyPackSize() + sizeof(igtlUint64);
}

int StartStringMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint64 * )this->m_Body = this->m_Resolution;

  return 1;
}

int StartStringMessage::UnpackBody()
{
  this->m_Resolution = * (igtlUint64 * )this->m_Body;

  return 1; 
}

StopStringMessage::StopStringMessage()
	: MessageBase() 
{ 
	this->m_DefaultBodyType  = "STP_STRING";
}

StopStringMessage::~StopStringMessage()
{
}

RTSStringMessage::RTSStringMessage()
	: MessageBase()
{
	this->m_Status = 0;
	this->m_DefaultBodyType  = "RTS_STRING";
}

RTSStringMessage::~RTSStringMessage() 
{
}

int RTSStringMessage::GetBodyPackSize()
{ 
  return sizeof (igtlUint8);
}

int RTSStringMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint8 * )this->m_Body = this->m_Status;

  return 1; 
}


int RTSStringMessage::UnpackBody()
{ 
  this->m_Status = * (igtlUint8 * )this->m_Body;

  return 1; 
}

} // namespace igtl





