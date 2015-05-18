/******************************************************************************
 * File: wlan_common.h
 * ---------------------------------------------------------------------------
 *
 *
 * Description:
 * ---------------
 * This file defines the application programming interface for accessing and 
 * modifying the status of a generic wireless local area (wlan) driver 
 * parameters. Although the same goal can be achieved via Wireless Tool for 
 * Linux, such as iwconfig, iwpriv, iwlist, etc., it is more difficult, 
 * if not impossible, to verify the result of these commands. 
 *
 * Compared to the Wireless Tool For Linux API, the developer don't need to 
 * deal with the low-level ioctl function calls when using this API. Thus, 
 * you can treat this API as a higher-level wrapper for those in the 
 * Wireless Tool for Linux API. 
 * 
 * To see more on wireless extension for linux, visit: 
 * //http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux/Tools.html
 *
 * Thus, this API serves two purposes, among others: 
 * 1. provides a wrapper for the Wireless Tool for Linux API
 * 2. provides verifications for the status change of a wlan driver. 
 *
 * A third goal is to make this API driver agnostic so that it can be applied
 * for different wlan drivers.
 *
 * SetCountryCode has to be down manually for madwifi, thus I chose not to 
 * provide such a method:
 * <ifconfig ath0 down> <modprobe ath_pci -i countrycode>
 * <rmmod ath_pci> to take it down, but never do this
 *
 * Assumption(s):
 * ---------------
 * your wlan driver must conform to the Wireless Extension for Linux (WE).
 *
 * History:
 * --------------------------------------------------------------------------
 * JP         11/02/2005          Original 
 *
 ****************************************************************************/

#ifndef _WLAN_COMMON_
#define _WLAN_COMMON_

#include "wlan_def.h"
#include "../LoadFile/LdProp.h"
#include "../LogTrace/LogTrace.h"

using namespace std;

#define WLAN_COMMON_DEBUG

#define REGION		"region"
#define STANDARD	"standard"
#define RADIOMODE       "radioMode"
#define WPKENABLED      "dot11PrivacyInvoked"
#define OPMODE          "operationMode"
#define SSID            "SSID"
#define CHANNEL         "CHANNEL"
#define OUTPOWER        "outputPower"

#define WEPKEYLEN       "PRIV_KEY128"
#define WEPKEY0         "dot11WEPDefaultKey0"
#define WEPKEY1         "dot11WEPDefaultKey1"
#define WEPKEY2         "dot11WEPDefaultKey2"
#define WEPKEY3         "dot11WEPDefaultKey3"
#define KEYINDEX        "dot11WEPDefaultKeyID"
#define AUTHTYPE  	"AuthType"

/**
 * wlan_common defines a generic application programming interface 
 * for accessing the status of a WLAN radio and modifying its 
 * operating parameters. The WLAN radio driver must conform with the
 * Wireless Extension for Linux (WE).
 *
 * NOTE: For all the setXXX methods, you need to have root privilidge 
 *       in order to change the state of the wireless device/radio. 
 */
class wlan_common {
 public:
 
 /**
  * This is the only constructor for wlan_common. It checks the 
  * given network interface, ifname, to see whether it is up and 
  * running and supports the Wireless Extension for Linux (WE). 
  * 
  * If it supports the WE, this contructor initializes the 
  * wlan_common object by setting its state with the parameters
  * specified by the filename.
  *
  * Furthermore, if initRadio is true, then these settings
  * will be applied to the radio. In other words, if initRadio
  * is false, the radio settings and the wlan_common object
  * state may be out of synchronization. However, this is 
  * purposely designed this way for the case where we know for
  * sure that the radio is using the settings specified
  * by filename configuration file, and we don't want to interrupt
  * the operation of the radio because it may have ongoing
  * data communications.
  *
  * @param ifname the wireless network interface name.
  * @param filename configuration file
  * @param initRadio whether radio needs to be initialized
  *
  * @throw INTERFACE_NAME_TOO_LONG_FAILURE* if ifname is too long
  * @throw EMPTY_INTERFACE_NAME_FAILURE* if ifname is an empty string
  * @throw ALLOCATE_MEMORY_FAILURE* if dynamic memory allocation failed
  * @throw WIRELESS_EXTENSION_UNSUPPORTED* if the OS/radio does not support wireless extension
  * @throw WIRELESS_INTERFACE_DISABLED* if the interface is deactivated
  * @throw RADIO_INITIALIZATION_FAILURE* if the radio initialization failed. This happens only
  *        when initRadio is true.
  */
  wlan_common(char *ifname, char *filename, bool initRadio = false)
  throw (INTERFACE_NAME_TOO_LONG_FAILURE*, 
	 EMPTY_INTERFACE_NAME_FAILURE*, 
	 ALLOCATE_MEMORY_FAILURE*,
	 WIRELESS_EXTENSION_UNSUPPORTED*, 
	 WIRELESS_INTERFACE_DISABLED*,
	 RADIO_INITIALIZATION_FAILURE*);
  
