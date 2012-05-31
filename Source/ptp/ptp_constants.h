#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/**
*\file
* \brief Plateform-dependent constants definition
*
* This header defines all includes and constants which are plateform-dependent
*
* ptpdv2 is only implemented for linux, NetBSD and FreeBSD
 */

/* platform dependent */

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

#define flip16(x) htons(x)
#define flip32(x) htonl(x)

#define CLOCK_IDENTITY_LENGTH 8
#define ADJ_FREQ_MAX  512000
#define MAX_PTP_PORTS 1 

/* UDP/IPv4 dependent */
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001UL
#endif

#define SUBDOMAIN_ADDRESS_LENGTH  4
#define PORT_ADDRESS_LENGTH       2
#define PTP_UUID_LENGTH           6
#define CLOCK_IDENTITY_LENGTH	    8
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
* \brief Default values and constants used in ptpdv2
*
* This header file includes all default values used during initialization
* and enumeration defined in the spec
 */

 #define MANUFACTURER_ID \
  "MaceG VanKempen;2.0.0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"


/* implementation specific constants */
#define DEFAULT_INBOUND_LATENCY      	0       /* in nsec */
#define DEFAULT_OUTBOUND_LATENCY     	0       /* in nsec */
#define DEFAULT_NO_RESET_CLOCK       	FALSE
#define DEFAULT_DOMAIN_NUMBER        	0
#define DEFAULT_DELAY_MECHANISM      	E2E     // TODO
#define DEFAULT_AP                   	10
#define DEFAULT_AI                   	1000
#define DEFAULT_DELAY_S              	6
#define DEFAULT_ANNOUNCE_INTERVAL    	1      /* 0 in 802.1AS */

/* Master mode operates in ARB (UTC) timescale, without TAI+leap seconds */
#define DEFAULT_UTC_OFFSET           	0
#define DEFAULT_UTC_VALID            	FALSE
#define DEFAULT_PDELAYREQ_INTERVAL   	1      /* -4 in 802.1AS */

#define DEFAULT_DELAYREQ_INTERVAL    	0      /* new value from page 237 of the standard */

#define DEFAULT_SYNC_INTERVAL        	0      /* -7 in 802.1AS */  /* from page 237 of the standard */
/* number of announces we need to lose until a time out occurs. Thus it is 12 seconds */
#define DEFAULT_ANNOUNCE_RECEIPT_TIMEOUT 6     /* 3 by default */

#define DEFAULT_QUALIFICATION_TIMEOUT	2
#define DEFAULT_FOREIGN_MASTER_TIME_WINDOW 4
#define DEFAULT_FOREIGN_MASTER_THRESHOLD 2


/*
section 7.6.2.4, page 55:
248     Default. This clockClass shall be used if none of the other clockClass definitions apply.
13      Shall designate a clock that is synchronized to an application-specific source of time. The timescale distributed
        shall be ARB. A clockClass 13 clock shall not be a slave to another clock in the domain. 
*/
#define DEFAULT_CLOCK_CLASS		248
#define DEFAULT_CLOCK_CLASS__APPLICATION_SPECIFIC_TIME_SOURCE	13

/*
section 7.6.2.5, page 56:
0x20      Time accurate to 25ns
...
0x31      Time accurate to > 10s
0xFE      Unkown accuracy
*/
#define DEFAULT_CLOCK_ACCURACY		0xFE

#define DEFAULT_PRIORITY1		128        
#define DEFAULT_PRIORITY2		128        /* page 238, default priority is the midpoint, to allow easy control of the BMC algorithm */


/* page 238:  τ, see 7.6.3.2: The default initialization value shall be 1.0 s.  */
#define DEFAULT_CLOCK_VARIANCE 	        28768 /* To be determined in 802.1AS. */
                                             


#define DEFAULT_MAX_FOREIGN_RECORDS  	5
#define DEFAULT_PARENTS_STATS			FALSE

/* features, only change to refelect changes in implementation */
#define NUMBER_PORTS      	1
#define VERSION_PTP       	2
#define TWO_STEP_FLAG    	  TRUE
#define BOUNDARY_CLOCK    	FALSE
#define SLAVE_ONLY		      FALSE
#define NO_ADJUST		        FALSE

