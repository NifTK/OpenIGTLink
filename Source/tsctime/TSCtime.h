// TSCtime.h : header file for high-precision time library
//

// $Id: TSCtime.h,v 1.4 2003/09/25 17:07:23 kw217 Exp $

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

#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include "igtl_win32header.h"

//#define DEBUG

#ifdef DEBUG
  #include <conio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern ULONGLONG basetsc;
extern ULONGLONG basest;
extern ULONGLONG tscfreq;
extern ULONGLONG tscsd;
extern ULONGLONG ugly_hack_offset;
extern ULONGLONG lastrecal;
extern ULONGLONG recalinterval;
extern ULONGLONG lastFreqRecal;

// be sure to build in Release configuration, and make sure inlining is on.
extern __forceinline ULONGLONG gettsc(void); 
extern void igtl_export calibrate_clock_freq(void);

extern __forceinline ULONGLONG igtl_export hectonanotime_of_tsc(ULONGLONG curtsc) ;

// recalibrate -- may take up to 100msec or so.  Updates the four static globals above.
extern void igtl_export recalibrate(void);

// get current time, without recalibration
extern __forceinline ULONGLONG igtl_export gethectonanotime_norecal(void); 

// get time of call, possibly recalibrating before returning
extern __forceinline ULONGLONG igtl_export gethectonanotime_first(void); 

// get time of return, possibly recalibrating before returning
extern __forceinline ULONGLONG igtl_export gethectonanotime_last(void); 

// get tsct parameters
extern __forceinline void igtl_export get_tsctparam(ULONGLONG &p_tscfreq, ULONGLONG &p_tscsd, ULONGLONG &p_ugly_hack_offset); 

// set tsct parameters
extern __forceinline void igtl_export set_tsctparam(ULONGLONG &p_tscfreq, ULONGLONG &p_tscsd, ULONGLONG &p_ugly_hack_offset);

#ifdef __cplusplus

}
#endif

#endif
