/*****************************************************************************************
 * File: wlan_common.cc
 * --------------------------------------------------------------------------------------
 *
 *
 * Description:
 * ---------------
 * This file defines implements wlan_common class
 *
 * History:
 * --------------------------------------------------------------------------
 * JP         11/01/2005          Original 
 *
 ***************************************************************************************/
#include "wlan_common.h"

// based on get_info from iwconfig.c
void wlan_common::GetWirelessStandard(WirelessStandardT *standard, bool fromRadio)
  throw (GET_STANDARD_FAILURE*, UNKNOWN_STANDARD*)
{
  if (!fromRadio) {
    *standard = m_standard;
    return;
  }

  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_STANDARD_FAILURE();

  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_STANDARD_FAILURE();
  }
  
  if(strstr(config.name, "IEEE 802.11a")) {
    *standard = IEEE_80211A;
  } else if (strstr(config.name, "IEEE 802.11g")) {
    *standard = IEEE_80211G;
  } else if (strstr(config.name, "IEEE 802.11b")) {
    *standard = IEEE_80211B;
  } else if (strstr(config.name, "IEEE 802.11")) {
    *standard = IEEE_80211AUTO;
  } else {
    close(sockfd);
    throw new UNKNOWN_STANDARD();
  }
  close(sockfd);
}

// based on set_private and set_private_cmd from iwpriv.c
void wlan_common::SetWirelessStandard(WirelessStandardT standard)
  throw (STANDARD_CONFIGURE_UNSUPPORTED*, STANDARD_CONFIGURE_FAILURE*)
{
  iwreq request;
  iwprivargs *priv;
  int numberCommands;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new STANDARD_CONFIGURE_FAILURE();
  if ((numberCommands = iw_get_priv_info(sockfd, m_ifname, &priv)) <= 0) {
    throw new STANDARD_CONFIGURE_UNSUPPORTED();
  } 

  int k = -1;
  while((++k < numberCommands) && strcmp(priv[k].name, "mode"));
  if (k == numberCommands) {
    throw new STANDARD_CONFIGURE_UNSUPPORTED();
  }
  
  int subcmd = standard;
  int offset = 0;
  if (priv[k].cmd < SIOCDEVPRIVATE) {
    int j = -1;
    while((++j < numberCommands) && ((priv[j].name[0] != '\0') ||
				     (priv[j].set_args != priv[k].set_args) ||
				     (priv[j].get_args != priv[k].get_args)));
    if (j == numberCommands) throw new STANDARD_CONFIGURE_UNSUPPORTED();
    subcmd = priv[k].cmd;
    offset = sizeof(__u32);
    k = j;
  } 
  u_char buffer[4096];
  request.u.data.length = 1;
  if(request.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK)) {
    request.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;
  }
  ((__s32 *) buffer)[0] = (__s32) standard;
  strncpy(request.ifr_name, m_ifname, IFNAMSIZ);
  if (offset) request.u.mode = subcmd;
  memcpy(request.u.name + offset, buffer, IFNAMSIZ - offset);
  if (ioctl(sockfd, priv[k].cmd, &request) < 0) {
    throw new STANDARD_CONFIGURE_FAILURE();
  }
  m_standard = standard;
  close(sockfd);
}

