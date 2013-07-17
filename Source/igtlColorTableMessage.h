/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlColorTableMessage_h
#define __igtlColorTableMessage_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

/// A class for the GET_COLORT message type.
class IGTLCommon_EXPORT GetColorTableMessage: public MessageBase
{
public:
  typedef GetColorTableMessage           Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetColorTableMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetColorTableMessage);

protected:
  GetColorTableMessage();
  ~GetColorTableMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};

class IGTLCommon_EXPORT StartColorTableMessage: public GetColorTableMessage
{
public:
  typedef StartColorTableMessage            Self;
  typedef GetColorTableMessage              Superclass;
  typedef SmartPointer<Self>	            Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

  igtlTypeMacro(igtl::StartColorTableMessage, igtl::GetColorTableMessage);
  igtlNewMacro(igtl::StartColorTableMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartColorTableMessage();
  ~StartColorTableMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopColorTableMessage: public MessageBase
{
public:
  typedef StopColorTableMessage               Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopColorTableMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopColorTableMessage);

protected:
  StopColorTableMessage();
  ~StopColorTableMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};

class IGTLCommon_EXPORT RTSColorTableMessage: public MessageBase
{
public:
  typedef RTSColorTableMessage			 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };

  igtlTypeMacro(igtl::RTSColorTableMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSColorTableMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSColorTableMessage();
  ~RTSColorTableMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};

/// A class for the COLORT message type.
class IGTLCommon_EXPORT ColorTableMessage: public MessageBase
{
public:
  typedef ColorTableMessage         Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ColorTableMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ColorTableMessage);

public:

  /// Index value types (UINT8 or UINT16) and map types (UINT8, UINT16 and RGB(UINT8x3))
  enum {
    INDEX_UINT8    = 3,
    INDEX_UINT16   = 5,
    MAP_UINT8      = 3,
    MAP_UINT16     = 5,
    MAP_RGB        = 19,
  };

public:

  /// Sets the index type for the color type
  void SetIndexType(int t)     { indexType = t; };

  /// Sets the index type for the color type to 8-bit unsigned integer.
  void SetIndexTypeToUint8()   { indexType = INDEX_UINT8; };

  /// Sets the index type for the color type to 16-bit unsigned integer.
  void SetIndexTypeToUint16()  { indexType = INDEX_UINT16; };

  /// Gets the index type. Returns either INDEX_UINT8 or INDEX_UINT16.
  int  GetIndexType()          { return indexType; };

  /// Sets the scalar type of the map.
  void SetMapType(int t)       { mapType = t; };

  /// Sets the scalar type of the map to 8-bit unsigned integer.
  void SetMapTypeToUint8()     { mapType = MAP_UINT8; };

  /// Sets the scalar type of the map to 16-bit unsigned integer.
  void SetMapTypeToUint16()    { mapType = MAP_UINT16; };
  
  /// Gets the type of the map.
  int  GetMapType()            { return mapType; };
  
  /// Gets the size of the color table.
  int   GetColorTableSize();

  /// Allocates a memory area for the color table.
  void  AllocateTable();

  /// Returns a pointer to the color table.
  void* GetTablePointer();

protected:
  ColorTableMessage();
  ~ColorTableMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  /// A variable to store the index type. Either INDEX_UINT8 or INDEX_UINT16.
  int indexType;

  /// A variable to store the type of the map. Either MAP_UINT8, MAP_UINT16 or MAP_RGB.
  int mapType;

  /// A pointer to the header for the color table.
  unsigned char*  m_ColorTableHeader;

  /// A pointer to the color table data.
  unsigned char*  m_ColorTable;

};


} // namespace igtl

#endif // _igtlColorTableMessage_h


