/******************************************************************************
 * File: wlan_advanced.h
 * ---------------------------------------------------------------------------
 *
 * Description:
 * ---------------
 * This file defines some utility functions based on wlan_common class. It 
 * provides the following features:
 *
 * 1. perform a scanning for the existing wireless network
 * 2. compute the next available SSID based on a user-defined prefix
 * 3. compute the optimal clear channel
 * 4. indicate whether wireless interference exists
 * 
 * Assumption(s):
 * ---------------
 * your wlan driver must conform to the Wireless Extension for Linux (WE). 
 *
 * History:
 * -------------------------------------------------------------------------
 * JP         10/15/2005          Original 
 *
 ****************************************************************************/

#ifndef _WLAN_ADVANCED_
#define _WLAN_ADVANCED_

#include "wlan_common.h"
#include "wlan_exception.h"

using namespace std;

// #define WLAN_ADVANCED_DEBUG

/**
 * wlan_advanced provides several utility functions based on wlan_common 
 * class. It provides the following features:
 *
 * 1. perform a scanning for the existing wireless network
 * 2. compute the next available SSID based on a user-defined prefix
 * 3. compute the optimal clear channel
 * 4. indicate whether wireless interference exists
 */
class wlan_advanced {
 public:
  /**
   * wlan_advanced is the only constructor for wlan_advanced class. 
   * @param wlandriver a pointer to a wlan_common object.
   */
  wlan_advanced(wlan_common *wlandriver);
  
  /**
   * ~wlan_advanced is the only destructor for wlan_advanced class.
   * It frees the memory space occupied by an wlan_advanced object.
   */
  ~wlan_advanced();

  /**
   * ScanForExistingWirelessNetworks performs active scanning for existing 
   * wireless networks.
   *
   * NOTE: The radio MUST operate in MODE_MANAGED mode in order to perform a 
   * scanning.
   * 
   * WARNING: Scanning can interrupt the ongoing work on the existing wireless
   * network. It is recommended only when you are sure that no useful network
   * operations are being performed. 
   *
   * @param times number of active scanning operations requested
   * @throw SCAN_NETWORK_FAILURE* if the scanning operation failed
   * @return a linked list of the scanning results
   */
  NetworkScanResultT *ScanForExistingWirelessNetworks(int times = 1) throw (SCAN_NETWORK_FAILURE*);

  /**
   * ComputeOptimalChannel computes the optimal channel given on the last 
   * scanning results. 
   * 
   * This method can be used to set the best operating channel of a wireless
   * radio so that wireless interference can be reduced or minimized.
   *
   * @param list a linked list of scanning results
   * @throw CLEAN_CHANNEL_UNAVAILABLE* if no interference-free channel exists
   * @return the optimal channel. 
   */
  int ComputeOptimalChannel(NetworkScanResultT *list) 
    throw (CLEAN_CHANNEL_UNAVAILABLE*, COMPUTE_OPTIMAL_CHANNEL_FAILURE*);

  /**
   * ComputeNextAvailableSSID computes a SSID with no name conflict given  
   * the last scanning results.
   *
   * This method can be used to set the SSID of a wireless network so that 
   * SSID name conflict can be prevented.
   *
   * @param prefix the SSID prefix. The next available ssid is a 
   *        concatenation of this prefix with a numeric value. For instance, 
   *        if the prefix is "myssid", min = 1, max = 11, and the existing 
   *        SSID's are "myssid01", "myssid2", "myssid3", "myssid4", and 
   *        "otherssid", the next available SSID is "myssid1", and the return 
   *        value for this method should be 1. We only consider directly 
   *        appending a natural number to a prefix, thus "myssid01" is not
   *        considered to be a SSID with a prefix of "myssid". Instead, the 
   *        prefix for "myssid01" is "myssid0". 
   *
   * @param min the lowest number we would like to search
   * @param max the highest number we would like to search. Thus, the search range
   *        is between min and max, inclusive.
   * @param list the scanning results
   *
   * @throw SSID_UNAVAILABLE* if name conflict is impossible
   * @throw INVALID_ARGUMENT* if at least one of the arguments are invalid. 
   *        For instance, if min > max.
   * @throw ALLOCATE_MEMORY_FAILURE* if there is no enough memory
   * @return the next available ssid based on the prefix
   */
  unsigned short ComputeNextAvailableSSID(char *prefix, unsigned short min, 
					  unsigned short max, NetworkScanResultT *list)
    throw (SSID_UNAVAILABLE*, ALLOCATE_MEMORY_FAILURE*, INVALID_ARGUMENT*);

  /**
   * GetDriver returns the wlan_common object associated with this object.
   *
   * @return a pointer to the wlan_common object
   */
  const wlan_common *GetDriver();