bool wlan_common::Initialize(char *filename, bool initRadio)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{

#ifdef ORIGINAL_READ

  ifstream in(filename);
  if (!in.is_open()) {
    return false;
  }
  char line[1024];
  bool getall[NUMBER_PARSE_ITEMS];
  while(in.getline(line, 1023, '\n')) {
    if (strstr(line, "dot11WEPDefaultKey") && !strstr(line, "dot11WEPDefaultKeyID=")) {
      ReadOneWepKeyFromString(line, getall); 
    }
    if (strstr(line, "standard=")) {ReadStandardFromString(line); getall[4] = true;}
    if (strstr(line, "radioMode=")) {ReadRadioModeFromString(line); getall[5] = true;}
    if (strstr(line, "dot11PrivacyInvoked=")) {ReadWepEnabledFromString(line); getall[6] = true;}
    if (strstr(line, "dot11WEPDefaultKeyID=")) {ReadUseKeyIndexFromString(line); getall[7] = true;}
    if (strstr(line, "CHANNEL=")) {ReadChannelFromString(line); getall[8] = true;}
    if (strstr(line, "SSID=")) {ReadSSIDFromString(line); getall[9] = true;}
    if (strstr(line, "AuthType=")) {ReadAuthOptionFromString(line); getall[10] = true;}
  }    
  in.close();
  for(int i = 0; i < NUMBER_PARSE_ITEMS; i++) {
    if (!getall[i]) return false;
  }

#else
 
  m_wlist.noComment(SSID);
  m_wlist.loadFromFile(filename);
  
  m_region = m_wlist.getIntValue(REGION);
  m_standard = (WirelessStandardT)m_wlist.getIntValue(STANDARD);
  m_wepEnabled = m_wlist.getIntValue(WPKENABLED);
  m_useKeyIndex = m_wlist.getIntValue(KEYINDEX) + 1;		// 07-20-06
  WepKeyLengthT klen = (m_wlist.getIntValue(WEPKEYLEN)?WEP128:WEP64);
  m_wepKeys[0].id = 1;
  m_wepKeys[0].length = klen;
  strcpy(m_wepKeys[0].key, m_wlist.getValue(WEPKEY0));
  m_wepKeys[1].id = 1;
  m_wepKeys[1].length = klen;
  strcpy(m_wepKeys[1].key, m_wlist.getValue(WEPKEY1));
  m_wepKeys[2].id = 1;
  m_wepKeys[2].length = klen;
  strcpy(m_wepKeys[2].key, m_wlist.getValue(WEPKEY2));
  m_wepKeys[3].id = 1;
  m_wepKeys[3].length = klen;
  strcpy(m_wepKeys[3].key, m_wlist.getValue(WEPKEY3));
 
  m_authOption = (m_wlist.getIntValue(AUTHTYPE)?AUTH_SHARED:AUTH_OPEN);
  m_channel = m_wlist.getIntValue(CHANNEL);
  m_mode = (m_wlist.getIntValue(RADIOMODE)?MODE_MANAGED:MODE_MASTER);
  strcpy(m_ssid, m_wlist.getValue(SSID));

#endif

  if (initRadio) 		
    SetAll();				// made setting as function

  return true;
}

void wlan_common::SetAll()		// Added on 03-10-06
  throw (INVALID_ARGUMENT*)
{
    if (checkAvail()) {
//    	SetWirelessStandard(m_standard);
    	SetRadioMode(m_mode);
    	SetSSID(m_ssid);
    	SetChannel(m_channel);

        int enabled = m_wepEnabled;	// 0: NONE 1: WEP  2: WPA

	try {
        SetAuthOption(m_wepEnabled==1? m_authOption : AUTH_OPEN);  // WEP only
        SetWepKeys(m_wepEnabled==1, m_useKeyIndex, m_wepKeys, NUMBER_WEP_KEYS);
	}
	catch(...) {
	if (m_Log) 
		m_Log->log(LogTrace::ERROR,"it is catched on set Wep Key!\n");
#ifdef WLAN_COMMON_DEBUG
      cout << "it is catched on set Wep Key!\n";
#endif
	}
        m_wepEnabled = enabled;

    }
}

#ifdef ORIGINAL_READ