  /**
   * The destructor frees all the dynamic allocated 
   * memory.
   */
  ~wlan_common();
   
  /**
   * This method sets radio using wlan_common object
   * @throw INVALID_ARGUMENT* when at least one of the
   *        arguments in object is invalid.
   */
  void SetAll()
  throw (INVALID_ARGUMENT*);

  /**
   * This method sets the WEP keys for the wlan_common
   * object as well as the radio involved, if enabled
   * is true; otherwise, it disables the WEP encryption. 
   * 
   * @param enabled enable or disable the WEP encryption
   * @param useKeyIndex the default WEP key index
   * @param keys the actual WEP keys to be used
   * @param numberKeys number of keys in keys
   *
   * @throw INVALID_ARGUMENT* when at least one of the 
   *        arguments is invalid. For instance, useKeyIndex
   *        is out of range, or numberKeys is too big.
   * @throw KEY_CONFIGURE_FAILURE* when the actual "set key"
   *        operation failed.
   */
  void SetWepKeys(bool enabled, unsigned short useKeyIndex, 
		  WepKeyT *keys, unsigned short numberKeys) 
    throw (INVALID_ARGUMENT*,KEY_CONFIGURE_FAILURE*);
  
  /**
   * This method sets the authentication option for the object and
   * and the radio.
   *
   * @param authOption authentication option to be used
   *
   * @throw AUTH_CONFIGURE_FAILURE* if the operation failed
   */ 
  void SetAuthOption(AuthenticationOptionT authOption)
    throw (AUTH_CONFIGURE_FAILURE*);
  
  /**
   * This method sets the channel for the objet and the radio.
   *
   * @param channel the channel number to be used
   * 
   * @throw INVALID_ARGUMENT* if the given channel is invalid to 
   *        the radio. A channel becomes valid either because it
   *        is out of the valid range for the operating standard
   *        or protocol, or because it is not allowed for the 
   *        country code that radio is set to work.
   * @throw CHANNEL_CONFIGURE_FAILURE* if the set operation failed
   */
  void SetChannel(unsigned short channel)
    throw (CHANNEL_CONFIGURE_FAILURE*, INVALID_ARGUMENT*);
  
  /**
   * This method sets the mode of operation for the object
   * and the radio.
   *
   * @param mode the opeartion mode to be used
   *
   * @throw MODE_CONFIGURE_FAILURE* if the operation failed
   */
  void SetRadioMode(RadioModeT mode)
    throw (MODE_CONFIGURE_FAILURE*);
  
  /**
   * This method sets the wireless network name or ssid for 
   * the object and the radio.
   *
   * @param ssid the SSID to be used
   * @param useAnyOrOff if true, we allow SSID 
   *        "any" or "off" to be treated as
   *        a special SSID, otherwise the SSID 
   *        is literally "any" or "off".
   *
   * @throw INVALID_ARGUMENT* if at least one of the arguments
   *        is invalid. For instance, when the radio is in 
   *        MODE_MASTER, its ssid cannot be "any".
   * @throw SSID_CONFIGURE_FAILURE* if the operation failed
   */
  void SetSSID(char *ssid, bool useAnyOrOff = false) 
    throw (INVALID_ARGUMENT*,SSID_CONFIGURE_FAILURE*);
  
  /**
   * This method retrieves the WEP keys information. If fromRadio
   * is true, the information is retrieved from the radio itself;
   * otherwise, the information is from the object. 
   *
   * @param enabled returns whether the WEP encryption is enabled
   * @param useKeyIndex returns which key index is used
   * @param keys returns the WEP keys
   * @param numberKeys specifies the number of WEP keys to be returned
   * @param fromRadio specifies whether the information is from the 
   *        radio itself or from the object.
   *
   * @throw INVALID_ARGUMENT* if the operation failed.
   */
  void GetWepKeys(bool *enabled, unsigned short *useKeyIndex, 
		  WepKeyT *keys, unsigned short numberKeys, bool fromRadio = true)
    throw (INVALID_ARGUMENT*);

