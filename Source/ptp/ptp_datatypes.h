#ifndef DATATYPES_DEP_H_
#define DATATYPES_DEP_H_

#if defined (_WIN32)
  #include <winsock2.h>
#endif

/**
*\file
* \brief Plateform-dependent constants definition
*
* This header defines all includes and constants which are plateform-dependent
*
* ptpdv2 is only implemented for linux, NetBSD and FreeBSD
 */ 

#ifdef	linux
#include<netinet/in.h>
#include<net/if.h>
#include<net/if_arp.h>
#define IFACE_NAME_LENGTH         IF_NAMESIZE
#define NET_ADDRESS_LENGTH        INET_ADDRSTRLEN

#define IFCONF_LENGTH 10

#include<endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define PTPD_LSBF
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PTPD_MSBF
#endif
#endif /* linux */


#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__APPLE__)
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <net/if.h>
# include <net/if_dl.h>
# include <net/if_types.h>
# if defined(__FreeBSD__) || defined(__APPLE__)
#  include <net/ethernet.h>
#  include <sys/uio.h>
# else
#  include <net/if_ether.h>
# endif
# include <ifaddrs.h>
# define IFACE_NAME_LENGTH         IF_NAMESIZE
# define NET_ADDRESS_LENGTH        INET_ADDRSTRLEN

# define IFCONF_LENGTH 10

# define adjtimex ntp_adjtime

# include <machine/endian.h>
# if BYTE_ORDER == LITTLE_ENDIAN
#   define PTPD_LSBF
# elif BYTE_ORDER == BIG_ENDIAN
#   define PTPD_MSBF
# endif
#endif 


#define CLOCK_IDENTITY_LENGTH 8
#define ADJ_FREQ_MAX  512000

/* UDP/IPv4 dependent */
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001UL
#endif

#define SUBDOMAIN_ADDRESS_LENGTH  4
#define PORT_ADDRESS_LENGTH       2
#define PTP_UUID_LENGTH           6
#define CLOCK_IDENTITY_LENGTH	  8
#define FLAG_FIELD_LENGTH         2

#define PACKET_SIZE  300 //ptpdv1 value kept because of use of TLV...

#define PTP_EVENT_PORT    319
#define PTP_GENERAL_PORT  320

#define DEFAULT_PTP_DOMAIN_ADDRESS     "224.0.1.129"
#define PEER_PTP_DOMAIN_ADDRESS        "224.0.0.107"

/* used for -I option */
#define ALTERNATE_PTP_DOMAIN1_ADDRESS  "224.0.1.130"
#define ALTERNATE_PTP_DOMAIN2_ADDRESS  "224.0.1.131"
#define ALTERNATE_PTP_DOMAIN3_ADDRESS  "224.0.1.132"

#define MM_STARTING_BOUNDARY_HOPS  0x7fff

/* others */

/* bigger screen size constants */
#define SCREEN_BUFSZ  228
#define SCREEN_MAXSZ  180

/* default size for string buffers */
#define BUF_SIZE  1000

#define NANOSECONDS_MAX 999999999

// limit operator messages to once every X seconds
#define OPERATOR_MESSAGES_INTERVAL 300.0

#define MAXTIMESTR 32 


/**
*\file
* \brief Implementation specific datatype

 */
/* FIXME: shouldn't uint32_t and friends be used here? */
typedef bool Boolean;
typedef char Octet;
typedef signed char Integer8;
typedef signed short Integer16;
typedef signed int Integer32;
typedef unsigned char UInteger8;
typedef unsigned short UInteger16;
typedef unsigned int UInteger32;
typedef unsigned short Enumeration16;
typedef unsigned char Enumeration8;
typedef unsigned char Enumeration4;
typedef unsigned char UInteger4;
typedef unsigned char Nibble;

/**
* \brief Implementation specific of UInteger48 type
 */
typedef struct {
	unsigned int lsb;     /* FIXME: shouldn't uint32_t and uint16_t be used here? */
	unsigned short msb;
} UInteger48;

/**
* \brief Implementation specific of Integer64 type
 */
typedef struct {
	unsigned int lsb;     /* FIXME: shouldn't uint32_t and int32_t be used here? */
	int msb;
} Integer64;

/**
* \brief Struct used to average the offset from master
*
* The FIR filtering of the offset from master input is a simple, two-sample average
 */
typedef struct {
  Integer32  nsec_prev, y;
} offset_from_master_filter;

/**
* \brief Struct used to average the one way delay
*
* It is a variable cutoff/delay low-pass, infinite impulse response (IIR) filter.
*
*  The one-way delay filter has the difference equation: s*y[n] - (s-1)*y[n-1] = x[n]/2 + x[n-1]/2, where increasing the stiffness (s) lowers the cutoff and increases the delay.
 */
typedef struct {
  Integer32  nsec_prev, y;
  Integer32  s_exp;
} one_way_delay_filter;

/**
* \brief Struct used to store network datas
 */
typedef struct {
  Integer32 eventSock, generalSock, multicastAddr, peerMulticastAddr,unicastAddr;

  /* used by IGMP refresh */
  struct in_addr interfaceAddr;

#ifdef PTP_EXPERIMENTAL
  /* used for Hybrid mode */
  Integer32 lastRecvAddr;
#endif

} NetPath;

/**
* \brief The ClockIdentity type identifies a clock
 */
typedef Octet ClockIdentity[CLOCK_IDENTITY_LENGTH];

/** 
 * Internal time structure used by ptpv2d.  
 * Uses time in signed seconds and signed nanoseconds
 */
typedef struct 
{
  Integer32 seconds;
  Integer32 nanoseconds;  
} TimeInternal;

typedef struct 
{
  Integer32  interval;
  Integer32  left;
  Boolean    expire;
} IntervalTimer;

/**
* \brief The Timestamp type represents a positive time with respect to the epoch
 */
typedef struct  {
	UInteger48 secondsField;
	UInteger32 nanosecondsField;
} Timestamp;


/**
* \brief The PortIdentity identifies a PTP port.
 */
typedef struct {
	ClockIdentity clockIdentity;
	UInteger16 portNumber;
} PortIdentity;

/**
* \brief The PortAdress type represents the protocol address of a PTP port
 */
typedef struct {
	Enumeration16 networkProtocol;
	UInteger16 adressLength;
	Octet* adressField;
} PortAdress;

/**
* \brief The ClockQuality represents the quality of a clock
 */
typedef struct {
	UInteger8 clockClass;
	Enumeration8 clockAccuracy;
	UInteger16 offsetScaledLogVariance;
} ClockQuality;

/**
* \brief The TLV type represents TLV extension fields
 */
typedef struct {
	Enumeration16 tlvType;
	UInteger16 lengthField;
	Octet* valueField;
} TLV;

/**
* \brief The PTPText data type is used to represent textual material in PTP messages
 */
typedef struct {
	UInteger8 lengthField;
	Octet* textField;
} PTPText;




#endif /*DATATYPES_DEP_H_*/
