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
#include "igtlutil/igtl_util.h"


#if defined(WIN32) || defined(_WIN32)
  #include <ctime>
#else
  #define __forceinline inline __attribute__((always_inline))

  #include <sys/time.h>
  #include <stdint.h>

  #include <time.h>
  #include "errno.h"

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
#define TAI_UTC 35


/** Stop the program for the duration specified in millisecond
*  The maximum dulation is 
* */
__forceinline void IGTLCommon_EXPORT Sleep(int milliseconds)
{
#if defined(_WIN32) && !defined(__CYGWIN__)

  // Call Windows Native Sleep() function
  ::Sleep(milliseconds);

#else
  //    ORIGINAL AND BUGGY
  //    struct timespec req;
  //    req.tv_sec  = (int) milliseconds / 1000;
  //    req.tv_nsec = (milliseconds % 1000) * 1000000;

  //    nanosleep(&req, NULL);

  //    METHOD 2
  struct timespec req = { 0 };
  req.tv_sec = (int)(milliseconds/1000);
  req.tv_nsec = (milliseconds % 1000) * 1000000;
  while ((nanosleep(&req, &req) == -1) && (errno == EINTR))
    continue;

  //    METHOD 3
//  struct timespec timeout0;
//  struct timespec timeout1;
//  struct timespec* tmp;
//  struct timespec* t0 = &timeout0;
//  struct timespec* t1 = &timeout1;

//  t0->tv_sec = (int)(milliseconds/1000);
//  t0->tv_nsec = (milliseconds % 1000) * 1000000;

//  while ((nanosleep(t0, t1) == (-1)) && (errno == EINTR))
//  {
//    std::cerr <<"remainder sleeptime: " <<t1->tv_sec <<" "<<t1->tv_nsec <<std::endl;
//    tmp = t0;
//    t0 = t1;
//    t1 = tmp;
//  }
#endif
}

/**
 * \brief Gets the time since Windows epoch (January 1, 1601) in hectonanoseconds.
 *
 * This method was created to replace the gethectonanotime_first that was, prior
 * to this commit being called in GetTimeUTC. The problem observed was the
 * the recalibration routines within the TSC library were causing unacceptable clock
 * jumps of the order of 77-80 seconds on Win XP and Win 7.
 */
#if defined(WIN32) || defined(_WIN32)

__forceinline ULONGLONG IGTLCommon_EXPORT GetHectoNanotime()
{
  FILETIME fileTime;
  GetSystemTimeAsFileTime(&fileTime);

  ULONGLONG hectoNanoSeconds = fileTime.dwHighDateTime*10000000 + fileTime.dwLowDateTime;
  return hectoNanoSeconds;
}

#endif

// Get the current system time in different formats
__forceinline void IGTLCommon_EXPORT GetTimeUTC(igtlUint32 &second, igtlUint32 &nanosecond)
{

#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = GetHectoNanotime();

  // Need to convert this to Unix time (UTC, epoch January 1, 1970), so subtract 11644473600 seconds
  // (369 years, 89 of which are leap years = 134774 days)

  curTime -= UNIX_EPOCH; //Offset in hectonanosec

  //curTime *=100;  //convert to nanosecs

  second     = curTime / 1e7;
  nanosecond = (curTime - second*1e7)*100;

#else

  struct timeval tval;

  //Gets the system time
  gettimeofday( &tval, 0 );

  second     = tval.tv_sec;
  nanosecond = tval.tv_usec * 1000; /* convert from micro to nano */

#endif  // defined(WIN32) || defined(_WIN32)
}


__forceinline void IGTLCommon_EXPORT GetTimeTAI(igtlUint32 &second, igtlUint32 &nanosecond)
{
  GetTimeUTC(second, nanosecond);
  second += TAI_UTC;
}