  /**
   * This method retrieves the authentication option. If fromRadio
   * is true, the returned value is from the radio; otherwise,
   * it is from the object.
   *
   * @param authOption the authentication option is used
   * @param fromRadio specifies whether the returned information
   *        is from the radio or from the object.
   * @throw GET_AUTH_OPTION_FAILURE* if the operation failed.
   */	  
  void GetAuthOption(AuthenticationOptionT *authOption, bool fromRadio = true)
    throw (GET_AUTH_OPTION_FAILURE*);

  /**
   * This method retrieves the current operating channel. If
   * fromRadio is true, the returned information is from the 
   * radio; otherwise, the information is from the object.
   *
   * @param channel the channel is used by the radio
   * @param fromRadio specifies whether the returned information
   *        is from the radio or from the object.
   * @throw GET_CHANNEL_FAILURE* if the operation failed
   */	
  void GetChannel(unsigned short *channel, bool fromRadio = true) throw (GET_CHANNEL_FAILURE*);
	
  /**
   * This method retrieves the radio mode. If fromRadio is
   * true, the returned value is from the radio; otherwise,
   * it is from the object.
   *
   * @param mode the mode is used by the radio.
   * @param fromRadio specifies whether the returned information
   *        is from the radio or from the object.
   * @throw GET_MODE_FAILURE* if the operation failed
   */
  void GetRadioMode(RadioModeT *mode, bool fromRadio = true) throw (GET_MODE_FAILURE*);
	
  /**
   * This method retrieves the wireless network name/SSID. If fromRadio is
   * true, the returned value is from the radio; otherwise,
   * it is from the object.
   *
   * @param ssid the SSID is used by the radio
   * @param length the maximum number of characters 
   *        allowed for ssid
   * @param fromRadio specifies whether the returned information
   *        is from the radio or from the object.
   * @throw BUFFER_TOO_SMALL_FAILURE* if ssid cannot hold the 
   *        returned network name or SSID
   * @throw GET_SSID_FAILURE* if the operation failed
   */
  void GetSSID(char *ssid, int length, bool fromRadio = true)
    throw (BUFFER_TOO_SMALL_FAILURE*, GET_SSID_FAILURE*);
  
  /**
   * This method retrieves the link status. This inforamtion
   * is ALWAYS retrieved from the radio itself in real-time.
   *
   * @param status the current link status of the radio.
   * @throw GET_LINK_STATUS_FAILURE* if the operation failed.
   */
  void GetLinkStatus(LinkStatusT *status) throw (GET_LINK_STATUS_FAILURE*);
  
  /**
   * This method retrieves the MAC address of the network interface
   * associated with this object. 
   *
   * @param mac the returned mac address
   * @throw GET_MAC_ADDRESS_FAILURE* if the operation failed
   */
  void GetMacAddress(MacAddressT *mac) throw (GET_MAC_ADDRESS_FAILURE*);
  
  /**
   * This method displays the MAC address in the usual format
   * xx:xx:xx:xx:xx:xx
   *
   * @param mac the MAC address
   */
  void ShowMacAddress(MacAddressT *mac);

  /**
   * This method disables WEP encryption feature so that no 
   * encryption is to be used for data communication.
   *
   * @throw KEY_CONFIGURE_FAILURE* exception if operation
   *        fails 
   */ 
  void DisableWep(void) throw (KEY_CONFIGURE_FAILURE*);


  /**
   * This method persists the states of the object to 
   * a file which can later be served as a configuration file
   * to initialize the radio's settings.
   *
   * @param filename the file storing the states of the object
   * @throw INVALID_ARGUMENT* exception if operation failed 
   * Note: Due to format changes don't use this method!
   */  
  void Serialize(const char *filename) throw (INVALID_ARGUMENT*);
    
  /**
   * @return the network interface name 
   */
  const char *GetInterfaceName();

  /**
   * This method retrieves the standard/protocol being used.
   * If fromRadio is true, this information is retrieved from
   * the radio; otherwise, the returned value is from the object.
   *
   * @param standard return the IEEE802.11 standard being used
   * @param fromRadio specifies whether the returned information
   *        is from the radio or from the object.
   * @throw UNKNOWN_STANDARD* if the standard is not part of
   *        the supported standards/protocols as specified by
   *        WirelessStandardT.
   * @throw GET_STANDARD_FAILURE* if the operation failed
   */
  void GetWirelessStandard(WirelessStandardT *standard, bool fromRadio = true) 
    throw (GET_STANDARD_FAILURE*, UNKNOWN_STANDARD*);