void wlan_common::ReadOneWepKeyFromString(char *line, bool* getall)
  throw (PARSE_CONFIGURATION_FILE_FAILURE *)
{
  WepKeyT key;
  char *name = "PRIV_KEY128=";
  int start = strlen(name);
  int keyLength = line[start] - '0';
  if (keyLength != 0 && keyLength != 1) {
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
  key.length = (keyLength == 0)?WEP64: WEP128;

  name = "dot11WEPDefaultKey";
  start = strstr(line, name) - line;
  if (start < 0) start = -start;
  start += strlen(name);
  int keyId = line[start] - '0';
  if (keyId < 0 || keyId >= NUMBER_WEP_KEYS) {
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
  key.id = keyId;

  // get the key itself
  start += 2; // include id and "="  
  int i, j;
  int len = (keyLength == 0)? 14 : 38;
  for(i = 0, j = 0; i < len;) {
    if (i % 3 == 2) {
      if (line[start+i] != ':') {
	throw new PARSE_CONFIGURATION_FILE_FAILURE();
      }
      i++;
    } else {
      if (line[start+i] - '0' < 0 || line[start+i] - '0' > 0xf) {
	throw new PARSE_CONFIGURATION_FILE_FAILURE();
      }
      key.key[j] = line[start+i];
      i++; j++;
    }
  }
  key.key[key.length] = '\0';
#ifdef WLAN_COMMON_DEBUG
  cout << "read key[" << key.id << "]: " << key.key << endl;
#endif
  // valid key can be copied now
  CopyWepKey(&m_wepKeys[keyId], &key); 
  getall[key.id] = true;
}

void wlan_common::ReadSSIDFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int start = strlen("SSID=");
  int end = strlen(line);
  if (end == start || end - start > SSID_LENGTH) {
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
  m_ssid[0] = '\0';
  strncpy(m_ssid, &line[start], end-start);
  m_ssid[end-start] = '\0';
}

// assume the number is not too large <= 4 bytes
// 10 digits?
int wlan_common::ReadIntegerValueFromString(char *line, char *name) 
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int start = strlen(name);
  int value = strtol(&line[start], NULL, 10);
  if (value == ERANGE || value == EINVAL) {
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
  return value;
}

void wlan_common::ReadStandardFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int standard = ReadIntegerValueFromString(line, "standard=");
  if (standard < IEEE_80211AUTO || standard > IEEE_80211G) {
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
  m_standard = (WirelessStandardT) standard;
}

void wlan_common::ReadChannelFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int channel = ReadIntegerValueFromString(line, "CHANNEL=");
  m_channel = channel;
}


void wlan_common::ReadRadioModeFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int radioMode = ReadIntegerValueFromString(line, "radioMode=");
  switch(radioMode) {
  case MODE_AUTO:
    m_mode = MODE_AUTO;
    break;
  case MODE_MASTER:
    m_mode = MODE_MASTER;
    break;
  case MODE_ADHOC:
    m_mode = MODE_ADHOC;
    break;
  case MODE_MANAGED:
    m_mode = MODE_MANAGED; 
    break;
  case MODE_MONITOR:
    m_mode = MODE_MONITOR;
    break;
  case MODE_REPEATER:
    m_mode = MODE_REPEATER;
    break;
  case MODE_SECONDARY:
    m_mode = MODE_SECONDARY;
    break;
  default:
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
  }
}


void wlan_common::ReadAuthOptionFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int authOption = ReadIntegerValueFromString(line, "AuthType=");
  switch(authOption) {
  case AUTH_OPEN:
    m_authOption = AUTH_OPEN;
    break;
  case AUTH_SHARED:
    m_authOption = AUTH_SHARED;
    break;
  default:
    throw new PARSE_CONFIGURATION_FILE_FAILURE();
    break;
  }
}


void wlan_common::ReadWepEnabledFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int wepEnabled = ReadIntegerValueFromString(line, "dot11PrivacyInvoked=");
  m_wepEnabled = (wepEnabled != 0);
}


void wlan_common::ReadUseKeyIndexFromString(char *line)
  throw (PARSE_CONFIGURATION_FILE_FAILURE*)
{
  int useKeyIndex = ReadIntegerValueFromString(line, "dot11WEPDefaultKeyID=");
  m_useKeyIndex = useKeyIndex;
}

#endif

void wlan_common::Serialize(const char *filename) 
  throw (INVALID_ARGUMENT*)
{
  assert(filename != NULL);

#ifdef ORIGINAL_READ
  ofstream out(filename);  
  if (!out.is_open()) {
    throw new INVALID_ARGUMENT();
  }
  
  out << "standard="<<m_standard<<"\n";
  out << "radioMode="<<m_mode<<"\n";
  out << "dot11PrivacyInvoked="<<m_wepEnabled<<"\n";
  out << "dot11WEPDefaultKeyID="<<m_useKeyIndex<<"\n";
  for(int i = 0; i < NUMBER_WEP_KEYS; i++) {
    out << "PRIV_KEY128="<< (m_wepKeys[i].length == WEP128) <<",";
    out << "dot11WEPDefaultKey" << m_wepKeys[i].id << "=";
    for(int j = 0; j < m_wepKeys[i].length; j++) {
      if (j != 0 && j % 2 == 0) out << ":";
      out << m_wepKeys[i].key[j];
    }
    out << "\n";
  }
  out << "AuthType="<< m_authOption << "\n";
  out << "SSID="<< m_ssid << "\n";
  out << "CHANNEL="<<m_channel<<"\n";
    
  out.close();
  system("sync; sleep 1\n"); // flush filesystem buffers

#else
  m_wlist.saveToFile((char*)filename);
#endif
}


