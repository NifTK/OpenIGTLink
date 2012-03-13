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

#ifndef __ptpPDelayRespMessage_h
#define __ptpPDelayRespMessage_h

#include "igtlObject.h"
#include "ptpMessageBase.h"

namespace ptp
{

class IGTLCommon_EXPORT PDelayRespMessage: public MessageBase
{

public:

  typedef PDelayRespMessage                Self;
  typedef MessageBase                    Superclass;
  typedef igtl::SmartPointer<Self>       Pointer;
  typedef igtl::SmartPointer<const Self> ConstPointer;

  igtlTypeMacro(ptp::PDelayRespMessage, ptp::MessageBase);
  igtlNewMacro(ptp::PDelayRespMessage);


public:

  void SetReceiveTimeStamp(Timestamp ts);
  void GetReceiveTimeStamp(Timestamp &ts);

  void SetRequestingPortIdentity(PortIdentity pi);
  void GetRequestingPortIdentity(PortIdentity &pi);
  
protected:
  PDelayRespMessage();
  ~PDelayRespMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  PortIdentity  m_sourcePortIdentity;
  PortIdentity  m_requestingPortIdentity;
  Timestamp     m_receiveTimestamp;
  Timestamp     m_requestReceiptTimestamp;
 
};


} // namespace igtl

#endif



