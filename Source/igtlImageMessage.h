/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlImageMessage.h $
  Language:  C++
  Date:      $Date: 2011-05-06 16:55:54 -0400 (Fri, 06 May 2011) $
  Version:   $Revision: 7472 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlImageMessage_h
#define __igtlImageMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

class IGTLCommon_EXPORT GetImageMessage: public MessageBase
{
public:
  typedef GetImageMessage                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetImageMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetImageMessage);

protected:
  GetImageMessage();
  ~GetImageMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT StartImageMessage: public GetImageMessage
{
public:
  typedef StartImageMessage             Self;
  typedef GetImageMessage               Superclass;
  typedef SmartPointer<Self>	        Pointer;
  typedef SmartPointer<const Self>		ConstPointer;

  igtlTypeMacro(igtl::StartImageMessage, igtl::GetImageMessage);
  igtlNewMacro(igtl::StartImageMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartImageMessage();
  ~StartImageMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopImageMessage: public MessageBase
{
public:
  typedef StopImageMessage                Self;
  typedef MessageBase					  Superclass;
  typedef SmartPointer<Self>			  Pointer;
  typedef SmartPointer<const Self>		  ConstPointer;

  igtlTypeMacro(igtl::StopImageMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopImageMessage);

protected:
  StopImageMessage();
  ~StopImageMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};

class IGTLCommon_EXPORT RTSImageMessage: public MessageBase
{
public:
  typedef RTSImageMessage				 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSImageMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSImageMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSImageMessage();
  ~RTSImageMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};



class IGTLCommon_EXPORT ImageMessage: public MessageBase
{
public:
  typedef ImageMessage              Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ImageMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ImageMessage);

public:

  enum {
    COORDINATE_LPS,
    COORDINATE_RAS
  };

  enum {
    ENDIAN_BIG=1,
    ENDIAN_LITTLE=2
  };

  enum {
    DTYPE_SCALAR = 1,
    DTYPE_VECTOR = 3
  };

  enum {
    TYPE_INT8    = 2,
    TYPE_UINT8   = 3,
    TYPE_INT16   = 4,
    TYPE_UINT16  = 5,
    TYPE_INT32   = 6,
    TYPE_UINT32  = 7,
    TYPE_FLOAT32 = 10,
    TYPE_FLOAT64 = 11
  };


public:

  // Image dimensions.
  // SetDimensions() should be called prior to SetSubVolume(), since SetDimensions()
  // sets subvolume parameters automatically assuming that subvolume = entire volume.
  void SetDimensions(int s[3]);
  void SetDimensions(int i, int j, int k);
  void GetDimensions(int s[3]);
  void GetDimensions(int &i, int &j, int &k);

  // Sub-volume dimensions and offset.
  // SetSubVolume() should be called after calling SetDiemensions(), since SetDimensions()
  // reset the subvolume parameters automatically.
  int  SetSubVolume(int dim[3], int off[3]);
  int  SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk);
  void GetSubVolume(int dim[3], int off[3]);
  void GetSubVolume(int &dimi, int &dimj, int &dimk, int &offi, int &offj, int &offk);

  // Spacings
  void SetSpacing(float s[3]);
  void SetSpacing(float si, float sj, float sk);
  void GetSpacing(float s[3]);
  void GetSpacing(float &si, float &sj, float &sk);

  // Origin coordinate
  void SetOrigin(float p[3]);
  void SetOrigin(float px, float py, float pz);
  void GetOrigin(float p[3]);
  void GetOrigin(float &px, float &py, float &pz);

  // Image orientation
  void SetNormals(float o[3][3]);
  void SetNormals(float t[3], float s[3], float n[3]);
  void GetNormals(float o[3][3]);
  void GetNormals(float t[3], float s[3], float n[3]);

  // Number of components
  void SetNumComponents(int num);
  int  GetNumComponents();

  // Origin/orientation matrix
  void SetMatrix(Matrix4x4& mat);
  void GetMatrix(Matrix4x4& mat);

  // Set matrix set flag to prevent the matrix being thrown away during pack() and 
  // unpack(). This is useful when receiving message
  void PreserveMatrix();

  // Image scalar type
  void SetScalarType(int t)    { scalarType = t; };
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };
  int  GetScalarType()         { return scalarType; };
  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  // Endian of image scalar (default is ENDIAN_BIG)
  void SetEndian(int e)        { endian = e; };
  int  GetEndian()             { return endian; };

  // TBD: Should returned value be 64-bit integer?
  int  GetImageSize();
  int  GetSubVolumeImageSize();

  void  AllocateScalars();
  void* GetScalarPointer();

#ifdef FLAGMENTED_PACK  
  void  SetScalarPOinter(void * p);
#endif

protected:
  ImageMessage();
  ~ImageMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  int    dimensions[3];
  float  spacing[3];
  int    subDimensions[3];
  int    subOffset[3];

  Matrix4x4 matrix;

  int    endian;
  int    dataType;
  int    numComponents;
  int    scalarType;
  int    coordinate;

  bool spacingSet;
  bool normalsSet;
  bool originSet;
  bool matrixSet;

  unsigned char*  m_ImageHeader;
  unsigned char*  m_Image;

  int ScalarSizeTable[12];
};


} // namespace igtl

#endif // _igtlImageMessage_h