const char *wlan_common::GetInterfaceName()
{
  return m_ifname;
}

// depends on regdomain and the standard
// we can get the list of available channels on the fly
// based on print_freq_info from iwlist.c
bool wlan_common::IsChannelValid(unsigned short channel)
  throw (CHANNEL_CONFIGURE_FAILURE*)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new CHANNEL_CONFIGURE_FAILURE();
  }
  iwrange range;  
  if(iw_get_range_info(sockfd, m_ifname, &range) < 0) {
    throw new CHANNEL_CONFIGURE_FAILURE();
  }

  int freq_range;
  freq_range = range.num_frequency;
  /* Since it is a world-widw version of card, 
   *   we limit the 11G and 11B frequency range according to the region. 
   */
  if (m_standard == IEEE_80211G) {
    if (m_region == NA) {  // NA
      freq_range = 11;
    }
  } 
  else if (m_standard == IEEE_80211B) {
    if (m_region == NA)  
      freq_range = 11;
    else if (m_region == EU)  
      freq_range = 13;
    else if (m_region == JP) 
      freq_range = 14;
  }

  for(int i = 0; i < freq_range; i++) {
    if (channel == range.freq[i].i) {
      close(sockfd);
      return true;
    }
  }
  close(sockfd);
  return false;
}


// based on iwconfig.c
void wlan_common::SetRadioMode(RadioModeT mode)
  throw (MODE_CONFIGURE_FAILURE*)
{
  struct iwreq request;
  request.u.mode = mode;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new MODE_CONFIGURE_FAILURE();
  }

  if (iw_set_ext(sockfd, m_ifname, SIOCSIWMODE, &request) < 0) {
    close(sockfd);
    throw new MODE_CONFIGURE_FAILURE();
  }
  close(sockfd);
  m_mode = mode;
}


// based on get_info from iwconfig.c
bool wlan_common::IsWirelessInterfaceEnabled(char *ifname)
{
  struct ifreq ifr;
  strncpy(ifr.ifr_name, m_ifname, IFNAMSIZ);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) return false;
  bool enabled = (ioctl(sockfd, SIOCGIFFLAGS, &ifr) >= 0);
  close(sockfd);
  return enabled;
}

// based on get_info from iwconfig.c
void wlan_common::GetLinkStatus(LinkStatusT *status)
  throw (GET_LINK_STATUS_FAILURE*)
{
  assert(status != NULL);
  iwstats stats;
  iwrange range;
  int hasRange = 0;
 
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new GET_LINK_STATUS_FAILURE();
  }
  if (iw_get_range_info(sockfd, m_ifname, &range) >= 0) {
    hasRange = 1;
  }
  if (iw_get_stats(sockfd, m_ifname, &stats, &range, hasRange) < 0) {
    close(sockfd);
    throw new GET_LINK_STATUS_FAILURE();
  }
  close(sockfd);
  status->linkQuality = stats.qual.qual;
  status->signalLevel = stats.qual.level;
}

// note that keys are stored as characters, not hex
bool wlan_common::AreWepKeysValid(WepKeyT *keys, unsigned short numberKeys)
{
  for(int i = 0; i < numberKeys; i++) {
    if ((keys[i].length != WEP64 && keys[i].length != WEP128) ||
	(keys[i].id < 0 || keys[i].id >= NUMBER_WEP_KEYS)) {
      return false;
    }

    for (int j = 0; j < keys[i].length; j++) {
      if (((keys[i].key[j] >= '0') && (keys[i].key[j] <= '9')) ||
		((keys[i].key[j] >= 'a') && (keys[i].key[j] <= 'f')) ||
		((keys[i].key[j] >= 'A') && (keys[i].key[j] <= 'F')) 
	) {
	continue;
      } else {
	return false;
      }							    
    }
  }
  return true;
}

