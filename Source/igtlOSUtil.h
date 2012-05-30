/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlOSUtil.h $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igltOSUtil_h
#define __igltOSUtil_h

#include "igtlTypes.h"
#include "igtlWin32Header.h"

#if defined(WIN32) || defined(_WIN32)
  #include <ctime>
  #include "tsctime/TSCtime.h"
#else
  #if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 4 && __GNUC_PATCHLEVEL__ >= 1 )
    #define __forceinline
  #else
    #define __forceinline __attribute__ ((always_inline))
  #endif

  #include <sys/time.h>
  #include <stdint.h>

  #ifndef ULONGLONG
    #define ULONGLONG uint64_t
  #endif
#endif



namespace igtl
{

  // Windows epoch for hectonanotime is January 1, 1601 UTC,
// so subtract 11644473600 seconds to get Unix time.
// (369 years, 89 of which are leap years  = 134774 days)

#define UNIX_EPOCH 116444736000000000ui64

// Current TAI to UTC offset
// Will change to 35 on 2012 JUL  1
#define TAI_UTC 34


/** Stop the program for the duration specified in millisecond
*  The maximum dulation is 
* */
__forceinline void IGTLCommon_EXPORT Sleep(int millisecond);

// Get the current system time in different formats
__forceinline void IGTLCommon_EXPORT GetTimeUTC(igtlUint32 &second, igtlUint32 &nanosecond);
__forceinline void IGTLCommon_EXPORT GetTimeTAI(igtlUint32 &second, igtlUint32 &nanosecond);
__forceinline void IGTLCommon_EXPORT GetTimeUTC_last(igtlUint32 &second, igtlUint32 &nanosecond);
__forceinline void IGTLCommon_EXPORT GetTimeTAI_last(igtlUint32 &second, igtlUint32 &nanosecond);

__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeUTC(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeTAI(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeUTC_last(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeTAI_last(void);

__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeUTC(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeTAI(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeUTC_last(void);
__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeTAI_last(void);

__forceinline void IGTLCommon_EXPORT ConvertToSec(ULONGLONG nanotime, igtlUint32 &sec, igtlUint32 &msec, igtlUint32 &usec, igtlUint32 &nsec);

}

#endif // __igltOSUtil_h


