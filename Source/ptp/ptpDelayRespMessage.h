/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTransformMessage.h $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ptpDelayRespMessage_h
#define __ptpDelayRespMessage_h

#include "igtlObject.h"
#include "ptpMessageBase.h"

namespace ptp
{

class IGTLCommon_EXPORT DelayRespMessage: public MessageBase
{

public:

  typedef DelayRespMessage                Self;
  typedef MessageBase                    Superclass;
  typedef igtl::SmartPointer<Self>       Pointer;
  typedef igtl::SmartPointer<const Self> ConstPointer;

  igtlTypeMacro(ptp::DelayRespMessage, ptp::MessageBase);
  igtlNewMacro(ptp::DelayRespMessage);


public:

  void SetReceiveTimeStamp(Timestamp ts);
  void GetReceiveTimeStamp(Timestamp &ts);

  void SetRequestingPortIdentity(PortIdentity pi);
  void GetRequestingPortIdentity(PortIdentity &pi);
  
protected:
  DelayRespMessage();
  ~DelayRespMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  PortIdentity  m_requestingPortIdentity;
  Timestamp     m_receiveTimestamp;
 
};


} // namespace igtl

#endif