// based on iw_in_key in iwlib.c
void wlan_common::EncodeOneWepKey(WepKeyT *wepKey, unsigned char *encodedKey, int *keyLength)
{
    char *hex, *out, *p;
 
    *keyLength = 0;
    char buff[IW_ENCODING_TOKEN_MAX+WEP_KEY_LENGTH+1];
    hex = buff + IW_ENCODING_TOKEN_MAX;
    strncpy(hex, wepKey->key, wepKey->length);
    hex[wepKey->length] = '\0';
    out = buff;
    p = strtok(hex, "-:;.,");
    while((p != (char *) NULL) && (*keyLength < IW_ENCODING_TOKEN_MAX)) {
      int temph, templ, count, len;
      
      count = sscanf(p, "%1X%1X", &temph, &templ);
      if(count < 1) return;
            
      len = strlen(p);
      if(len % 2) count = 1;
      if(count == 2) templ |= temph << 4;
      else templ = temph;
	
      out[(*keyLength)++] = (unsigned char) (templ & 0xFF);
      if(len > count) p += count;
      else p = strtok((char *) NULL, "-:;.,");
    }
    memcpy(encodedKey, out, *keyLength);
}

// iwconfig.c did not check the range of the key index => bug
void wlan_common::SetWepKeys(bool enabled, unsigned short useKeyIndex, 
			     WepKeyT *keys, unsigned short numberKeys)
  throw (INVALID_ARGUMENT*, KEY_CONFIGURE_FAILURE*)
{
  if (!enabled) {
    DisableWep();
    return;
  }
  
  if ((useKeyIndex > NUMBER_WEP_KEYS || useKeyIndex <= 0) || 
      (numberKeys > NUMBER_WEP_KEYS || numberKeys <= 0  ) || 
      (!AreWepKeysValid(keys, numberKeys)) ) {
    throw new INVALID_ARGUMENT();
  }
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new KEY_CONFIGURE_FAILURE();
  }
  struct iwreq request;
  for(int i = 0; i < numberKeys; i++) {
    unsigned char key[IW_ENCODING_TOKEN_MAX];
    int keylen = 0;
    EncodeOneWepKey(&(keys[i]), key, &keylen);
    request.u.data.length = keylen;
    request.u.data.pointer = (caddr_t) key;
    request.u.data.flags = 0;
    request.u.encoding.flags = 0;
    
    request.u.data.flags |= ((m_authOption == AUTH_OPEN)? IW_ENCODE_OPEN: IW_ENCODE_RESTRICTED);
    request.u.encoding.flags |= i+1;
    
    if (iw_set_ext(sockfd, m_ifname, SIOCSIWENCODE, &request) < 0) {
      close(sockfd);
      throw new KEY_CONFIGURE_FAILURE();
    }
    CopyWepKey(&(m_wepKeys[i]), &(keys[i]));
  }

  request.u.data.pointer = (caddr_t) NULL;
  request.u.data.length = 0;
  request.u.data.flags = 0;
  request.u.encoding.flags = 0;
  request.u.data.flags |= ((m_authOption == AUTH_OPEN)? IW_ENCODE_OPEN: IW_ENCODE_RESTRICTED);
  request.u.encoding.flags |= useKeyIndex;
  if (iw_set_ext(sockfd, m_ifname, SIOCSIWENCODE, &request) < 0) {
    close(sockfd);
    throw new KEY_CONFIGURE_FAILURE();
  }
  m_useKeyIndex = useKeyIndex;
  m_wepEnabled = enabled;
  close(sockfd);
}

void wlan_common::CopyWepKey(WepKeyT *target, WepKeyT *source)
{
  if (target == source) return;
  target->length = source->length;
  target->id = source->id;
  target->key[0] = '\0';
  strncpy(target->key, source->key, target->length);
  target->key[target->length] = '\0';
}

void wlan_common::SetAuthOption(AuthenticationOptionT authOption)
    throw (AUTH_CONFIGURE_FAILURE*)
{
  struct iwreq request;

#if 0 
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new AUTH_CONFIGURE_FAILURE();
  }
  request.u.data.pointer = (caddr_t) NULL;
  request.u.data.flags = 0;
  request.u.data.length = 0;
  request.u.data.flags |= ((authOption == AUTH_OPEN)?IW_ENCODE_OPEN:IW_ENCODE_RESTRICTED);
    
  if (iw_set_ext(sockfd, m_ifname, SIOCSIWENCODE, &request) < 0) {
    close(sockfd);
    throw new AUTH_CONFIGURE_FAILURE();
  }
  close(sockfd);
