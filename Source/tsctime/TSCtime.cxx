// TSCtime.c : high-precision time library
//

// $Id: TSCtime.c,v 1.7 2003/09/25 17:07:23 kw217 Exp $

//
// Copyright (c) 2003, Keith Wansbrough
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of the University of Cambridge nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//

// (for information only: this is the BSD license as found on
//  http://www.opensource.org/licenses/bsd-license.php)

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)

#include "tsctime.h"
//#include "Winreg.h"
#include "time.h"

#pragma intrinsic(__rdtsc)

// recalibrate -- may take up to 100msec or so.  Updates the four static globals above.
#define nreps 5  // number of ticks to examine; we take the average.

ULONGLONG basetsc = 0;                 // base timestamp in processor clocks
ULONGLONG basest  = 0;                 // base time in hecto-nanoseconds (100ns units, 10^-7 sec)
ULONGLONG tscfreq = 500000000uI64;     // timestamp clock frequency in ticks/second
ULONGLONG tscsd   = 500000000uI64;     // measured error (standard deviation) of above
ULONGLONG ugly_hack_offset = 0;        // correction to add to returned time
  // this accounts for the fact that we never get control exactly at the tick; it's always
  // a bit later than that.  The value is a guesstimate.
  // This is stored as a string value (for easy editing) in the registry.

  // ** Turns out that isn't necessary for our tests; but we leave the hook here just in case.

// store time of last recalibration
ULONGLONG lastrecal = 0;  // hectonanosecond time since epoch of last recalibration; 0 == never

// store time of last frequency recalibration
ULONGLONG lastFreqRecal = 0; // hectonanosecond time since epoch of last recalibration; 0 == never

// interval between automatic recalibrations
ULONGLONG recalinterval = 1000000000uI64;  // recal every 100sec.


// be sure to build in Release configuration, and make sure inlining is on.
__forceinline ULONGLONG gettsc(void) {

  ULONGLONG i;
  i = __rdtsc();
  //printf_s("%I64d ticks\n", i);

  return i;
}

// this is to measure the actual clock frequency
void igtl_export calibrate_clock_freq(void)
{
	ULONGLONG basetsc,curtsc;  // in processor clocks
	FILETIME baseft,curft;     // in hecto-nanoseconds (100ns units, 10^-7 sec)
	ULONGLONG basest, curst;

	//int numtrials = 10;
	//DWORD trialmsec = 10000;

  int numtrials = 5;
	DWORD trialmsec = 5000;

	double *rate = new double[numtrials];
	ULONGLONG *dur = new ULONGLONG[numtrials];

  printf("\nCalibrating the clock frequency, this will take a while...\n");

#ifdef DEBUG
  LARGE_INTEGER ld;
	QueryPerformanceFrequency(&ld);
	printf("For comparison, QueryPerformanceFrequency says %I64u\n",ld.QuadPart);
#endif

	for (int i=0; i<numtrials; i++)   // let's do it a few times for good measure
  {
	  // get base times
	  Sleep(5);  // to be sure we're on a tick boundary
    basetsc = gettsc();  // get this first because it changes very fast
    GetSystemTimeAsFileTime(&baseft);  // no rush; we have 1/64 sec to read this before it changes

	  // get later times
	  Sleep(trialmsec);
    curtsc = gettsc();
    GetSystemTimeAsFileTime(&curft);

    // coerce filetimes
	  basest = ULONGLONG(baseft.dwHighDateTime)<<32 | ULONGLONG(baseft.dwLowDateTime);
    curst = ULONGLONG(curft.dwHighDateTime)<<32 | ULONGLONG(curft.dwLowDateTime);

    // do calibration
	  // NB: dwLowDateTime wraps every 429.5 seconds, so don't just use that!
	  ULONGLONG deltsc = curtsc - basetsc;
	  ULONGLONG delst = curst - basest;
	  double tscrate = double(deltsc) / (double(delst) / 1e7);

    #ifdef DEBUG
	    printf("Tick delta %I64u, hectonanosecond delta %I64u.\nSo ticks/sec = %0.4f\n",deltsc,delst,tscrate);
    #endif

	  rate[i] = tscrate;
	  dur[i] = delst;
	}

	ULONGLONG sumdur = 0;
	
  for (int i=0; i<numtrials; i++) 
  {
		sumdur += dur[i];
	}
	double avdur = double(sumdur) / numtrials / 1e7;

	double minrate, maxrate;
	minrate = maxrate = rate[0];
	for (int i=1; i<numtrials; i++) 
  {
		if (rate[i]<minrate) { minrate = rate[i]; }
		if (rate[i]>maxrate) { maxrate = rate[i]; }
	}
	double sum = 0;
	for (int i=0; i<numtrials; i++) { sum += rate[i]; }
	double av = sum / numtrials;
	double sumsqdiff = 0;
	for (int i=0; i<numtrials; i++) { double diff = rate[i] - av; sumsqdiff += diff*diff; }
	double sd = sqrt(sumsqdiff/numtrials);

#ifdef DEBUG
	printf("Mean rate  %14.4f ticks/sec\n      min -%14.4f\n      max +%14.4f\n      s.d. %14.4f\n",av,av-minrate,maxrate-av,sd);

	printf("Scheduling noise (TSC measurement precision):\n  min -%14.4f ticks\n  max +%14.4f\n  s.d. %14.4f\n", avdur * (av-minrate), avdur * (maxrate-av), avdur * sd);
#endif

	tscfreq = ULONGLONG(floor(av+0.5));
	tscsd = ULONGLONG(floor(sd+0.5));
	
  FILETIME calibTime;
  GetSystemTimeAsFileTime(&calibTime);

  // coerce filetime
	lastFreqRecal = ULONGLONG(calibTime.dwHighDateTime)<<32 | ULONGLONG(calibTime.dwLowDateTime);
}


