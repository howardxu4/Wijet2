#include "wlan_advanced.h"

int wlan_advanced::ComputeOptimalChannel(NetworkScanResultT *list)
  throw (CLEAN_CHANNEL_UNAVAILABLE*, COMPUTE_OPTIMAL_CHANNEL_FAILURE*)
{
  WirelessStandardT standard;
  try {
    m_driver->GetWirelessStandard(&standard, true);
  } catch(WLAN_EXCEPTION* e) {
    throw new COMPUTE_OPTIMAL_CHANNEL_FAILURE();
  }
  
  const int *channels;
  int numberChannels;
  int spacing;
  switch(standard) {
  case IEEE_80211B:
  case IEEE_80211G:
    channels = IEEE_80211G_CHANNELS;
    numberChannels = sizeof(IEEE_80211G_CHANNELS)/sizeof(IEEE_80211G_CHANNELS[0]);
    spacing = IEEE_80211G_INTERFERENCE_FREE_SPACING;
    break;
  case IEEE_80211A:
    channels = IEEE_80211A_CHANNELS;
    numberChannels = sizeof(IEEE_80211A_CHANNELS)/sizeof(IEEE_80211A_CHANNELS[0]);
    spacing = IEEE_80211A_INTERFERENCE_FREE_SPACING;
    break;    
  case IEEE_80211AUTO:
  default:
    channels = IEEE_80211_CHANNELS;
    numberChannels = sizeof(IEEE_80211_CHANNELS)/sizeof(IEEE_80211_CHANNELS[0]);
    spacing = IEEE_80211_INTERFERENCE_FREE_SPACING;
    break;
  }
  return ComputeOptimalChannel(list, channels, numberChannels, spacing);
}

int wlan_advanced::ComputeOptimalChannel(NetworkScanResultT *list, const int *channels, 
					 int numberChannels, int spacing)
  throw (COMPUTE_OPTIMAL_CHANNEL_FAILURE*,CLEAN_CHANNEL_UNAVAILABLE*)
{
  int skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1) {
    throw new COMPUTE_OPTIMAL_CHANNEL_FAILURE();
  }
  iwrange range;
  const char *ifname = m_driver->GetInterfaceName();
  if (!IsScanningSupported(skfd, ifname, &range)) {
    close(skfd);
    throw new COMPUTE_OPTIMAL_CHANNEL_FAILURE();
  }

  // key of the algorithm:
  // maximize the min distance among all the channels
  NetworkScanResultT *next = list;
  BestChannelT best;
  best.channel = -1;
  best.minDistance = 0;
  for(int i = 0; i < numberChannels; i++) {
    if (!m_driver->IsChannelValid(channels[i])) continue;
    else if (list == NULL) return channels[i];
    BestChannelT chann;
    chann.channel = -1;
    chann.minDistance = IEEE_80211_INTERFERENCE_FREE_MAX_SPACING;
    next = list;
    while(next) {
      double freq;
      if (iw_channel_to_freq(channels[i], &freq, &range) < 0) {
	close(skfd);
	throw new COMPUTE_OPTIMAL_CHANNEL_FAILURE();    
      }
      freq /= MHZ;
      if ((fabs(next->frequency - freq) + 0.5) < chann.minDistance) {
	chann.channel = channels[i];
	chann.minDistance = fabs(next->frequency - freq) + 0.5;
      }
      next = next->next;
    } // end of while(next)

    if ((chann.minDistance + 1.0) > (IEEE_80211_INTERFERENCE_FREE_SPACING + 1.0)) {
      return chann.channel;
    } else {
      if (chann.minDistance > best.minDistance) {
	best.channel = chann.channel;
	best.minDistance = chann.minDistance;
      }
    }
  } // end of for(int i...)
  return best.channel;
}


bool wlan_advanced::InterferenceIsDetected(NetworkScanResultT *list, int channel, int spacing) 
  throw (DETECT_INTERFERENCE_FAILURE*)
{
  if (list == NULL) return false;

  int skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1) {
    throw new DETECT_INTERFERENCE_FAILURE();
  }
  
  iwrange range;
  const char *ifname = m_driver->GetInterfaceName();
  if (!IsScanningSupported(skfd, ifname, &range)) {
    close(skfd);
    throw new DETECT_INTERFERENCE_FAILURE();
  }
  
  double freq;
  if (iw_channel_to_freq(channel, &freq, &range) < 0) {
    close(skfd);
    throw new DETECT_INTERFERENCE_FAILURE();    
  }
  freq /= MHZ;