  /**
   * This method sets the standard/protocol.
   *
   * @param standard change the IEEE802.11 standard being used 
   *        to this standard.  
   * @throw STANDARD_CONFIGURE_UNSUPPORTED* if the radio does not
   *        allow the protocol to be changed
   * @throw STANDARD_CONFIGURE_FAILURE* if the operation failed
   */
  void SetWirelessStandard(WirelessStandardT standard) 
    throw (STANDARD_CONFIGURE_FAILURE*, STANDARD_CONFIGURE_UNSUPPORTED*);


  /**
   * IsChannelValid tells whether the given channel is valid for the current 
   * operating radio. 
   *
   * Note that whether a given channel is valid depends on several factors:
   * 1. the standard that the radio is operating, i.e., 802.11G/B or 802.11A
   * 2. the regulatory domain that the radio is configured for. 
   *
   * @param channel the channel to test
   * @throw CHANNEL_CONFIGURE_FAILURE* if the operation fails
   * @return whether the given channel is valid
   */  
  bool IsChannelValid(unsigned short channel) 
    throw (CHANNEL_CONFIGURE_FAILURE*);
  
  /**
   *	The list of inline functions for access class members directly
   *	Added on Mar. 10, 2006 
   */

  inline WirelessStandardT getStandard() { return m_standard; };
  inline RadioModeT getMode() { return m_mode; };
  inline int getWepEnabled() { return m_wepEnabled; };
  inline AuthenticationOptionT getAuthOption() { return m_authOption; };
  inline WepKeyT* getWepKeys() { return &m_wepKeys[0]; };
  inline int getUseKeyIndex() { return m_useKeyIndex; };
  inline int getChannel() { return m_channel; };
  inline char* getSsid() { return &m_ssid[0]; };
  inline int getRegion() { return m_region; };
  inline bool isRadioOn() { return m_avail?true:checkAvail(); };
 
  /**
   * m_wlist is the list of wireless properties that can be read or write
   * to a config file
   */
  LdProp	m_wlist;
  LogTrace*     m_Log;

  inline void setLog(LogTrace*log) { m_Log = log; }; 

 private:

  /**
   * checkAvail method to give current wireless avail support
   */
  bool  checkAvail();

  /**
   * m_avail the flag which indicates the radio availability
   */
  bool	m_avail;

  /**
   * m_region is decided by contry code. 0: EU  1: JP  2: NA
   */
  int m_region;

  /**
   * m_standard keeps track of the protocol/standard that the radio
   * is using.
   */
  WirelessStandardT m_standard;          // protocol being used
  
  /**
   * m_mode keeps track of the mode of operation
   */
  RadioModeT     m_mode;                 // radio mode

  /** 
   * m_ifname keeps track of the wireless network interface name
   */
  char   m_ifname[IFNAMSIZ+1];          // interface name
  
  /**
   * m_mac keeps track of the MAC address of the interface
   */
  MacAddressT* m_mac;                    // mac address
  
  /**
   * m_wepEnabled keeps track of whether WEP encryption is enabled
   */
  int     m_wepEnabled;                  // whether WEP is used
  
  /**
   * m_authOption keeps track of the authentication option
   */
  AuthenticationOptionT     m_authOption;// authentication option
  
  /**
   * m_wepKeys stores all the default WEP encryption keys.
   */
  WepKeyT m_wepKeys[NUMBER_WEP_KEYS];    // default wep keys

  /**
   * m_useKeyIndex indicates the default WEP key index
   */
  int     m_useKeyIndex;                 // default key index
 
  /**
   * m_channel is the current operating channel. It only
   * makes sense for MODE_ADHOC or MODE_MASTER radio.
   */
  int     m_channel;                     // current channel

  /**
   * m_ssid is the SSID or network name
   */
  char    m_ssid[SSID_LENGTH+1];         // ssid or network name

  /**
   * AreWepKeysValid checks whether the given WEP keys are valid
   *
   * @param keys the WEP keys to be validated
   * @param numberKeys the number of keys to be validated
   * @return whether the keys are valid or not. If any key is invalid,
   *         it returns false. If all keys are valid, it returns true. 
   */
  bool AreWepKeysValid(WepKeyT *keys, unsigned short numberKeys);