__forceinline ULONGLONG igtl_export hectonanotime_of_tsc(ULONGLONG curtsc) 
{
  // compute seconds and hectonanoseconds separately to avoid overflow problems
	// deltaticks may be negative if we're measuring first and calibrating later
	BOOL neg = curtsc < basetsc;
	ULONGLONG deltaticks = neg ? basetsc - curtsc : curtsc - basetsc;
	ULONGLONG deltasecs = deltaticks / tscfreq;
	ULONGLONG deltafrac = (10000000 * (deltaticks % tscfreq)) / tscfreq;  /* in hectonanoseconds */
	ULONGLONG delta = deltasecs * 10000000 + deltafrac;
	return (neg ? basest - delta : basest + delta);
}

void igtl_export recalibrate(void) 
{
	int i;

	ULONGLONG basetscs[nreps];  // basetsc for each round
	ULONGLONG basests[nreps];   // basest for each round

  char buf[100] = "0";

	// get base times
	for (i=0; i<nreps; i++) 
  {
		FILETIME baseft;
		Sleep(5);  // to be sure we're on a tick boundary
		basetscs[i] = gettsc();  // get this first because it changes very fast
		GetSystemTimeAsFileTime(&baseft);  // no rush; we have 1/64 sec to read this before it changes
		basests[i] = ((ULONGLONG)baseft.dwHighDateTime)<<32 | ((ULONGLONG)baseft.dwLowDateTime);
	}

	basetsc = basetscs[nreps-1];
	
  /* Now: we want to know the correct basest for the latest basetsc.  Each measurement can be
	   extrapolated to give us a basest for the latest basetsc.  But if there was a delay between the
	   tick and our process being scheduled in a particular round, this will show up as the basetsc
	   being high, a.k.a., the basest being low.  Thus, we take the highest computed basest. */
	basest = basests[nreps-1];
	
  for (i=0; i<nreps; i++) 
  {
		ULONGLONG basestx = basests[i] + (10000000 * (basetscs[nreps-1] - basetscs[i])) / tscfreq;
			// don't worry about overflow; at 500MHz (glia) it won't overflow until just over an hour
		if (basestx > basest) 
    {
			basest = basestx;
		}
	}

  basest -= ugly_hack_offset;   // apply the hack offset.

	lastrecal = basest;  // we just recalibrated.
}

// get current time, without recalibration
__forceinline ULONGLONG igtl_export gethectonanotime_norecal(void) 
{
	ULONGLONG curtsc;

	// get the timestamp right up front
	curtsc = gettsc();
	return hectonanotime_of_tsc(curtsc);
}

// get time of call, possibly recalibrating before returning
__forceinline ULONGLONG igtl_export gethectonanotime_first(void) 
{
	ULONGLONG curtsc = gettsc();
	ULONGLONG now = hectonanotime_of_tsc(curtsc);
	if ((lastrecal == 0) || (now - lastrecal) > recalinterval) 
  {
		recalibrate();
		now = hectonanotime_of_tsc(curtsc);
	}
	return now;
}

// get time of return, possibly recalibrating before returning
__forceinline ULONGLONG igtl_export gethectonanotime_last(void) 
{
	(void)gethectonanotime_first();
	return gethectonanotime_norecal();
}

// get tsct parameters
__forceinline void igtl_export get_tsctparam(ULONGLONG &p_tscfreq, ULONGLONG &p_tscsd, ULONGLONG &p_ugly_hack_offset)
{
  p_tscfreq = tscfreq;
  p_tscsd   = tscsd;
  p_ugly_hack_offset = ugly_hack_offset;
}

// set tsct parameters
extern __forceinline void igtl_export set_tsctparam(ULONGLONG &p_tscfreq, ULONGLONG &p_tscsd, ULONGLONG &p_ugly_hack_offset)
{
  tscfreq = p_tscfreq;
  tscsd = p_tscsd;
  ugly_hack_offset = p_ugly_hack_offset;
}

#endif
