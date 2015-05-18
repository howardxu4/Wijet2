#if !defined(_PSM_PROTOCOL_AUTOIP_PROCESS_DEFINED)
#define _PSM_PROTOCOL_AUTOIP_PROCESS

#include "WijetTypes.h"

#define FLASH_PORT_ST (30400)
#define INCOMING_PORT (30500)
#define RFB_PORT_OFST (30900)
#define HTTP_PORT_OFF (30800)		// we don't use this

#define UDP_PORT_RECV (30600)		// reverse from PSM
#define UDP_PORT_SEND (30700)

#define WIJET_PORT (30300)		// TCP WIJET SERVER for moderator

#define SERVERPORT UDP_PORT_SEND

/*
 * Message type definition for IP address setup at the beginning
 * of session establishment fro PSM.
 */
#define DISCOVERY_REQ   0x0101 //(VERSION_MAIN_PROTOCOL || 0x0100)  //(0x01 << 8))
#define DISCOVERY_RESP  0x0201 //(VERSION_MAIN_PROTOCOL || 0x0200)  //(0x02 << 8))
#define IPADDR_CONFIRM  0x0301 //(VERSION_MAIN_PROTOCOL || 0x0300)  //(0x03 << 8))
#define IPADDR_OFFER    0x0401 //(VERSION_MAIN_PROTOCOL || 0x0400)  //(0x04 << 8))
#define IPADDR_ACCEPT   0x0501 //(VERSION_MAIN_PROTOCOL || 0x0500)  //(0x05 << 8))

/*
 * Message type definition for PSM to probe WiJET to see if it is still
 * on wireless link.  For the re-connection, the IP address of WiJET should
 * not being changed.
 */
#define RECONNECT_REQ   0x0102   /* PSM reconnect probe */
#define RECONNECT_RESP  0x0202   /* Respond to PSM reconnect probe */

/*
 * Message type definetion for PSM to notify WiJET that a <release session>
 * was pressed.  When WiJET receives this message, it should turn off the delay
 * and return to banner immediately.
 */
#define RELEASE_NOTIFY  0x0103  /* PSM notify <release session> being pressed */

#define QUERYONLY_REQ	0x0601	/* unicast query to check current status */  
#define QUERYONLY_RESP	0x0602	/* respons of information like DISCOVERY_RESP */

#define TYPE_ACK        0x0001
#define TYPE_MORE       0x0000
#define TYPE_IN_SESSION 0x0002

#define DEVICE_NAME_MAX_LEN      30

//-------------------------------------------------
// Auto IP packet format pre-defined data structure
//-------------------------------------------------

typedef struct s_ipMask {
        CARD8   ipAddr[4];         /* IP address */
        CARD8   nMask[4];          /* subnet mask */
} ipAddrMask;

typedef struct s_macIpAddr {
        CARD8   mac[6];
        CARD8   ipAddr[4];
} macIpAddr;

typedef union s_pktCtrl {
        CARD8       pVer;
        CARD8       msgType;
} pktCtrl;

typedef struct s_ptCtrlBits {
        unsigned type     : 4;      /* Type field */
        unsigned flag     : 4;      /* Flag field */
        unsigned reserved : 8;      /* Reserved field */
} ptCtrlBits;

typedef union {
        CARD16      type;
        CARD16      proCtrl;
} protocolCtrl;

typedef struct s_discoveryRespExt {
        CARD16      dNameLen;
        CARD8       dName[DEVICE_NAME_MAX_LEN+1];
} discoveryRespExt;

//---------------------------------------
// Auto IP packet format data structure
//---------------------------------------
typedef struct s_discoveryReqMsg {
        CARD16      cmd;            /* Packet Control field */
        ipAddrMask  r;              /* PSM offering IP address subnet mask pair */
} discoveryReqMsg;

typedef struct s_discoveryRespMsg {
        CARD16           cmd;       /* Packet Control field */
        protocolCtrl     t;         /* Protocol Control field */
        macIpAddr        r;         /* WiJET respond IP address & subnet mask pair */
        CARD16           extLen;    /* Extended message length */
        discoveryRespExt e;         /* Extended message */
} discoveryRespMsg;

typedef struct s_ipAddrConfirmMsg {
        CARD16  cmd;
        CARD16  padding;
} ipAddrConfirmMsg;

#define OFFER_MAX 50

typedef struct s_ipAddressOfferMsg {
        CARD16    cmd;
        short     numOfUnit;
        macIpAddr aOffer[OFFER_MAX];
} ipAddressOfferMsg;

typedef struct s_reconnectMsg reconnectMsg;
typedef struct s_reconnectMsg releaseMsg;

struct s_reconnectMsg {
        CARD16    cmd;
        CARD16    pad;
};

typedef union {
        CARD16                  cmd;
        pktCtrl                 pCtrl;
        discoveryReqMsg         drq;
        discoveryRespMsg        drp;
        ipAddressOfferMsg       ipA;
        reconnectMsg            rc;
} ipHandshake;

typedef struct s_scanReqMsg {
        unsigned short  cmd;
        unsigned long   ipAddr;
        unsigned long   nMask;
} scanReqMsg;

typedef struct s_scanRespMsg {
        CARD16          cmd;
        CARD32          ipAddr;
        CARD16  nLen;
        CARD8   name[32];
} scanRespMsg;

#endif	// _PSM_PROTOCOL_AUTOIP_PROCESS

