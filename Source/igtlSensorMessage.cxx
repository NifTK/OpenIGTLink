/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlSensorMessage.cxx $
  Language:  C++
  Date:      $Date: 2010-01-17 13:04:20 -0500 (Sun, 17 Jan 2010) $
  Version:   $Revision: 5575 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlSensorMessage.h"

#include "igtl_header.h"
#include "igtl_sensor.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {

SensorMessage::SensorMessage():
  MessageBase()
{
  this->m_Length = 0;
  this->m_Status = 0;
  this->m_Unit   = 0;
  this->m_Array.clear();

  this->m_DefaultBodyType = "SENSOR";
}


SensorMessage::~SensorMessage()
{
}

  
int SensorMessage::SetLength(unsigned int n)
{
  if (n <= 256)
    {
    this->m_Length = n;
    this->m_Array.resize(n);
    }

  return (int) this->m_Length;
}


unsigned int SensorMessage::GetLength()
{
  return this->m_Length;
}


int SensorMessage::SetUnit(igtlUnit unit)
{
  this->m_Unit = unit;
  return 1;
}


int SensorMessage::SetUnit(igtl::Unit * unit)
{
  this->m_Unit = unit->Pack();

  return 1;
}


igtlUnit SensorMessage::GetUnit()
{
  return this->m_Unit;
}


int SensorMessage::GetUnit(igtl::Unit * unit)
{
  return unit->Unpack(this->m_Unit);
}


int SensorMessage::SetValue(igtlFloat64 * data)
{
  for (int i = 0; i < this->m_Length; i ++)
    {
    this->m_Array[i] = data[i];
    }

  return 1;
}


int SensorMessage::SetValue(unsigned int i, igtlFloat64 value)
{
  if (i < this->m_Length)
    {
    this->m_Array[i] = value;
    return 1;
    }

  return 0;
}


igtlFloat64 SensorMessage::GetValue(unsigned int i)
{
  if (i < this->m_Length)
    {
    return this->m_Array[i];
    }
  else
    {
    return 0.0;
    }
}


int SensorMessage::GetBodyPackSize()
{
  // Body pack size is the sum of LARRAY, STATUS, UNIT and DATA
  return sizeof(igtlUint8)*2 + sizeof(igtlUnit) + sizeof(igtlFloat64)*this->m_Length;
}


int SensorMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

  igtl_sensor_header * sensor_header;
  igtl_float64       * data;

  // Set pointers
  sensor_header = (igtl_sensor_header *) this->m_Body;
  data = (igtl_float64 *) (this->m_Body + sizeof(igtl_sensor_header));

  // Copy data
  sensor_header->larray = this->m_Length;
  sensor_header->status = this->m_Status;
  sensor_header->unit   = this->m_Unit;

  for (int i = 0; i < this->m_Length; i ++)
    {
    data[i] = this->m_Array[i];
    }

  // Convert byte order from local to network
  igtl_sensor_convert_byte_order(sensor_header, data);

  return 1;
}

int SensorMessage::UnpackBody()
{

  igtl_sensor_header * sensor_header;
  igtl_float64       * data;

  // Set pointers
  sensor_header = (igtl_sensor_header *) this->m_Body;
  data = (igtl_float64 *) (this->m_Body + sizeof(igtl_sensor_header));

  // Convert byte order from local to network  
  igtl_sensor_convert_byte_order(sensor_header, data);

  // Copy data
  this->m_Length = sensor_header->larray;
  this->m_Status = sensor_header->status;
  this->m_Unit   = sensor_header->unit;

  this->m_Array.resize(this->m_Length);
  for (int i = 0; i < this->m_Length; i ++)
    {
    this->m_Array[i] = data[i];
    }

  return 1;
}

GetSensorMessage::GetSensorMessage() 
	: MessageBase() 
  { 
	  this->m_DefaultBodyType  = "GET_SENSOR"; 
  }
 
GetSensorMessage::~GetSensorMessage() 
{
}

StartSensorMessage::StartSensorMessage()
	: GetSensorMessage()
{
  this->m_DefaultBodyType = "STT_SENSOR";
  this->m_Resolution      = 0;
}


StartSensorMessage::~StartSensorMessage()
{
}

void StartSensorMessage::SetResolution(igtlUint64 res)
{
  this->m_Resolution = res; 
}


igtlUint64 StartSensorMessage::GetResolution()
{
  return this->m_Resolution;
}

int StartSensorMessage::GetBodyPackSize()
{
  // Only a time stamp field is in the message
  return Superclass::GetBodyPackSize() + sizeof(igtlUint64);
}

int StartSensorMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint64 * )this->m_Body = this->m_Resolution;

  return 1;
}

int StartSensorMessage::UnpackBody()
{
  this->m_Resolution = * (igtlUint64 * )this->m_Body;

  return 1; 
}

StopSensorMessage::StopSensorMessage()
	: MessageBase()
{ 
	this->m_DefaultBodyType  = "STP_SENSOR"; 
}

StopSensorMessage::~StopSensorMessage()
{
}

RTSSensorMessage::RTSSensorMessage()
	: MessageBase()
{	
	this->m_Status = 0; 
	this->m_DefaultBodyType  = "RTS_SENSOR";
}

RTSSensorMessage::~RTSSensorMessage()
{
}

int RTSSensorMessage::GetBodyPackSize()
{ 
  return sizeof (igtlUint8);
}

int RTSSensorMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint8 * )this->m_Body = this->m_Status;

  return 1; 
}

int RTSSensorMessage::UnpackBody()
{ 
  this->m_Status = * (igtlUint8 * )this->m_Body;

  return 1; 
}

} // namespace igtl





