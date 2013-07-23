/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_WIN32HEADER_H2
#define __IGTL_WIN32HEADER_H2

#include "igtl_typeconfig.h"

#if (defined(_WIN32) || defined(WIN32)) && !defined(IGTLSTATIC) 
# ifdef IGTLCommon_EXPORTS
#  define igtl_export __declspec(dllexport)
# else
#  define igtl_export __declspec(dllimport)
# endif  /* igtl_common_exports */
#else
/* unix needs nothing */
#define igtl_export
#endif

#if defined(_WIN32)
  #if defined(IGTL_INCLUDE_WINDOWS_H)
    #include <winsock2.h>
    #include <windows.h>
  #endif
#endif

#if defined(_MSC_VER)
 /* Enable MSVC compiler warning messages that are useful but off by default.*/
#  pragma warning ( disable : 4996 ) /* 'strncpy': This function or variable may be unsafe. */
#endif


#endif /*__IGTL_WIN32HEADER_H*/
