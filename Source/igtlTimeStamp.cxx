/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


/*=========================================================================

  Part of the code is copied from itk::RealTimeClock class in
  Insight Segmentation & Registration Toolkit:


  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRealTimeClock.cxx,v $
  Language:  C++
  Date:      $Date: 2011-03-24 00:08:23 -0400 (Thu, 24 Mar 2011) $
  Version:   $Revision: 7354 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlTimeStamp.h"

#include <math.h>
#include <string.h>

#include "igtl_util.h"
#include "igtlOSUtil.h"

namespace igtl
{

TimeStamp::TimeStamp(): Object()
{
  GetTimeUTC(this->m_Second, this->m_Nanosecond);
  m_UTC = true;
}

TimeStamp::~TimeStamp()
{
}


void TimeStamp::Update()
{
  GetTimeUTC(this->m_Second, this->m_Nanosecond);
  m_UTC = true;
}

void TimeStamp::SetTimeInSeconds(double totalSeconds)
{
  double second = floor(totalSeconds);
  this->m_Second = static_cast<igtlUint32>(second);
  this->m_Nanosecond = static_cast<igtlUint32>((totalSeconds - second)*1e9);
}

double TimeStamp::GetTimeInSeconds()
{
  double tm;
  tm = static_cast<double>(this->m_Second) + static_cast<double>(this->m_Nanosecond) / 1e9;

  return tm;
}

void TimeStamp::SetTime(igtlUint32 second, igtlUint32 nanosecond)
{
  if (nanosecond < 1e9)
    {
    this->m_Second = second;
    this->m_Nanosecond = nanosecond;
    }
}

void TimeStamp::GetTime(igtlUint32* second, igtlUint32* nanosecond)
{
  *second     = this->m_Second;
  *nanosecond = this->m_Nanosecond;
}
  

void TimeStamp::SetTimeInNanoSeconds(igtlUint64 totalNanos)
{
  // Export from 64-bit fixed-point expression used in OpenIGTLink
  igtlUint32 seconds, nanoseconds;

  seconds = (igtlUint64)totalNanos / (igtlUint64)1000000000;
  nanoseconds = (igtlUint64)totalNanos % (igtlUint64)1000000000;

  this->m_Second = seconds;
  this->m_Nanosecond = nanoseconds;
}

  
igtlUint64 TimeStamp::GetTimeInNanoSeconds()
{
  // Export as 64-bit fixed-point expression used in OpenIGTLink

  igtlUint64 result = (igtlUint64)m_Second * 1000000000 + (igtlUint64)m_Nanosecond;
  return result;
}


void TimeStamp::PrintSelf( std::ostream& os) const
{
  Superclass::PrintSelf(os);

  std::string indent = "    ";

  os << indent << "Frequency of the clock: "
    << this->m_Frequency << std::endl;
  os << indent << "Second : "
    << this->m_Second << std::endl;
  os << indent << "Nanosecond : "
    << this->m_Nanosecond << std::endl;
}

}