/** \name Packet length
 Minimal length values for each message.
 If TLV used length could be higher.*/
 /**\{*/
#define HEADER_LENGTH					        34
#define ANNOUNCE_LENGTH					      64
#define SYNC_LENGTH					          44
#define FOLLOW_UP_LENGTH				      44
#define PDELAY_REQ_LENGTH				      54
#define DELAY_REQ_LENGTH				      44
#define DELAY_RESP_LENGTH				      54
#define PDELAY_RESP_LENGTH 			      54
#define PDELAY_RESP_FOLLOW_UP_LENGTH  54
#define MANAGEMENT_LENGTH				      48
/** \}*/

/*Enumeration defined in tables of the spec*/

/**
 * \brief Domain Number (Table 2 in the spec)*/

enum {
	DFLT_DOMAIN_NUMBER = 0, ALT1_DOMAIN_NUMBER, ALT2_DOMAIN_NUMBER, ALT3_DOMAIN_NUMBER
};

/**
 * \brief Network Protocol  (Table 3 in the spec)*/
enum {
	UDP_IPV4=1,UDP_IPV6,IEE_802_3,DeviceNet,ControlNet,PROFINET
};

/**
 * \brief Time Source (Table 7 in the spec)*/
enum {
	ATOMIC_CLOCK=0x10,GPS=0x20,TERRESTRIAL_RADIO=0x30,PTP=0x40,NTP=0x50,HAND_SET=0x60,OTHER=0x90,INTERNAL_OSCILLATOR=0xA0
};


/**
 * \brief PTP State (Table 8 in the spec)*/
enum {
	INITIALIZING=1, FAULTY,DISABLED,LISTENING,PRE_MASTER,MASTER,PASSIVE,UNCALIBRATED,SLAVE
};

/**
 * \brief Delay mechanism (Table 9 in the spec)*/
enum {
	E2E=1,P2P=2,DELAY_DISABLED=0xFE
};


/**
 * \brief PTP timers
 */
enum {
  PDELAYREQ_INTERVAL_TIMER=0,/**<\brief Timer handling the PdelayReq Interval*/
  DELAYREQ_INTERVAL_TIMER,/**<\brief Timer handling the delayReq Interva*/
  SYNC_INTERVAL_TIMER,/**<\brief Timer handling Interval between master sends two Syncs messages */
  ANNOUNCE_RECEIPT_TIMER,/**<\brief Timer handling announce receipt timeout*/
  ANNOUNCE_INTERVAL_TIMER, /**<\brief Timer handling interval before master sends two announce messages*/

  /* non-spec timers */
  OPERATOR_MESSAGES_TIMER,  /* used to limit the operator messages */
  TIMER_ARRAY_SIZE
};

/**
 * \brief PTP states
 */
enum {
  PTP_INITIALIZING=0,  PTP_FAULTY,  PTP_DISABLED,
  PTP_LISTENING,  PTP_PRE_MASTER,  PTP_MASTER,
  PTP_PASSIVE,  PTP_UNCALIBRATED,  PTP_SLAVE
};

/**
 * \brief PTP Messages
 */
enum {
	SYNC=0x0,
	DELAY_REQ,
	PDELAY_REQ,
	PDELAY_RESP,
	FOLLOW_UP=0x8,
	DELAY_RESP,
	PDELAY_RESP_FOLLOW_UP,
	ANNOUNCE,
	SIGNALING,
	MANAGEMENT,
  UNDEFINED
};

enum {
	PTP_ETHER, PTP_DEFAULT
};


/**
 * \brief PTP flags
 */
enum
{
	PTP_ALTERNATE_MASTER = 0x01,
	PTP_TWO_STEP = 0x02,
	PTP_UNICAST = 0x04,
	PTP_PROFILE_SPECIFIC_1 = 0x20,
	PTP_PROFILE_SPECIFIC_2 = 0x40,
	PTP_SECURITY = 0x80,
};

enum
{
	PTP_LI_61 = 0x01,
	PTP_LI_59 = 0x02,
	PTP_UTC_REASONABLE = 0x04,
	PTP_TIMESCALE = 0x08,
	TIME_TRACEABLE = 0x10,
	FREQUENCY_TRACEABLE = 0x20,
};

#endif /*CONSTANTS_H_*/