#endif

  /* Set authentication mode */
  u_char buffer[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
  int skfd = iw_sockets_open();
  if (skfd < 0) {
    throw new AUTH_CONFIGURE_FAILURE();
  }
  strncpy(request.ifr_name, m_ifname, IFNAMSIZ);
  request.u.mode = 3;   /* sub-ioctl = 3 (authmode) */
  buffer[0] = (authOption == AUTH_OPEN)? 1:2;
  memcpy(request.u.name+4, buffer, 12);
  if (ioctl(skfd, SIOCIWFIRSTPRIV, &request) < 0) {
      fprintf(stderr, "Interface doesn't accept private ioctl...\n");
      fprintf(stderr, "%s (%X): %s\n", "authmode", SIOCIWFIRSTPRIV, strerror(errno));
      close(skfd);
      throw new AUTH_CONFIGURE_FAILURE();
  }
  close(skfd);

  m_authOption = authOption;
}


// based on iwconfig.c
void wlan_common::DisableWep(void)
  throw (KEY_CONFIGURE_FAILURE*)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new KEY_CONFIGURE_FAILURE();
  }
  struct iwreq request;
  request.u.data.pointer = (caddr_t) NULL; 
  request.u.data.length = 0;
  request.u.data.flags = IW_ENCODE_DISABLED;
  if (iw_set_ext(sockfd, m_ifname, SIOCSIWENCODE, &request) < 0) {
    close(sockfd);
    throw new KEY_CONFIGURE_FAILURE();
  }
  close(sockfd);
  m_wepEnabled = false;
}


// based on iwconfig.c
// anyone knows what is the essid index?
void wlan_common::SetSSID(char *ssid, bool useAnyOrOff)
  throw (INVALID_ARGUMENT*, SSID_CONFIGURE_FAILURE*)
{
  assert(ssid != NULL);
    
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new SSID_CONFIGURE_FAILURE();
  }
  struct iwreq request;
  char essid[SSID_LENGTH+1];
  bool anyOrOff = (!strcasecmp(ssid, "off")) || (!strcasecmp(ssid, "any"));
  essid[0] = '\0';

  if(useAnyOrOff && anyOrOff) { // treat any or off differently
    if (m_mode == MODE_MASTER) {
#ifdef WLAN_COMMON_DEBUG
      cout << "it is invalid to set an access point's ssid to any\n";
#endif
      throw new INVALID_ARGUMENT();
    }
    request.u.essid.flags = 0;
  } else {                      // set ssid literally
    // strlen(ssid) > SSID_LENGTH => incompatible types
    // strlen(ssid) - SSID_LENGTH > 0 => strange error
    if (strlen(ssid) + 1> SSID_LENGTH + 1 ) { // avoid warning for comparison
      close(sockfd);
      throw new INVALID_ARGUMENT();
    }
    strcpy(essid, ssid);
    essid[strlen(ssid)] = '\0';
  } 
  request.u.essid.flags = 1;
  request.u.essid.pointer = (caddr_t) essid;
  request.u.essid.length = strlen(essid) + 1;
  
  // set new network ssid
  if (iw_set_ext(sockfd, m_ifname, SIOCSIWESSID, &request) < 0) {
    close(sockfd);
    throw new SSID_CONFIGURE_FAILURE();
  }

  close(sockfd);
  if (ssid != m_ssid)
  	strcpy(m_ssid, ssid);
}

// based on iwconfig.c
void wlan_common::SetChannel(unsigned short channel) 
  throw (CHANNEL_CONFIGURE_FAILURE*, INVALID_ARGUMENT*)
{
  if (!IsChannelValid(channel)) {
#ifdef WLAN_COMMON_DEBUG
    cout << "channel is invalid\n";
#endif
    throw new INVALID_ARGUMENT();
  }
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    throw new CHANNEL_CONFIGURE_FAILURE();
  }
  struct iwreq request;  
  iw_float2freq(channel, &(request.u.freq));
  request.u.freq.flags = IW_FREQ_FIXED;
  if (iw_set_ext(sockfd, m_ifname, SIOCSIWFREQ, &request) < 0) {
    close(sockfd);
    throw new CHANNEL_CONFIGURE_FAILURE();
  }
  close(sockfd);

  // update local copy 
  m_channel = channel;
}

bool wlan_common::IsWirelessExtensionSupported(char *ifname)
{
  assert(iw_get_kernel_we_version() >= 16);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) return false;

  struct wireless_config config;
  bool supported = (iw_get_basic_config(sockfd, ifname, &config) >= 0);
  close(sockfd);
  return supported;
}