#ifdef WLAN_ADVANCED_DEBUG
  cout << __FUNCTION__ << ": f(channel=" << channel << ") = " << freq << "MHz" << endl;
#endif
  close(skfd);
  NetworkScanResultT *next = list;
  while(next) {
    if (fabs(next->frequency - freq) + 0.5 < spacing) {
      return true;
    }
    next = next->next;
  }
  return false;
}


unsigned short wlan_advanced::ComputeNextAvailableSSID(char *prefix, unsigned short min, unsigned short max, NetworkScanResultT* list) 
  throw (SSID_UNAVAILABLE*, ALLOCATE_MEMORY_FAILURE*, INVALID_ARGUMENT*)
{
// Change return 0 for No one there.  03-10-06
  int len = strlen(prefix);
  if (min > max) throw new INVALID_ARGUMENT();
  if (len > SSID_LENGTH) throw new INVALID_ARGUMENT();
  if (list == NULL) return 0;
  
  NetworkScanResultT item;
  item.channel = 1; // anything should do
  strcpy(item.ssid, prefix);
  if (!FindScanItemInList(&item, list))
      return 0;
  int i, k = 1;
  for(i = max; i > 0; k++) i /= 10;
  if (len > SSID_LENGTH-k) throw new INVALID_ARGUMENT();
  for(i = min; i <= max; i++) {
    char *suffix = itoa(i);
//  cout << __FUNCTION__ << ": " << suffix << endl;
    strcpy(item.ssid, prefix);
    strncat(item.ssid, suffix, strlen(suffix));
    if (!FindScanItemInList(&item, list)) {
      free(suffix);
      return i;
    } 
    free(suffix);
  }
  throw new SSID_UNAVAILABLE();
}


// since number is unsigned short, thus
// it at most has 5 digits (0-65535)
char *wlan_advanced::itoa(unsigned short number)
  throw (ALLOCATE_MEMORY_FAILURE*)
{
  // why could not I declare base as "unsigned short" below?
  // unsigned short base = 1000000;
  int base = 1000000;
  char *numStr = NULL;
  int i = 0;
  int j = 0;

  if (number == 0) {
    numStr = (char *) malloc(2);
    numStr[0] = '0';
    numStr[1] = '\0';
    return numStr;
  }

  while(base != 0) {
#ifdef WLAN_ADVANCED_DEBUG
    cout << __FUNCTION__ << ": i = " << i << ", j = " << j 
	 << ", base = " << base << ", number = " << number << endl;
#endif
    if (number/base != 0) {
      if (numStr == NULL) {
	if ((numStr = (char *) malloc(7-i)) == NULL) {
	  throw new ALLOCATE_MEMORY_FAILURE();
	}
      }
    }
    
    if (numStr != NULL) {
      numStr[j] = '0' + number/base;
      j++;
    }
    number -= (number/base) * base;
    base /= 10;
    i++;
  }
  
  if (numStr == NULL) {
    numStr[j] = '0';
    j++;
  }

  numStr[j] = '\0';
#ifdef WLAN_ADVANCED_DEBUG
  cout << __FUNCTION__ << ": return " << numStr << endl;
#endif
  return numStr;
}

// based on print_scanning_token from iwlist.c
// removed all other command hanlders, except those needed
// inserted the scanning results to our linked list of NetworkScanResultT
// TODO: do not include itself!
int wlan_advanced::GetOneScanningToken(struct iw_event *event,
				       int ap_num,
				       struct iw_range *iw_range, 
				       NetworkScanResultT *scan)
{
  switch(event->cmd) {
  case SIOCGIWAP:
#ifdef WLAN_ADVANCED_DEBUG
    cout << "cell: " << ap_num << endl;
#endif
    ap_num++;
    break;
  case SIOCGIWFREQ:
    {
      double frequency = iw_freq2float(&(event->u.freq));
      int channel = iw_freq_to_channel(frequency, iw_range);
#ifdef WLAN_ADVANCED_DEBUG
      cout << __FUNCTION__ << ": channel=" << channel 
	   << ", frequency=" << frequency/MHZ << "MHz" << endl;
#endif
      scan->frequency = frequency/MHZ;
      scan->channel = channel;
    }
    break;
  case SIOCGIWESSID:
    {
      char essid[IW_ESSID_MAX_SIZE+1];
      if((event->u.essid.pointer) && (event->u.essid.length))
	memcpy(essid, event->u.essid.pointer, event->u.essid.length);
      essid[event->u.essid.length] = '\0';
#ifdef WLAN_ADVANCED_DEBUG
      cout << "ESSID: " << essid << endl;
#endif
      memcpy(scan->ssid, essid, event->u.essid.length);
    }
    break;
  default:
    break;
  }
  
  return(ap_num);
}


