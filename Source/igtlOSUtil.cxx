/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlOSUtil.cxx $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlOSUtil.h"
#include "igtl_util.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
#else
  #include <time.h>
  #include "errno.h"
#endif

#include <iostream>

namespace igtl
{

void Sleep(int milliseconds)
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

// Get the current system time in different formats
__forceinline void GetTimeUTC(igtlUint32 &second, igtlUint32 &nanosecond)
{

#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = gethectonanotime_first();
  
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

void GetTimeTAI(igtlUint32 &second, igtlUint32 &nanosecond)
{
  GetTimeUTC(second, nanosecond);
  second += TAI_UTC;
}

void GetTimeUTC_last(igtlUint32 &second, igtlUint32 &nanosecond)
{

#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = gethectonanotime_last();
  
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

void GetTimeTAI_last(igtlUint32 &second, igtlUint32 &nanosecond)
{
  GetTimeUTC_last(second, nanosecond);
  second += TAI_UTC;
}

ULONGLONG GetNanoTimeUTC(void)
{
#if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = gethectonanotime_first();
  
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

ULONGLONG GetNanoTimeTAI(void)
{
  return (ULONGLONG)(GetNanoTimeUTC() + TAI_UTC *1e9);
}

ULONGLONG GetNanoTimeUTC_last(void)
{
  #if defined(WIN32) || defined(_WIN32)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  ULONGLONG curTime = gethectonanotime_last();
  
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

ULONGLONG GetNanoTimeTAI_last(void)
{
  return (ULONGLONG)(GetNanoTimeUTC_last() + TAI_UTC *1e9);
}

ULONGLONG GetOIGTLTimeUTC(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC(second, nanosecond);
  frac = igtl_nanosec_to_frac(nanosecond);
  long long unsigned int cat = ULONGLONG(second)<<32 | ULONGLONG(frac);
  return BYTE_SWAP_INT64(cat);
}

ULONGLONG GetOIGTLTimeTAI(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC(second, nanosecond);
  second += TAI_UTC;
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

ULONGLONG GetOIGTLTimeUTC_last(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC_last(second, nanosecond);
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

ULONGLONG GetOIGTLTimeTAI_last(void)
{
  igtlUint32 second;
  igtlUint32 nanosecond;
  igtlUint32 frac;

  GetTimeUTC_last(second, nanosecond);
  second += TAI_UTC;
  frac = igtl_nanosec_to_frac(nanosecond);
  return BYTE_SWAP_INT64(ULONGLONG(second)<<32 | ULONGLONG(frac));
}

__forceinline void ConvertToSec(ULONGLONG nanotime, igtlUint32 &sec, igtlUint32 &msec, igtlUint32 &usec, igtlUint32 &nsec)
{
  sec  = nanotime /1e9;
  msec = (nanotime - sec*1e9) /1e6;
  usec = (nanotime - sec*1e9 - msec*1e6)/1e3;
  nsec = nanotime - sec*1e9 - msec*1e6 - usec*1e3;
}

}
