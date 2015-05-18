/*****************************************************************************
 * File: wlan_def.h
 * --------------------------------------------------------------------------
 *
 *
 * Description:
 * ---------------
 * This file defines common data types used by the wlan_common and wlan_advanced
 * classes.
 *
 * History:
 * --------------------------------------------------------------------------
 * JP         11/01/2005          Original 
 *
 ****************************************************************************/


#ifndef _WLAN_DEF_
#define _WLAN_DEF_

#include <iwlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "wlan_exception.h"

static const int SSID_LENGTH     	= IW_ESSID_MAX_SIZE; // should be 32
static const int NUMBER_WEP_KEYS 	= 4;
static const int WEP_KEY_LENGTH  	= 26;
static const int MAC_ADDRESS_LENGTH 	= 6;
static const int NUMBER_PARSE_ITEMS     = 11; // configuration file settings
static const int SCANNING_TIMEOUT       = 3000000; // 1 second in total
static const int SCANNING_INTERVAL      = 250000; // 100 ms for one attemp

static const int MHZ = 1000000; // 1 MHz = 1000000 Hz
static const int IEEE_80211_INTERFERENCE_FREE_MAX_SPACING = 5825-2412; // MHz
static const int IEEE_80211_INTERFERENCE_FREE_SPACING  = 5*4; // 20MHz
static const int IEEE_80211G_INTERFERENCE_FREE_SPACING = 5*4; // 20MHz
static const int IEEE_80211A_INTERFERENCE_FREE_SPACING = 5*4; // 20MHz

static const int IEEE_80211G_CHANNELS[] = {1, 2,  3, 4, 5, 6, 7, 8, 9,10,11,12, 13, 14};
static const int IEEE_80211A_CHANNELS[] = {36,40,42,44,48,50,52,56,58,60,64,149,152,153,157,160,161,165};
static const int IEEE_80211_CHANNELS[]  = {1, 2,  3, 4, 5, 6, 7, 8, 9,10,11,12, 13, 14,
					   36,40,42,44,48,50,52,56,58,60,64,149,152,153,157,160,161,165};

/**
 * NetworkScanResultT defines the results returned by a wireless network 
 * scanning operation. The result includes the channels occupied by existing wireless 
 * network and the corresponding ssids or network names.
 */
typedef struct NetworkScanResultT {
  short       channel;               // occupied channel
  double      frequency;             // channel's frequency (MHz)
  char        ssid[SSID_LENGTH+1];   // network ssid/name
  NetworkScanResultT *next;          // next item
} NetworkScanResultT;

/**
 * WirelessStandardT defines the IEEE802.11 wireless LAN standards
 */
typedef enum {
  IEEE_80211AUTO,
  IEEE_80211A,
  IEEE_80211B,
  IEEE_80211G,
} WirelessStandardT;

/**
 * WirelessRegionT defines the region option of EU, JP, NA
 */
typedef enum {
  EU,
  JP,
  NA,
} WirelessRegionT;

/**
 * AuthenticationOptionT defines the authentication option
 */
typedef enum {
  AUTH_OPEN,
  AUTH_SHARED,
} AuthenticationOptionT;

/**
 * WepKeyLengthT defines the length of a WEP encryption key
 */
typedef enum {
  WEP64 = 10,
  WEP128 = 26
} WepKeyLengthT;

/**
 * WepKeyT defines a single WEP encryption key. 
 *
 * NOTE: these are characters, not hex values
 */
typedef struct {
  int id; // starts with 0
  WepKeyLengthT length;
  char key[WEP_KEY_LENGTH+1];
} WepKeyT;


/**
 * RadioModeT defines the radio mode of operation. this enum matches the array 
 * iw_operation_mode in iwlib.c
 */
typedef enum {
  MODE_AUTO = 0,
  MODE_ADHOC,
  MODE_MANAGED,
  MODE_MASTER,
  MODE_REPEATER,
  MODE_SECONDARY,
  MODE_MONITOR
} RadioModeT;

/**
 * LinkQualityT defines the link quality and signal strength/level of a WLAN radio.
 */
typedef struct {
  short linkQuality;
  short signalLevel;
} LinkStatusT;

/**
 * MacAddressT defines the MAC address of an ethernet network interface.
 */
typedef struct {
  unsigned char addr[MAC_ADDRESS_LENGTH];
} MacAddressT;

#endif // _WLAN_DEF_
