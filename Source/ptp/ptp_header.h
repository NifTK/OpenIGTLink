/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlutil/igtl_header.h $
  Language:  C
  Date:      $Date: 2011-03-24 00:32:25 -0400 (Thu, 24 Mar 2011) $
  Version:   $Revision: 7357 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __PTP_HEADER_H
#define __PTP_HEADER_H

#define PTP_HEADER_VERSION   1
#define PTP_HEADER_SIZE      58

#define PTP_HEADER_TYPE_SIZE  12
#define PTP_HEADER_NAME_SIZE  20

// Device name prefix macro
#define PTP_TYPE_PREFIX_NONE     0
#define PTP_TYPE_PREFIX_GET      1
#define PTP_TYPE_PREFIX_STT      2
#define PTP_TYPE_PREFIX_STP      3
#define PTP_TYPE_PREFIX_RTS      4
#define PTP_NUM_TYPE_PREFIX      5

#include "ptp_datatypes.h"
#include "igtl_win32header.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/*
 * Message header of OpenIGTLink message.
 *
 * igtl_header is an overall data header for OpenIGTLink protocol.
 * It is transfered at beginning of every OpenIGTLink message to give
 * type and size of following data body to a receiver.
 * These parameters allow the receiver to parse or skip the data body.
 */

typedef struct 
{
	Nibble        transportSpecific;
	Enumeration4  messageType;
	UInteger4     versionPTP;
	UInteger16    messageLength;
	UInteger8     domainNumber;
	Octet         flagField[2];
	Integer64     correctionfield;
	PortIdentity  sourcePortIdentity;
	UInteger16    sequenceId;
	UInteger8     controlField;
	Integer8      logMessageInterval;
} ptp_header;


/**
* \brief Announce message fields (Table 25 of the spec)
 */
/*Announce Message */
typedef struct 
{
	Timestamp originTimestamp;
	Integer16 currentUtcOffset;
	UInteger8 grandmasterPriority1;
	ClockQuality grandmasterClockQuality;
	UInteger8 grandmasterPriority2;
	ClockIdentity grandmasterIdentity;
	UInteger16 stepsRemoved;
	Enumeration8 timeSource;
} ptp_announce;

/**
* \brief DelayReq message fields (Table 26 of the spec)
 */
/*DelayReq Message */
typedef struct {
	Timestamp originTimestamp;
} ptp_delayReq;

#pragma pack()

/*
 * Byte order converter for the header structre
 * 
 * ptp_header_convert_byte_order convers endianness of each
 * member variable in ptp_header structre from host byte order
 * to network byte order, or vice versa.
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_HEADER_H */

