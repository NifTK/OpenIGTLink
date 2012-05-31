/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlMessageBase.h $
  Language:  C++
  Date:      $Date: 2009-01-13 14:11:16 -0500 (Tue, 13 Jan 2009) $
  Version:   $Revision: 3544 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ptpMessageBase_h
#define __ptpMessageBase_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlTimeStamp.h"

#include "ptpMessageHeader.h"
#include "ptp_datatypes.h"
#include "ptp_constants.h"

#include <string>

//-------------------------------------------------------------------------
// PTP Timer
//-------------------------------------------------------------------------

namespace ptp
{

class IGTLCommon_EXPORT Timer: public igtl::Object
{
public:

  typedef ptp::Timer                      Self;
  typedef igtl::Object                    Superclass;
  typedef igtl::SmartPointer<Self>        Pointer;
  typedef igtl::SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(ptp::Timer, igtl::Object)
  igtlNewMacro(ptp::Timer);

public:

#if defined(__WINDOWS__) || defined(_WIN32)
  static VOID CALLBACK catch_alarm(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
#else
  static void catch_alarm(int sig);
#endif

  void initTimer(Integer32  seconds, UInteger32 microseconds);
  void timerUpdate(IntervalTimer *itimer, int port_id);
  void timerStop(UInteger16 index, IntervalTimer *itimer);
  void timerStart(UInteger16 index, UInteger16 interval, IntervalTimer *itimer);
  Boolean timerExpired(UInteger16 index, IntervalTimer *itimer, int port_id);

protected:
  Timer() : igtl::Object() {}
 ~Timer() {}

protected:

 /** Elaspsed time (allocated one integer per PTP port) */
 static int elapsed[10];

/**
 * TIMER_INTERVAL is based on number of "TICKS" as set
 * by initTimer function where currently 1 Tick
 * maps to the seconds and microseconds value passed
 * when calling initTimer
 */
#define TIMER_INTERVAL 1

/* Timer global variables */

#if defined(__WINDOWS__) || defined(_WIN32)
  HANDLE TimerQueue;
#endif

#ifdef LIMIT_RUNTIME
/* Variables to allow for limited run time (i.e. terminate the
 * system after a certain amount of time since the daemon
 * was started
 */
int max_ticks=14400; /*14400 4 hours in seconds (60 *60 * 4) */ 
int license_tick_counter = 0;
#endif
};

} // namespace ptp

#endif // _ptpMessageBase_h


