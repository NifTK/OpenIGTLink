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

}
