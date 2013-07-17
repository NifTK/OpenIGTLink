/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlSensorMessage_h
#define __igtlSensorMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlUnit.h"

namespace igtl
{

class IGTLCommon_EXPORT GetSensorMessage: public MessageBase
{

public:

  typedef GetSensorMessage				 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetSensorMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetSensorMessage);


protected:
  GetSensorMessage();
  ~GetSensorMessage();
  
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};

class IGTLCommon_EXPORT StartSensorMessage: public GetSensorMessage
{
public:
  typedef StartSensorMessage             Self;
  typedef GetSensorMessage               Superclass;
  typedef SmartPointer<Self>	         Pointer;
  typedef SmartPointer<const Self>		 ConstPointer;

  igtlTypeMacro(igtl::StartSensorMessage, igtl::GetSensorMessage);
  igtlNewMacro(igtl::StartSensorMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartSensorMessage();
  ~StartSensorMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopSensorMessage: public MessageBase
{
public:
  typedef StopSensorMessage                   Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopSensorMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopSensorMessage);

protected:
  StopSensorMessage();
  ~StopSensorMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSSensorMessage: public MessageBase
{
public:
  typedef RTSSensorMessage			     Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSSensorMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSSensorMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSSensorMessage();
  ~RTSSensorMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};

/// SENSOR is a message type, which is used to transfer sensor reading,
/// 3-axis position, velocity, acceleration, angle, angle velocity and angle acceleration.
/// The message format is intended for manipulator control and various types of sensors.
class IGTLCommon_EXPORT SensorMessage: public MessageBase
{
public:
  typedef SensorMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::SensorMessage, igtl::MessageBase);
  igtlNewMacro(igtl::SensorMessage);

public:

  /// Sets the length of the array. Returns the resulted length.
  int          SetLength(unsigned int n);

  /// Gets the length of the array.
  unsigned int GetLength();

  /// Sets the unit for the sensor values. Returns 1 if success.
  int          SetUnit(igtlUnit unit);

  /// Sets the unit for the sensor values Returns 1 if success.
  int          SetUnit(igtl::Unit * unit);

  /// Gets the unit as 64-bit unit field defined in igtlUnit.h.
  igtlUnit     GetUnit();

  /// Gets the unit as igtl::Unit class.
  int          GetUnit(igtl::Unit * unit);

  /// Sets sensor values from an array of 64-bit floating data. Returns 1 if success.
  int          SetValue(igtlFloat64 * data);

  /// Sets the value for the i-th sensor. Returns 1 if success.
  int          SetValue(unsigned int i, igtlFloat64 value);

  /// Gets the value of the i-th sensor.
  igtlFloat64  GetValue(unsigned int i);

protected:
  SensorMessage();
  ~SensorMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  /// Length of sensor array.
  igtlUint8    m_Length;

  /// Sensor status (Reserved).
  igtlUint8    m_Status;

  /// Unit (generated by igtl::Unit::Pack()).
  igtlUnit     m_Unit;
  
  /// The array of sensor values.
  std::vector<igtlFloat64> m_Array;
};


} // namespace igtl

#endif // _igtlSensorMessage_h


