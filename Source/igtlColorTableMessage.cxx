/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlColorTableMessage.h"

#include "igtl_header.h"
#include "igtl_colortable.h"

namespace igtl {


ColorTableMessage::ColorTableMessage():
  MessageBase()
{

  indexType = INDEX_UINT8;
  mapType   = MAP_UINT8;
  m_ColorTableHeader = NULL;
  m_ColorTable       = NULL;
  m_DefaultBodyType  = "COLORT";

}

ColorTableMessage::~ColorTableMessage()
{
}

void ColorTableMessage::AllocateTable()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  AllocatePack();
  m_ColorTableHeader = m_Body;
  m_ColorTable       = &m_ColorTableHeader[IGTL_COLORTABLE_HEADER_SIZE];
}

void* ColorTableMessage::GetTablePointer()
{
  return (void*)m_ColorTable;
}

int ColorTableMessage::GetColorTableSize()
{
  igtl_colortable_header header;

  header.indexType = this->indexType;
  header.mapType   = this->mapType;

  return (int) igtl_colortable_get_table_size(&header);

}

int ColorTableMessage::GetBodyPackSize()
{
  return GetColorTableSize() + IGTL_COLORTABLE_HEADER_SIZE;
}

int ColorTableMessage::PackBody()
{
  igtl_colortable_header* colortable_header = (igtl_colortable_header*)m_ColorTableHeader;

  colortable_header->indexType = this->indexType;
  colortable_header->mapType   = this->mapType;

  igtl_colortable_convert_byte_order(colortable_header, (void*)m_ColorTable);

  return 1;

}


int ColorTableMessage::UnpackBody()
{

  this->m_ColorTableHeader = this->m_Body;
  this->m_ColorTable       = &(this->m_Body[IGTL_COLORTABLE_HEADER_SIZE]);

  igtl_colortable_header* colortable_header = (igtl_colortable_header*)this->m_ColorTableHeader;
  igtl_colortable_convert_byte_order(colortable_header, (void*)this->m_ColorTable);

  this->indexType = colortable_header->indexType;
  this->mapType   = colortable_header->mapType;

  return 1;
}

GetColorTableMessage::GetColorTableMessage() 
	: MessageBase() 
{ 
	this->m_DefaultBodyType  = "GET_COLORT"; 
}

GetColorTableMessage::~GetColorTableMessage()
{
}

StartColorTableMessage::StartColorTableMessage()
	: GetColorTableMessage()
{
  this->m_DefaultBodyType = "STT_COLORT";
  this->m_Resolution      = 0;
}


StartColorTableMessage::~StartColorTableMessage()
{
}

void StartColorTableMessage::SetResolution(igtlUint64 res)
{
  this->m_Resolution = res; 
}


igtlUint64 StartColorTableMessage::GetResolution()
{
  return this->m_Resolution;
}

int StartColorTableMessage::GetBodyPackSize()
{
  // Only a time stamp field is in the message
  return Superclass::GetBodyPackSize() + sizeof(igtlUint64);
}

int StartColorTableMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint64 * )this->m_Body = this->m_Resolution;

  return 1;
}

int StartColorTableMessage::UnpackBody()
{
  this->m_Resolution = * (igtlUint64 * )this->m_Body;

  return 1; 
}

StopColorTableMessage::StopColorTableMessage()
	: MessageBase()
{ 
	this->m_DefaultBodyType  = "STP_COLORT"; 
}

StopColorTableMessage::~StopColorTableMessage()
{
}

RTSColorTableMessage::RTSColorTableMessage()
	: MessageBase()
{	
	this->m_Status = 0; 
	this->m_DefaultBodyType  = "RTS_COLORT";
}

RTSColorTableMessage::~RTSColorTableMessage()
{
}

int RTSColorTableMessage::GetBodyPackSize()
{ 
  return sizeof (igtlUint8);
}

int RTSColorTableMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint8 * )this->m_Body = this->m_Status;

  return 1; 
}

int RTSColorTableMessage::UnpackBody()
{ 
  this->m_Status = * (igtlUint8 * )this->m_Body;

  return 1; 
}



} // namespace igtl