__forceinline void IGTLCommon_EXPORT GetTimeUTC_last(igtlUint32 &second, igtlUint32 &nanosecond)
{
#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = GetHectoNanotime();

  // Need to convert this to Unix time (UTC, epoch January 1, 1970), so subtract 11644473600 seconds
  // (369 years, 89 of which are leap years = 134774 days)

  curTime -= UNIX_EPOCH; //Offset in hectonanosec

  //curTime *=100;  //convert to nanosecs

  second     = curTime / 1e7;
  nanosecond = (curTime - second*1e7)*100;

#else

  struct timeval tval;

  //Gets the system time
  ::gettimeofday( &tval, 0 );

  second     = tval.tv_sec;
  nanosecond = tval.tv_usec * 1000; /* convert from micro to nano */

#endif  // defined(WIN32) || defined(_WIN32)
}

__forceinline void IGTLCommon_EXPORT GetTimeTAI_last(igtlUint32 &second, igtlUint32 &nanosecond)
{
  GetTimeUTC_last(second, nanosecond);
  second += TAI_UTC;
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeUTC(void)
{
#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = GetHectoNanotime();

  // Need to convert this to Unix time (UTC, epoch January 1, 1970), so subtract 11644473600 seconds
  // (369 years, 89 of which are leap years = 134774 days)

  curTime -= UNIX_EPOCH; //Offset in hectonanosec

  //curTime *=100;  //convert to nanosecs

  return (ULONGLONG)(curTime *100);
#else

  struct timeval tval;

  //Gets the system time
  ::gettimeofday( &tval, 0 );

  //second     = tval.tv_sec;
  //nanosecond = tval.tv_usec * 1000; /* convert from micro to nano */

  return (ULONGLONG)(tval.tv_usec * 1000 + tval.tv_sec *1e9);

#endif  // defined(WIN32) || defined(_WIN32)
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeTAI(void)
{
  return (ULONGLONG)(GetNanoTimeUTC() + TAI_UTC *1e9);
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeUTC_last(void)
{
  #if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = GetHectoNanotime();

  // Need to convert this to Unix time (UTC, epoch January 1, 1970), so subtract 11644473600 seconds
  // (369 years, 89 of which are leap years = 134774 days)

  curTime -= UNIX_EPOCH; //Offset in hectonanosec

  //curTime *=100;  //convert to nanosecs

  return (ULONGLONG)(curTime *100);
#else

  struct timeval tval;

  //Gets the system time
  ::gettimeofday( &tval, 0 );

  //second     = tval.tv_sec;
  //nanosecond = tval.tv_usec * 1000; /* convert from micro to nano */

  return (ULONGLONG)(tval.tv_usec * 1000 + tval.tv_sec *1e9);

#endif  // defined(WIN32) || defined(_WIN32)
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetNanoTimeTAI_last(void)
{
  return (ULONGLONG)(GetNanoTimeUTC_last() + TAI_UTC *1e9);
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeUTC(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC(second, nanosecond);
  frac = igtl_nanosec_to_frac(nanosecond);
  long long unsigned int cat = ULONGLONG(second)<<32 | ULONGLONG(frac);
  return BYTE_SWAP_INT64(cat);
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeTAI(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC(second, nanosecond);
  second += TAI_UTC;
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeUTC_last(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC_last(second, nanosecond);
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

__forceinline ULONGLONG IGTLCommon_EXPORT GetOIGTLTimeTAI_last(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC_last(second, nanosecond);
  second += TAI_UTC;
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

__forceinline void IGTLCommon_EXPORT ConvertToSec(ULONGLONG nanotime, igtlUint32 &sec, igtlUint32 &msec, igtlUint32 &usec, igtlUint32 &nsec)
{
  sec  = nanotime /1e9;
  msec = (nanotime - sec*1e9) /1e6;
  usec = (nanotime - sec*1e9 - msec*1e6)/1e3;
  nsec = nanotime - sec*1e9 - msec*1e6 - usec*1e3;
}

}

#endif // __igltOSUtil_h


