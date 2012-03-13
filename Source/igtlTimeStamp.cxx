/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTimeStamp.cxx $
  Language:  C++
  Date:      $Date: 2011-03-24 00:08:23 -0400 (Thu, 24 Mar 2011) $
  Version:   $Revision: 7354 $

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


void TimeStamp::GetTime()
{
  GetTimeUTC(this->m_Second, this->m_Nanosecond);
  m_UTC = true;
}

void TimeStamp::GetTime_TAI()
{
  GetTimeTAI(this->m_Second, this->m_Nanosecond);
  m_UTC = false;
}

void TimeStamp::toUTC()
{
  if (m_UTC)  //currently UTC
    return;
  
  //Need to subtract the offset
  this->m_Second -= TAI_UTC;
  m_UTC = false;
}

void TimeStamp::toTAI()
{
  if (!m_UTC)  //currently TAI
    return;
  
  this->m_Second += TAI_UTC;
  m_UTC = true;
}


void TimeStamp::SetTime(double tm)
{
  double second = floor(tm);
  this->m_Second = static_cast<igtlUint32>(second);
  this->m_Nanosecond = static_cast<igtlUint32>((tm - second)*1e9);
}

void TimeStamp::SetTime(igtlUint32 second, igtlUint32 nanosecond)
{
  if (nanosecond < 1e9)
    {
    this->m_Second = second;
    this->m_Nanosecond = nanosecond;
    }
}


void TimeStamp::SetTime(igtlUint64 tm)
{
  // Export from 64-bit fixed-point expression used in OpenIGTLink
  igtlUint32 sec      = static_cast<igtlUint32>((tm >> 32 ) & 0xFFFFFFFF);
  igtlUint32 fraction = static_cast<igtlUint32>(tm & 0xFFFFFFFF);
  this->m_Second     = sec;
  this->m_Nanosecond = igtl_frac_to_nanosec(static_cast<igtlUint32>(fraction));
}


double TimeStamp::GetTimeStamp()
{
  double tm;
  tm = static_cast<double>(this->m_Second) + static_cast<double>(this->m_Nanosecond) / 1e9;

  return tm;
}

void TimeStamp::GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond)
{
  *second     = this->m_Second;
  *nanosecond = this->m_Nanosecond;
}
  
  
igtlUint64 TimeStamp::GetTimeStampUint64()
{
  // Export as 64-bit fixed-point expression used in OpenIGTLink
  igtlUint32 sec      = this->m_Second;
  igtlUint32 fraction = igtl_nanosec_to_frac(this->m_Nanosecond);

  igtlUint64 ts  =  sec & 0xFFFFFFFF;
  ts = (ts << 32) | (fraction & 0xFFFFFFFF);

  return ts;
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