// based on print_scanning_info from iwlist.c
// but changed the "goto" statement to while loop
bool wlan_advanced::InitiateScanning(int skfd, const char *ifname, 
				     iwrange *range, 
				     iwreq *wrq)
{
  char *buffer = NULL;
  int buflen = IW_SCAN_MAX_DATA;
  bool isBufferTooSmall = true;
  int timeout = SCANNING_TIMEOUT;
  struct timeval tv;

  /* Initiate Scanning */
  if (iw_set_ext(skfd, ifname, SIOCSIWSCAN, wrq) < 0) {
    if (errno != EPERM) return false;
  }
  int count = 50;
//  while(true) {
  while(count--) {
    fd_set rfds;
    int	last_fd, ret;
    FD_ZERO(&rfds);
    last_fd = skfd;  
    tv.tv_sec = 0;
    tv.tv_usec = SCANNING_INTERVAL; 
  
    if ((ret = select(last_fd + 1, &rfds, NULL, NULL, &tv)) < 0) {
      if(errno == EAGAIN || errno == EINTR) continue;
      return false;       // unhandled signal
    } else if (ret > 0) { // get scan results
      break;
    } else {              // ret == 0, timeout
      char *newbuf;
      realloc:
      //if (isBufferTooSmall) {
	newbuf = (char *) realloc(buffer, buflen);
	if(newbuf == NULL) {
	  if(buffer != NULL) free(buffer);
	  return false; 
	}
	buffer = newbuf;
      //}
      /* Try to read the results */
      wrq->u.data.pointer = buffer;
      wrq->u.data.flags = 0;
      wrq->u.data.length = buflen;
      isBufferTooSmall = false;
      if(iw_get_ext(skfd, ifname, SIOCGIWSCAN, wrq) >= 0) {
	break; // result available, go process them
      } else {
	if((errno == E2BIG) && (range->we_version_compiled > 16)) {
	  if(wrq->u.data.length > buflen) buflen = wrq->u.data.length;
	  else buflen *= 2;
	  //isBufferTooSmall = true;
	  //continue;
          goto realloc;
	}
	
	if(errno == EAGAIN) { // check if results not available yet
	  timeout -= tv.tv_usec;
	  if(timeout > 0) continue;	
	}
	
	// bad error
	free(buffer);
	return false; 
      }
    }
  } // end of while(true)
  
// printf("count = %d\n", count);
  // we have got the scanning results here
  if (count >= 0)
     UpdateScanningResults(skfd, ifname, range, wrq, buffer);
  if (buffer != NULL) free(buffer);
  return true;
}

void wlan_advanced::UpdateScanningResults(int skfd, const char *ifname, 
					  iwrange *range,
					  iwreq *wrq, char *buffer)
{
  if(wrq->u.data.length) {
    struct iw_event iwe;
    struct stream_descr	stream;
    int	ap_num = 0;
    iw_init_event_stream(&stream, buffer, wrq->u.data.length);
    FreeNetworkScanResult(m_scan);
    NetworkScanResultT *scan = NULL;
    if (scan == NULL) {
      scan = (NetworkScanResultT *) malloc(sizeof(NetworkScanResultT));
      scan->next = NULL;
      m_scan = scan;
    }
    while(iw_extract_event_stream(&stream, &iwe,
				  range->we_version_compiled) > 0) {
      if (ap_num != GetOneScanningToken(&iwe, ap_num, range, scan)) {
	ap_num++;
	if (ap_num != 1) {
	  scan->next = (NetworkScanResultT *) malloc(sizeof(NetworkScanResultT));
	  scan->next->next = NULL;
	  scan = scan->next;
	}
      }
    } 
  }
}

