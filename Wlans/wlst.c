/*****************************************************************************
 * File: wlan_startup.c
 * ---------------------------------------------------------------------------
 * the sole purpose of this file is to perform a one-time scanning and then
 * based on the scanning results, configure the driver and update the 
 * configuration files if necessary.
 *
 * Author: Jianpeng Dong
 * Date:   01/19/2004
 *
 * History:
 * --------------------------------------------------------------------------
 * JP         01/19/04          Added support for PRISM54 driver
 * JP         02/02/04          Added flexible model, version and ssid pattern
 *                              handling
 * JP         02/26/04          Added infrastructure mode and manual ssid input
 *                              handling
 * JP         03/09/04          put ssid auto discovery back in with a different
 *                              scheme based on user's entry.
 *                              factory default handling
 *****************************************************************************/

#include <stdlib.h> // for system
#include <stdio.h>
#include <string.h> // for strlen, strncat, etc
#include "wlan_common.h"

#ifdef PRISM54
  #include "iwlib.h"
  #include <sys/time.h>
#endif

int main(void)
{ 
  int  i, maxch;
  short  channel, ssidIndex;
  short one = 0x0001;
  unsigned char linebuf[1024];
  int found, interfere, len, changed;
  unsigned char *ssid_base, extended_ssid[33], list[1024];
  struct wj_scan_t scan;
  config wlan_conf;
  FILE *out; 

  char          mode[24];
  unsigned long currentChannel = 0;
#ifdef PRISM54
  int           skfd;
#endif
    
  //======================== initialization ==============================
  out = fopen(WJ_START_SCAN_FILE, "w");
  if (!out) {
    fprintf(stderr, "Failed to write into persistant storage\n");
    return -1;
  }

  found = 0;
  interfere = 0;
  len = 0;
  changed = 0;
  list[0] = '\0';

#ifdef WJ_START_DEFAULT
  GetConfigurationFromFile(&wlan_conf, WLAN_CONF_DEFAULT);
#else
  GetConfigurationFromFile(&wlan_conf, WLAN_CONF_FILE);
#endif
  channel = wlan_conf.channel;
  ssidIndex = 0;
  ssid_base = wlan_conf.ssid;

  switch(wlan_conf.nmode) {
  case 1: // infrastructure station
    sprintf(mode, "Infrastructure Station");
    break;
  case 2:
    sprintf(mode, "Ad Hoc Station");
    break;
  default:
    sprintf(mode, "Local Access Point");
    break;
  }

  // ===================== scan and optimize =======================
  ScanForExistingWirelessNetworks(&scan, ssid_base);
  // DisplayScanResult(&scan);
  if (wlan_conf.omode == 0) { // automatic mode
    // search for ideal channel
    if (FindOptimalChannel(scan.channels, &channel, wlan_conf.region) == WJ_SUCCESS) {
      interfere = 0;
    } else {
      interfere = 1;
    }
    // prevent from using illegal channel
    switch(wlan_conf.region) {
      case 0:
        if (channel < 0 || channel > WJ_START_EU_CHANNELS) channel = 6;
        break;
      case 1:
        if (channel < 0 || channel > WJ_START_JP_CHANNELS) channel = 6;
        break;
      default:
        if (channel < 0 || channel > WJ_START_NA_CHANNELS) channel = 6;
        break;
      }

    if (channel != wlan_conf.channel) {
      SetChannel(channel, &wlan_conf);
      changed = 1;
    }

    // search for ideal ssid based on user entry
    FindAvailableSSID(scan.ssids, &ssidIndex);
    if (ssidIndex < 0 || ssidIndex >= 10) ssidIndex = 0;
    if (ssidIndex != 0) {
      sprintf(extended_ssid, ssid_base);
      extended_ssid[strlen(ssid_base)] = '0';
      extended_ssid[strlen(ssid_base) + 1] = '0' + ssidIndex;
      extended_ssid[strlen(ssid_base) + 2] = '\0';
      // fprintf(stderr, "extended ssid = %s\n", extended_ssid);
      SetSSID(extended_ssid, &wlan_conf);
      changed = 1;
    }
  } else {                     // manual mode
    if (wlan_conf.nmode != 1) { // AP mode or others
      if (DoesInterfere(scan.channels, wlan_conf.channel,
                        wlan_conf.region, wlan_conf.nmode) == WJ_SUCCESS) {
        interfere = 0;
      } else {
        interfere = 1;
      }
    } else { //Infrastructure STA
#ifdef PRISM54
      if ((skfd = iw_sockets_open()) >= 0) {
        if ((currentChannel = getCurrentChannel(skfd, "eth0")) <= 0) 
#else
        if (get_prismoid(WJ_START_IF_NAME, DOT11_OID_CHANNEL, (void *)&currentChannel, 4) != 0)
#endif
          printf("  Get current channel error!\n");
#ifdef PRISM54
        close(skfd);
      }
#endif

      if (DoesInterfere(scan.channels, (short)currentChannel,
                        wlan_conf.region, wlan_conf.nmode) == WJ_SUCCESS) {
        interfere = 0;
      } else {
        interfere = 1;
      }
    }   

  }
  
  //============= generate the display message on the banner =========
  switch(wlan_conf.region) {
  case 0: // EU
    maxch = WJ_START_EU_CHANNELS;
    break;
  case 1: // JP
    maxch = WJ_START_JP_CHANNELS;
    break;
  default: // NA
    maxch = WJ_START_NA_CHANNELS;
    break;
  }
  //
  // Generate RF activities info
  //
  sprintf(list, "RF activities detected on other channels (");
  for (i = 1; i <= maxch; i++) {
    if ((scan.channels >> (i-1)) & one) {
      if (i != wlan_conf.channel) {
	if (found) { // Not the 1st found channel
	  sprintf(linebuf, ",%d", i);
	} else { // first found channel
          sprintf(linebuf, "%d", i);
	}
	strcat(list, linebuf);
	found = 1;
      }
    }
  }
  if (found) {
    strcat(list, ")   ");
    if (interfere) {
      strcat(list, "WARNING: Interference");
    } 
  } else {
    strcat(list, "none)");
  }
  
  // ===================== save the information =======================
  if(out) {
    if (wlan_conf.nmode != 1)
      fprintf(out, "SSID=%s\nChannel=%d\n%s\nMode=%s\n", wlan_conf.ssid, wlan_conf.channel, list, mode);
    else
      fprintf(out, "SSID=%s\nChannel=%d\n%s\nMode=%s\n", wlan_conf.ssid, (short)currentChannel, list, mode);
 
    //fprintf(out, "SSID=%s\nChannel=%d\n%s\n", wlan_conf.ssid, wlan_conf.channel, list);
    fflush(out);
    fclose(out);
    system("sync");
  }

  // =============== configure the radio ====================
  // must initialize the driver, no matter what the mode is
  SetConfiguration(&wlan_conf, 1);
  return 0;
}








