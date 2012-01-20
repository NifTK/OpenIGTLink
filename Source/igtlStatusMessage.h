/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlStatusMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlStatusMessage_h
#define __igtlStatusMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{

class IGTLCommon_EXPORT GetStatusMessage: public MessageBase
{

public:

  typedef GetStatusMessage				 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetStatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetStatusMessage);


protected:
  GetStatusMessage();
  ~GetStatusMessage();
  
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};

class IGTLCommon_EXPORT StartStatusMessage: public GetStatusMessage
{
public:
  typedef StartStatusMessage				Self;
  typedef GetStatusMessage					Superclass;
  typedef SmartPointer<Self>	            Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

  igtlTypeMacro(igtl::StartStatusMessage, igtl::GetStatusMessage);
  igtlNewMacro(igtl::StartStatusMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartStatusMessage();
  ~StartStatusMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopStatusMessage: public MessageBase
{
public:
  typedef StopStatusMessage					  Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopStatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopStatusMessage);

protected:
  StopStatusMessage();
  ~StopStatusMessage();

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSStatusMessage: public MessageBase
{
public:
  typedef RTSStatusMessage				 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSStatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSStatusMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSStatusMessage();
  ~RTSStatusMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};



class IGTLCommon_EXPORT StatusMessage: public MessageBase
{
public:
  typedef StatusMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StatusMessage);

public:

  /* Status codes -- see igtl_status.h */
  enum {
    STATUS_INVALID             = 0,
    STATUS_OK                  = 1,
    STATUS_UNKNOWN_ERROR       = 2,
    STATUS_PANICK_MODE         = 3,  /* emergency */
    STATUS_NOT_FOUND           = 4,  /* file, configuration, device etc */
    STATUS_ACCESS_DENIED       = 5,
    STATUS_BUSY                = 6,
    STATUS_TIME_OUT            = 7,  /* Time out / Connection lost */
    STATUS_OVERFLOW            = 8,  /* Overflow / Can't be reached */
    STATUS_CHECKSUM_ERROR      = 9,  /* Checksum error */
    STATUS_CONFIG_ERROR        = 10, /* Configuration error */
    STATUS_RESOURCE_ERROR      = 11, /* Not enough resource (memory, storage etc) */
    STATUS_UNKNOWN_INSTRUCTION = 12, /* Illegal/Unknown instruction */
    STATUS_NOT_READY           = 13, /* Device not ready (starting up)*/
    STATUS_MANUAL_MODE         = 14, /* Manual mode (device does not accept commands) */
    STATUS_DISABLED            = 15, /* Device disabled */
    STATUS_NOT_PRESENT         = 16, /* Device not present */
    STATUS_UNKNOWN_VERSION     = 17, /* Device version not known */
    STATUS_HARDWARE_FAILURE    = 18, /* Hardware failure */
    STATUS_SHUT_DOWN           = 19, /* Exiting / shut down in progress */
    STATUS_NUM_TYPES           = 20
  };

public:

  void        SetCode(int code);
  int         GetCode();
  void        SetSubCode(igtlInt64 subcode);
  igtlInt64   GetSubCode();
  
  void        SetErrorName(const char* name);
  const char* GetErrorName();

  void        SetStatusString(const char* str);
  const char* GetStatusString();

protected:
  StatusMessage();
  ~StatusMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  igtlUint16   m_Code;
  igtlInt64    m_SubCode;
  char         m_ErrorName[20];
  
  //BTX
  std::string  m_StatusMessageString;
  //ETX

  unsigned char*  m_StatusHeader;
  char*           m_StatusMessage;

};


} // namespace igtl

#endif // _igtlStatusMessage_h



