/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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

  /// Gets the frequency of a clock.
  igtlGetConstMacro(Frequency,  igtlUint32);

  /// Gets the second part of the time stamp.
  igtlGetConstMacro(Second,     igtlUint32);

  /// Gets the fraction of second part of the time stamp.
  igtlGetConstMacro(Nanosecond, igtlUint32);

  /// Gets the current time from the system's clock and save it as a time stamp in UTC.
  void Update();

  /// Checks if the time stamp is in UTC format.
  inline bool isUTC() {return m_UTC; }
  
  // Set time in human readable form
  void SetTime(igtlUint32 second, igtlUint32 nanosecond);

    // Get time in human readable form
  void GetTime(igtlUint32* second, igtlUint32* nanosecond);
  
  // Set time as the number of seconds since epoch in double
  void SetTimeInSeconds(double totalSeconds);

  // Get time  as the number of seconds since epoch in double
  double GetTimeInSeconds();

  // Set time in the OpenIGTLink compatible form
  void SetTimeUint64(igtlUint64 totalNanos);  /* 64-bit fixed-point expression used in OpenIGTLink */

  // Get time in the OpenIGTLink compatible form
  igtlUint64 GetTimeUint64();        /* 64-bit fixed-point expression used in OpenIGTLink */

protected:

  /** constructor */
  TimeStamp();

  /** destructor */
  virtual ~TimeStamp();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os) const;

private:

  /// Clock frequency (Hz)
  igtlUint32       m_Frequency;   /* Clock frequency (Hz)*/

  /// Second part of the time relative to 00:00:00 January 1, 1970 UTC
  igtlUint32       m_Second;      /* Second part of the time relative to 00:00:00 January 1, 1970 UTC */

  /// Nano-second part of the time stamp
  igtlUint32       m_Nanosecond;  /* Nano-second part of -- */
  bool             m_UTC;
};

} // end of namespace igtl

#endif  // __igtlTimeStamp_h