void wlan_common::CheckConstructorArgument(char *ifname)
  throw (INTERFACE_NAME_TOO_LONG_FAILURE*, 
	 EMPTY_INTERFACE_NAME_FAILURE*, 
	 ALLOCATE_MEMORY_FAILURE*)
{
  int length = strlen(ifname);
  if (length > IFNAMSIZ) {
    throw new INTERFACE_NAME_TOO_LONG_FAILURE();
  } else if (length == 0) {
    throw new EMPTY_INTERFACE_NAME_FAILURE();
  }

//  m_ifname = (char *) malloc(length+1);
//  if (m_ifname == NULL) {
//    throw new ALLOCATE_MEMORY_FAILURE();
//  } 
//  m_ifname[0] = '\0';
  strcpy(m_ifname, ifname);
//  m_ifname[length] = '\0';
}

wlan_common::wlan_common(char *ifname, char *filename, bool initRadio)
  throw (INTERFACE_NAME_TOO_LONG_FAILURE*,  
	 EMPTY_INTERFACE_NAME_FAILURE*,
	 ALLOCATE_MEMORY_FAILURE*,
	 WIRELESS_EXTENSION_UNSUPPORTED*, 
	 WIRELESS_INTERFACE_DISABLED*,
	 RADIO_INITIALIZATION_FAILURE*)
{
  assert(ifname != NULL && filename != NULL);
  
  CheckConstructorArgument(ifname);
  
  checkAvail();

  if (!Initialize(filename, initRadio)) {
    throw new RADIO_INITIALIZATION_FAILURE();
  }
  m_Log = NULL;
}


wlan_common::~wlan_common() 
{
//  free(m_ifname);
}

bool wlan_common::checkAvail()
{
  m_avail = true;
  if (!IsWirelessExtensionSupported(m_ifname)) {
      m_avail = false;
//    throw new WIRELESS_EXTENSION_UNSUPPORTED();
  }

  if (!IsWirelessInterfaceEnabled(m_ifname)) {
        m_avail = false;
//    throw new WIRELESS_INTERFACE_DISABLED();
  }
	return m_avail;
}

void wlan_common::GetSSID(char *ssid, int length, bool fromRadio) 
  throw (BUFFER_TOO_SMALL_FAILURE*, GET_SSID_FAILURE*)
{
  assert(ssid != NULL && length >= 1);
  int ssid_len = strlen(m_ssid);
  if (ssid_len > length) throw new BUFFER_TOO_SMALL_FAILURE();
  if (!fromRadio) {
    ssid[0] = '\0';
    strncpy(ssid, m_ssid, strlen(m_ssid));
    ssid[strlen(m_ssid)] = '\0';
    return;
  }
  
  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_SSID_FAILURE();

  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_SSID_FAILURE();
  }
  
  if (!config.essid_on) {
    close(sockfd);
    throw new GET_SSID_FAILURE();
  }

  close(sockfd);  
  if (strlen(config.essid) + 1> SSID_LENGTH + 1) {
    throw new BUFFER_TOO_SMALL_FAILURE();
  } 

  ssid[0] = '\0';
  strncpy(ssid, config.essid, strlen(config.essid));
  ssid[strlen(config.essid)] = '\0';
}


void wlan_common::GetAuthOption(AuthenticationOptionT *authOption, bool fromRadio)
  throw (GET_AUTH_OPTION_FAILURE*)
{
  assert(authOption != NULL);
  if (!fromRadio) {
    *authOption = m_authOption;
    return;
  }

  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_AUTH_OPTION_FAILURE();

  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_AUTH_OPTION_FAILURE();
  }
  
  if (!config.has_key) {
    close(sockfd);
    throw new GET_AUTH_OPTION_FAILURE();
  }

  if (config.key_flags & IW_ENCODE_RESTRICTED) *authOption = AUTH_SHARED;
  else *authOption = AUTH_OPEN;
  close(sockfd);  
  m_authOption = *authOption;
}