  /**
   * CopyMacAddress copies the MAC from source to target.
   *
   * @param source source of the MAC address
   * @param target target of the MAC address
   */
  void CopyMacAddress(MacAddressT *source, MacAddressT *target);

  /**
   * IsWirelessExtensionSupported tells whether the radio driver supports
   * WE.
   *
   * @param ifname network interface name
   * @return whether ifname supports wireless extension.
   */
  bool IsWirelessExtensionSupported(char *ifname);

  /**
   * IsWirelessInterfaceEnabled tells whther the given network interface
   * is activated or enabled.
   *
   * @param ifname network interface name
   * @return whether ifname is enabled or activated
   */ 
  bool IsWirelessInterfaceEnabled(char *ifname);

  /**
   * Initialize initializes the object with the settings specified in the 
   * given configuration file. Further, if initRadio is true, the radio
   * itself also is initialized with the settings specified in the 
   * configuration file.
   *
   * @param filename the file containing the settings
   * @param initRadio whether the radio itself needs to be initialized
   *        with the settings
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   * @return whether the radio has been configured successfully with the 
   *         settings 
   */
  bool Initialize(char *filename, bool initRadio = false) 
    throw (PARSE_CONFIGURATION_FILE_FAILURE*);

  /**
   * CheckConstructorArgument valids the constructor arguments
   *
   * @param ifname network interface name
   * @param filename configuration file name 
   * @throw INTERFACE_NAME_TOO_LONG_FAILURE* if the string of ifname is too long
   * @throw EMPTY_INTERFACE_NAME_FAILURE* if ifname is an empty string
   * @throw ALLOCATE_MEMORY_FAILURE* if no enougth memory available
   */ 
  void CheckConstructorArgument(char *ifname)
    throw (INTERFACE_NAME_TOO_LONG_FAILURE*, 
	   EMPTY_INTERFACE_NAME_FAILURE*, 
	   ALLOCATE_MEMORY_FAILURE*);

  /**
   * CopyWepKey copies the source WEP key to the target WEP key
   * @param source the key to be copied
   * @param target the key to be copied to
   */
  void CopyWepKey(WepKeyT *target, WepKeyT *source);

#ifdef ORIGINAL_READ

  /**
   * ReadIntegerValueFromString parses the string of a pair of
   * name=value bindings.
   *
   * @param string the input string  
   * @param name name of the bindings
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   * @return the integer specified by the string
   */ 
  int ReadIntegerValueFromString(char *string, char *name)
    throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadStandardFromString parses the string specifying the standard/protocol
   *
   * @param string string specifying the standard
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadStandardFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);


  /**
   * ReadSSIDFromString parses the string specifying the ssid
   *
   * @param string string specifying the ssid 
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadSSIDFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadChannelFromString parses the string specifying the channel
   *
   * @param string string specifying the channel 
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadChannelFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);

  /**
   * ReadAuthOptionFromString parses the string specifying auth option value
   *
   * @param string string specifying aut option 
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadAuthOptionFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadWepEnabledFromString parses the string and read in the wep eanbled value
   *
   * @param string the string specifying wep enabled value
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadWepEnabledFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadRadioModeFromString reads radio mode from a string
   *
   * @param string the string specifying a radio mode
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadRadioModeFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadUseKeyIndexFromString reads the use key index from a string
   *
   * @param string the string containing a key index
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadUseKeyIndexFromString(char *string) throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
  /**
   * ReadOneWepKeyFromString  parses the string to obtain a WEP key and save this
   * key internally.
   *
   * @param keyString the string to be parsed containing one WEP key
   * @param getall a boolean array used to see which key has been read from the string
   * @throw PARSE_CONFIGURATION_FILE_FAILURE* if the operation fails
   */ 
  void ReadOneWepKeyFromString(char *keyString, bool *getall)
    throw (PARSE_CONFIGURATION_FILE_FAILURE*);
  
#endif

  /**
   * EncodeOneWepKey encodes the given WEP key in a format understood by the radio
   * driver.
   *
   * @param wepKey the WEP key to be encoded
   * @param encodedKey the encoded key
   * @param keyLength the length of the encoded key
   */ 
  void EncodeOneWepKey(WepKeyT *wepKey, unsigned char *encodedKey, int *keyLength);
};

#endif // _WLAN_COMMON_










