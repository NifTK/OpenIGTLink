/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlStringMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlStringMessage_h
#define __igtlStringMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#define IGTL_STRING_MESSAGE_DEFAULT_ENCODING 3 /* Default encoding -- ANSI-X3.5-1968 */

namespace igtl
{

class IGTLCommon_EXPORT GetStringMessage: public MessageBase
{

public:

  typedef GetStringMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetStringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetStringMessage);


protected:
  GetStringMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_STRING"; };
  ~GetStringMessage() {};
  
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };


};

class IGTLCommon_EXPORT StartStringMessage: public GetStringMessage
{
public:
  typedef StartStringMessage             Self;
  typedef GetStringMessage               Superclass;
  typedef SmartPointer<Self>	            Pointer;
  typedef SmartPointer<const Self>			ConstPointer;

  igtlTypeMacro(igtl::StartStringMessage, igtl::GetStringMessage);
  igtlNewMacro(igtl::StartStringMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartStringMessage();
  ~StartStringMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};

class IGTLCommon_EXPORT StopStringMessage: public MessageBase
{
public:
  typedef StopStringMessage                Self;
  typedef MessageBase						  Superclass;
  typedef SmartPointer<Self>				  Pointer;
  typedef SmartPointer<const Self>			  ConstPointer;

  igtlTypeMacro(igtl::StopStringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopStringMessage);

protected:
  StopStringMessage() : MessageBase() { this->m_DefaultBodyType  = "STP_STRING"; };
  ~StopStringMessage() {};

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSStringMessage: public MessageBase
{
public:
  typedef RTSStringMessage         Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSStringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSStringMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSStringMessage() : MessageBase(), m_Status(0) { this->m_DefaultBodyType  = "RTS_STRING"; };
  ~RTSStringMessage() {};

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


class IGTLCommon_EXPORT StringMessage: public MessageBase
{
public:
  typedef StringMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StringMessage);

public:

  int        SetString(const char* string);
  int        SetString(std::string & string);
  int        SetEncoding(igtlUint16 enc);

  const char* GetString();
  igtlUint16  GetEncoding();

protected:
  StringMessage();
  ~StringMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  igtlUint16   m_Encoding;
  
  //BTX
  std::string  m_String;
  //ETX

};


} // namespace igtl

#endif // _igtlStringMessage_h