// based on print_scanning_token and
// print_scanning_info from iwlist.c
NetworkScanResultT* wlan_advanced::ScanForExistingWirelessNetworks(int times)
  throw (SCAN_NETWORK_FAILURE*)
{
  assert(times >= 1);

  // scanning has to be performed in MODE_MANAGED mode
  RadioModeT mode;
  m_driver->GetRadioMode(&mode);
  if (mode != MODE_MANAGED) {
    throw new SCAN_NETWORK_FAILURE();
  }
  
  int skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1) {
    throw new SCAN_NETWORK_FAILURE();
  }

  iwrange range;
  const char *ifname = m_driver->GetInterfaceName();
  if (!IsScanningSupported(skfd, ifname, &range)) {
    close(skfd);
    throw new SCAN_NETWORK_UNSUPPORTED();
  }

  NetworkScanResultT *scan = NULL;
  NetworkScanResultT *last = NULL;
  while(times--) {
    iwreq request;
    request.u.data.pointer = NULL;
    request.u.data.flags = 0;
    request.u.data.length = 0;
    if (!InitiateScanning(skfd, ifname, &range, &request)) {
      close(skfd);
      throw new SCAN_NETWORK_FAILURE();
    }
    NetworkScanResultT *next = m_scan;
    while(next) {
      if (!FindScanItemInList(next, scan)) {
//	cout << "find new item\n";
	if (scan == NULL) {
	  scan = (NetworkScanResultT *) malloc(sizeof(NetworkScanResultT));
	  last = scan;
	} else if (last->next == NULL) {
	  last->next = (NetworkScanResultT *) malloc(sizeof(NetworkScanResultT));
	  last = last->next;
	}
	CopyOneScanItem(next, last);
      }
      next = next->next;
    }
  }
  close(skfd);
  return scan;
}

bool wlan_advanced::FindScanItemInList(NetworkScanResultT *item, NetworkScanResultT *list) 
{
  if (list == NULL) return false;
  assert(item != NULL);

  NetworkScanResultT *next = list;
  while(next) {
#ifdef WLAN_ADVANCED_DEBUG
    cout << __FUNCTION__ << ": " << item->ssid << ", " << next->ssid << endl; 
#endif
    if (strcmp(item->ssid, next->ssid) == 0) return true;
    next = next->next;
  }
  return false;
}

void wlan_advanced::CopyOneScanItem(NetworkScanResultT *source, NetworkScanResultT *target) 
{
  target->ssid[0] = '\0';
  memcpy(target->ssid, source->ssid, strlen(source->ssid));
  target->ssid[strlen(source->ssid)] = '\0';
  target->channel = source->channel;
  target->frequency = source->frequency;
  target->next = NULL;
}

// based on print_scanning_info from iwlist.c
bool wlan_advanced::IsScanningSupported(int skfd, const char *ifname, 
					iwrange *range) 
{
  if((iw_get_range_info(skfd, ifname, range) < 0) || 
     (range->we_version_compiled < 14)) {
    return false;
  }
  return true;
}

const wlan_common *wlan_advanced::GetDriver(void)
{
  return m_driver;
}

wlan_advanced::wlan_advanced(wlan_common *driver)
{
  assert(driver != NULL);
  m_driver = driver;
  m_scan = NULL;
}


wlan_advanced::~wlan_advanced()
{
  FreeNetworkScanResult(m_scan);
}

void wlan_advanced::FreeNetworkScanResult(NetworkScanResultT *list) {
  NetworkScanResultT *next = list;
  while(next != NULL) {
    next = next->next;
    free(list);
    list = next;
  }
}

void wlan_advanced::DisplayScanResult(NetworkScanResultT *list) 
{
  if (list != NULL) {
    cout << "wireless network scanning result in (ssid,channel) pairs:\n";
    cout << "---------------------------------------------------------\n";
    NetworkScanResultT *next = list;
    while(next != NULL) {
      cout << "(" << next->ssid << ", " << next->channel << ")" << endl;
      next = next->next;
    }
  } else {
    cout << "No wireless networks detected\n";
  }
}

