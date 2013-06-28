/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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
  GetStringMessage();
  ~GetStringMessage();
  
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
  StopStringMessage();
  ~StopStringMessage();

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
  RTSStringMessage();
  ~RTSStringMessage();

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


/// THe STRING message type is used for transferring a character string. It supports character strings up to 65535 bytes.
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

  /// Sets the string by character array.
  int        SetString(const char* string);

  /// Sets the string by std::string.
  int        SetString(std::string & string);

  /// Sets the encoding of the string. For character encoding, please refer IANA Character Sets
  /// (http://www.iana.org/assignments/character-sets).
  /// US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.
  int        SetEncoding(igtlUint16 enc);

  /// Gets the string.
  const char* GetString();

  /// Gets the encoding of the string. The returned value is defined in
  /// IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16  GetEncoding();

protected:
  StringMessage();
  ~StringMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  /// The encoding of the string.
  /// The value is defined in IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16   m_Encoding;

  /// The string.
  std::string  m_String;

};


} // namespace igtl

#endif // _igtlStringMessage_h