void wlan_common::GetChannel(unsigned short *channel, bool fromRadio)
  throw (GET_CHANNEL_FAILURE*)
{
  assert(channel != NULL);
  if (!fromRadio) {
    *channel = m_channel;
    return;
  }

  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_CHANNEL_FAILURE();

  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_CHANNEL_FAILURE();
  }
  
  if (!config.has_freq) {
    close(sockfd);
    throw new GET_CHANNEL_FAILURE();
  }
  iwrange range;
  int ch;
  if (iw_get_range_info(sockfd, m_ifname, &range) < 0 || 
      ((ch = iw_freq_to_channel(config.freq, &range)) < 0)) {
    close(sockfd);
    throw new GET_CHANNEL_FAILURE();
  }
  close(sockfd);  
  *channel = ch;
  m_channel = *channel;
}

void wlan_common::GetRadioMode(RadioModeT *mode, bool fromRadio)
  throw (GET_MODE_FAILURE*)
{
  assert(mode != NULL);
  if (!fromRadio) {
    *mode = m_mode;
    return;
  }

  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_MODE_FAILURE();
  
  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_MODE_FAILURE();
  }
  
  if (!config.has_mode) {
    close(sockfd);
    throw new GET_MODE_FAILURE();
  }

  close(sockfd);
  *mode = (RadioModeT) config.mode;
}


void wlan_common::GetMacAddress(MacAddressT *mac)
  throw (GET_MAC_ADDRESS_FAILURE*)
{  
  assert(mac != NULL);

  if (m_mac == NULL) { // has not been called yet
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if (sockfd == -1) {
      throw new GET_MAC_ADDRESS_FAILURE();
    }
    struct ifreq ifbuffer;
    memset(&ifbuffer, 0, sizeof(ifbuffer));
    strncpy(ifbuffer.ifr_name, m_ifname, sizeof(ifbuffer.ifr_name));
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifbuffer) == -1 ||
	ifbuffer.ifr_hwaddr.sa_family  != ARPHRD_ETHER) {
      close(sockfd);
      throw new GET_MAC_ADDRESS_FAILURE();
    }
    m_mac = new (MacAddressT);			// MAR-06-2006 Fixed
    memcpy(m_mac->addr, ifbuffer.ifr_hwaddr.sa_data, 6);
    close(sockfd);
  }
  
  CopyMacAddress(m_mac, mac);
}

void wlan_common::CopyMacAddress(MacAddressT *source, MacAddressT *target)
{
  for(int i = 0; i < MAC_ADDRESS_LENGTH; i++) {
    target->addr[i] = source->addr[i];
  }
}


void wlan_common::ShowMacAddress(MacAddressT *mac)
{
  printf("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", 
	 mac->addr[0], mac->addr[1],
	 mac->addr[2], mac->addr[3],
	 mac->addr[4], mac->addr[5]);
}


void wlan_common::GetWepKeys(bool *enabled, unsigned short *useKeyIndex, 
			     WepKeyT *keys, unsigned short numberKeys, bool fromRadio)
  throw (INVALID_ARGUMENT*)
{
  assert(enabled && useKeyIndex && keys);

  if (numberKeys > NUMBER_WEP_KEYS) {
    throw new INVALID_ARGUMENT();
  }

  if (!fromRadio) {
    *enabled = m_wepEnabled;
    *useKeyIndex = m_useKeyIndex;
    for(int i = 0; i < numberKeys; i++) {
      keys[i].length = m_wepKeys[i].length;
      keys[i].id = m_wepKeys[i].id;
      memcpy(keys[i].key, m_wepKeys[i].key, WEP_KEY_LENGTH);
    }
    return;
  }

  assert(numberKeys == 1); // get the current key being used
  struct wireless_config config;
  memset((char *) &config, 0, sizeof(struct wireless_config));
  
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) throw new GET_KEY_FAILURE();
  
  if (iw_get_basic_config(sockfd, m_ifname, &config) < 0) {
    close(sockfd);
    throw new GET_KEY_FAILURE();
  }
  
  if (!config.has_key) {
    close(sockfd);
    throw new GET_KEY_FAILURE();
  }

  close(sockfd);

  if (config.key_flags & IW_ENCODE_DISABLED || config.key_size == 0) {
    *enabled = false;
  } else {
    if ((config.key_flags & IW_ENCODE_INDEX) > 1) {
      *useKeyIndex = (config.key_flags & IW_ENCODE_INDEX) - 1;
    }
    if (!(config.key_flags & IW_ENCODE_NOKEY)) {
	keys[0].length = WEP64;
	keys[0].id = *useKeyIndex;
	memset(keys[0].key, 0, WEP64);
    }
  }
}

