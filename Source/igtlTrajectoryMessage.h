/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlImgmetaMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTrajectoryMessage_h
#define __igtlTrajectoryMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

class IGTLCommon_EXPORT GetTrajectoryMessage: public MessageBase
{
public:
  typedef GetTrajectoryMessage           Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetTrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetTrajectoryMessage);

protected:
  GetTrajectoryMessage();
  ~GetTrajectoryMessage();
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};

class IGTLCommon_EXPORT StartTrajectoryMessage: public GetTrajectoryMessage
{
public:
  typedef StartTrajectoryMessage            Self;
  typedef GetTrajectoryMessage              Superclass;
  typedef SmartPointer<Self>	            Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

  igtlTypeMacro(igtl::StartTrajectoryMessage, igtl::GetTrajectoryMessage);
  igtlNewMacro(igtl::StartTrajectoryMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartTrajectoryMessage();
  ~StartTrajectoryMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopTrajectoryMessage: public MessageBase
{
public:
  typedef StopTrajectoryMessage               Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopTrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopTrajectoryMessage);

protected:
  StopTrajectoryMessage();
  ~StopTrajectoryMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSTrajectoryMessage: public MessageBase
{
public:
  typedef RTSTrajectoryMessage			 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSTrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSTrajectoryMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSTrajectoryMessage();
  ~RTSTrajectoryMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};

class IGTLCommon_EXPORT TrajectoryElement: public Object
{
public:
  typedef TrajectoryElement                   Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::TrajectoryElement, igtl::Object);
  igtlNewMacro(igtl::TrajectoryElement);

  enum {
    TYPE_ENTRY_ONLY   = 1,
    TYPE_TARGET_ONLY  = 2,
    TYPE_ENTRY_TARGET = 3,
  };

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetGroupName(const char* grpname);
  const char*   GetGroupName()                       { return this->m_GroupName.c_str(); };

  int           SetType(igtlUint8 type);
  igtlUint8     GetType()                            { return this->m_Type; };

  void          SetRGBA(igtlUint8 rgba[4]);
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);
  void          GetRGBA(igtlUint8* rgba);
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);

  void          SetEntryPosition(igtlFloat32 position[3]);
  void          SetEntryPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  void          GetEntryPosition(igtlFloat32* position);
  void          GetEntryPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  void          SetTargetPosition(igtlFloat32 position[3]);
  void          SetTargetPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  void          GetTargetPosition(igtlFloat32* position);
  void          GetTargetPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  void          SetRadius(igtlFloat32 radius)        { this->m_Radius = radius; };
  igtlFloat32   GetRadius()                          { return this->m_Radius; };

  int           SetOwner(const char* owner);
  const char*   GetOwner()                           { return this->m_Owner.c_str(); };

protected:
  TrajectoryElement();
  ~TrajectoryElement();

protected:

  std::string   m_Name;        /* name / description (< 64 bytes)*/
  std::string   m_GroupName;   /* Can be "Labeled Trajectory", "Landmark", Fiducial", ... */
  igtlUint8     m_Type;        /* Trajectory type (see TYPE_* constants) */
  igtlUint8     m_RGBA[4];     /* Color in R/G/B/A */
  igtlFloat32   m_EntryPosition[3];  /* Coordinate of the entry point */
  igtlFloat32   m_TargetPosition[3]; /* Coordinate of the target point */
  igtlFloat32   m_Radius;      /* Radius of the trajectory. Can be 0. */
  std::string   m_Owner;       /* Device name of the ower image */
};

class IGTLCommon_EXPORT TrajectoryMessage: public MessageBase
{
public:
  typedef TrajectoryMessage              Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::TrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TrajectoryMessage);

public:
  int  AddTrajectoryElement(TrajectoryElement::Pointer& elem);
  void ClearTrajectoryElement(TrajectoryElement::Pointer& elem);

  int  GetNumberOfTrajectoryElement();
  void GetTrajectoryElement(int index, TrajectoryElement::Pointer& elem);


protected:
  TrajectoryMessage();
  ~TrajectoryMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<TrajectoryElement::Pointer> m_TrajectoryList;
  
};


} // namespace igtl

#endif // _igtlTrajectoryMessage_h



