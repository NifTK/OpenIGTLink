/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTimeStamp.h $
  Language:  C++
  Date:      $Date: 2011-03-24 00:08:23 -0400 (Thu, 24 Mar 2011) $
  Version:   $Revision: 7354 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTimeStamp_h
#define __igtlTimeStamp_h

#include "igtlMacro.h"
#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlTypes.h"
#include "igtlOSUtil.h"

#if defined(WIN32) || defined(_WIN32)
  #include <windows.h>
  #include <ctime>
  #include "tsctime/TSCtime.h"
#else
  #include <sys/time.h>
#endif  // defined(WIN32) || defined(_WIN32)

namespace igtl
{

class IGTLCommon_EXPORT TimeStamp : public Object
{
public:
  typedef TimeStamp                  Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  igtlTypeMacro(TimeStamp, Object);
  igtlNewMacro(Self);

  /** Returns the frequency of a clock */
  igtlGetConstMacro(Frequency,  igtlUint32);
  igtlGetConstMacro(Second,     igtlUint32);
  igtlGetConstMacro(Nanosecond, igtlUint32);

  void GetTime();
  void GetTime_TAI();
  
  void toUTC();
  void toTAI();
  inline bool isUTC() {return m_UTC; }
  
  void SetTime(double tm);
  void SetTime(igtlUint32 second, igtlUint32 nanosecond);
  void SetTime(igtlUint64 tm);  /* 64-bit fixed-point expression used in OpenIGTLink */

  double GetTimeStamp();
  void   GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond);
  igtlUint64 GetTimeStampUint64();     /* 64-bit fixed-point expression used in OpenIGTLink */


protected:

  /** constructor */
  TimeStamp();

  /** destructor */
  virtual ~TimeStamp();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os) const;

private:

  igtlUint32       m_Frequency;   /* Clock frequency (Hz)*/
  igtlUint32       m_Second;      /* Second part of the time relative to 00:00:00 January 1, 1970 UTC */
  igtlUint32       m_Nanosecond;  /* Nano-second part of -- */
  bool            m_UTC;
};

} // end of namespace igtl

#endif  // __igtlTimeStamp_h