  /**
   * InterferenceIsDetected tells whether wireless interference has been 
   * detected given the scanning results and the channel to be used.
   *
   * @param list a linked list of scanning results
   * @param channel the channel to be used
   * @param spacing the interference free spacing for neighboring channels in MHz
   * @throw DETECT_INTERFERENCE_FAILURE* if operation failed
   * @return whether interference has been detected.
   */
  bool InterferenceIsDetected(NetworkScanResultT *list, int channel, 
			      int spacing = IEEE_80211_INTERFERENCE_FREE_SPACING)
    throw (DETECT_INTERFERENCE_FAILURE*);
  
  /**
   * DisplayScanResult displays the scanning results to the standard 
   * output. 
   *
   * @param list a linked list of scanning results
   */
  void DisplayScanResult(NetworkScanResultT *list);

  
 private:

  /**
   * m_driver keeps a pointer to a wlan_common object so that scanning 
   * operation can be performed to the associated wireless radio.
   */
  wlan_common *         m_driver;       // a wrapper for a wlan driver

  /**
   * BestChannelT is an internal data structure for finding the best
   * or optimal channel.
   */ 
  typedef struct {
    int channel;        // channel number
    double minDistance; // channel distance to its neighbors in MHz
  } BestChannelT; 

  /**
   * m_scan keeps a copy of the previous scanning results in a linked list 
   */
  NetworkScanResultT *  m_scan;      
  
  /** 
   * FreeNetworkScanResult frees the memory occupied by the list.
   *
   * @param list a linked list of the scanning results
   */
  void FreeNetworkScanResult(NetworkScanResultT *list);
    
  /**
   * itoa converts a number to a ASCII character string
   *
   * @param number the number to be converted
   * @throw ALLOCATE_MEMORY_FAILURE* if no enough memory is available
   * @return a string representation of the given number
   */
  char *itoa(unsigned short number) throw (ALLOCATE_MEMORY_FAILURE*);
 
  /**
   * InitiateScanning actually initiates the scanning operation
   *
   * @param skfd an open socket
   * @param ifname the network interface name
   * @param range a returned pointer to an iwrange object
   * @param request the actual request sent to the radio
   * @return whether this operation succeeded
   */
  bool InitiateScanning(int skfd, const char *ifname, iwrange *range, 
			iwreq *request);

  /**
   * UpdateScanningResults reads the scanning results and updates 
   * the state of this wlan_advanced object accordingly
   *
   * @param skfd an open socket
   * @param ifname the network interface name
   * @param range a returned pointer to an iwrange object
   * @param request the actual request sent to the radio
   * @param buffer a returned buffer holding dynamically allocated 
   *        memory to store the scanning results
   */ 
  void UpdateScanningResults(int skfd, const char *ifname, 
			     iwrange *range, iwreq *request, char *buffer);

  /**
   * IsScanningSupported tells whether the radio/driver supports
   * active scanning.
   *
   * @param skfd an open socket
   * @param ifname the network interface name
   * @param range a returned pointer to an iwrange object
   * @return whether scanning is supported 
   */
  bool IsScanningSupported(int skfd, const char *ifname, iwrange *range);

  /**
   * GetOneScanningToken read one item at one time from the 
   * scanning results.  
   *
   * @param event represents the next item from the scanning
   *        result
   * @param ap_num the current access point detected
   * @param range a iw_range helps decode the result
   * @param scan store the scanned result
   * @return the next access pointer number 
   */
  int GetOneScanningToken(struct iw_event *event,
			  int ap_num,
			  struct iw_range *range, 
			  NetworkScanResultT *scan);

  
  /**
   * FindScanItemInList finds whether the given NetworkScanResultT item is
   * in the linked list of NetworkScanResultT objects.
   * @param item the object to be found
   * @param list a linked list of NetworkScanResultT objects
   * @return true if the item is found in the list; false otherwise.
   */
  bool FindScanItemInList(NetworkScanResultT *item, NetworkScanResultT *list);
  
  /**
   * CopyOneScanItem copies the source NetworkScanResultT object to the target
   * @param source the NetworkScanResultT object to be copied
   * @param target the NetworkScanResultT object to be copied to
   */ 
  void CopyOneScanItem(NetworkScanResultT *source, NetworkScanResultT *target);  
  
  /**
   * ComputeOptimalChannel is overridden to actually compute the optimal channel
   *
   * @param list linked list of scanning results
   * @param channels the array of channels for a given protocl/standard
   * @param numberChannels the size of the array channels
   * @param spacing the interference-free distance between neighboring channels in MHz
   *
   * @throw COMPUTE_OPTIMAL_CHANNEL_FAILURE* if operation failed
   * @throw CLEAN_CHANNEL_UNAVAILABLE* if no interference-free optimal channel exists
   * @return one of the optimal channels 
   */
  int ComputeOptimalChannel(NetworkScanResultT *list, const int *channels, int numberChannels, int spacing)
    throw (COMPUTE_OPTIMAL_CHANNEL_FAILURE*, CLEAN_CHANNEL_UNAVAILABLE*);
};

#endif // _WLAN_ADVANCED_







