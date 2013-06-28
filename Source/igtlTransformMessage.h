/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTransformMessage_h
#define __igtlTransformMessage_h

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

/// A class for the GET_TRANS message type.
class IGTLCommon_EXPORT GetTransformMessage: public MessageBase
{

public:

  typedef GetTransformMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetTransformMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetTransformMessage);


protected:
  GetTransformMessage();
  ~GetTransformMessage();
  
protected:
  virtual int  GetBodyPackSize() { return 0; }
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};

class IGTLCommon_EXPORT StartTransformMessage: public GetTransformMessage
{
public:
  typedef StartTransformMessage         Self;
  typedef GetTransformMessage           Superclass;
  typedef SmartPointer<Self>	          Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

  igtlTypeMacro(igtl::StartTransformMessage, igtl::GetTransformMessage);
  igtlNewMacro(igtl::StartTransformMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartTransformMessage();
  ~StartTransformMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopTransformMessage: public MessageBase
{
public:
  typedef StopTransformMessage                Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopTransformMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopTransformMessage);

protected:
  StopTransformMessage();
  ~StopTransformMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSTransformMessage: public MessageBase
{
public:
  typedef RTSTransformMessage			 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSTransformMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSTransformMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSTransformMessage();
  ~RTSTransformMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};

/// The TRANSFORM data type is used to transfer a homogeneous linear transformation
/// in 4-by-4 matrix form. One such matrix was shown earlier in equation (1).
/// Note that if a device is sending only translation and rotation, then TRANSFORM
/// is equivalent to POSITION. But TRANSFORM can also be used to transfer affine
/// transformations or simple scaling. Like IMAGE and POSITION, TRANSFORM carries
class IGTLCommon_EXPORT TransformMessage: public MessageBase
{

public:

  typedef TransformMessage               Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::TransformMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TransformMessage);


public:

  /// Sets a position (or a translation vector) in the RAS coordinate system.
  void SetPosition(float p[3]);

  /// Gets a position (or a translation vector) in the RAS coordinate system. 
  void GetPosition(float p[3]);

  /// Sets a position (or a translation vector) in the RAS coordinate system. 
  void SetPosition(float px, float py, float pz);

  /// Gets a position (or a translation vector) in the RAS coordinate system. 
  void GetPosition(float* px, float* py, float* pz);

  /// Sets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void SetNormals(float o[3][3]);

  /// Gets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void GetNormals(float o[3][3]);

  /// Sets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void SetNormals(float t[3], float s[3], float n[3]);

  /// Gets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void GetNormals(float t[3], float s[3], float n[3]);

  /// Sets rotation matrix using igtl::Matrix4x4. 
  void SetMatrix(Matrix4x4& mat);

  /// Gets rotation matrix using igtl::Matrix4x4. 
  void GetMatrix(Matrix4x4& mat);
  
  /// Gets rotation matrix as string 
  void GetMatrixAsString(std::string &str);

protected:
  TransformMessage();
  ~TransformMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  /// The transformation matrix.
  Matrix4x4 matrix;

  /// The byte array for the serialized transform data.
  unsigned char*  m_Transform;

};


} // namespace igtl

#endif // _igtlTransformMessage_h



