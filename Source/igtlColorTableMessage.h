/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlColorTableMessage.h $
  Language:  C++
  Date:      $Date: 2009-02-03 16:48:16 -0500 (Tue, 03 Feb 2009) $
  Version:   $Revision: 3634 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlColorTableMessage_h
#define __igtlColorTableMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

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

  enum {
    INDEX_UINT8    = 3,
    INDEX_UINT16   = 5,
    MAP_UINT8      = 3,
    MAP_UINT16     = 5,
    MAP_RGB        = 19,
  };

public:

  // ColorTable index type
  void SetIndexType(int t)     { indexType = t; };
  void SetIndexTypeToUint8()   { indexType = INDEX_UINT8; };
  void SetIndexTypeToUint16()  { indexType = INDEX_UINT16; };
  int  GetIndexType()          { return indexType; };

  // Map scalar type
  void SetMapType(int t)       { mapType = t; };
  void SetMapTypeToUint8()     { mapType = MAP_UINT8; };
  void SetMapTypeToUint16()    { mapType = MAP_UINT16; };
  int  GetMapType()            { return mapType; };

  // Should returned value be 64-bit integer?
  int   GetColorTableSize();
  void  AllocateTable();
  void* GetTablePointer();

protected:
  ColorTableMessage();
  ~ColorTableMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  int    indexType;
  int    mapType;

  unsigned char*  m_ColorTableHeader;
  unsigned char*  m_ColorTable;

};


} // namespace igtl

#endif // _igtlColorTableMessage_h


