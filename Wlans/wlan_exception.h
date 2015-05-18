/******************************************************************************
 * File: wlan_exception.h
 * --------------------------------------------------------------------------
 *
 *
 * Description:
 * ---------------
 * This file defines the exceptions that might be thrown from the wlan_common
 * and wlan_advanced classes.
 *
 * The base class for all the exception classes is WLAN_EXCEPTION. Based on
 * polymorphism, you can simply catch all WLAN_EXCEPTION*, and then send
 * description message to get the specific reason causing the exception
 *
 * The exception classes are all very simply and self-describing, thus
 * no detailed documentation is necessary.
 *
 * History:
 * -------------------------------------------------------------------------
 * JP         11/01/2005          Original 
 *
 ****************************************************************************/
#ifndef _WLAN_EXCEPTION_
#define _WLAN_EXCEPTION_


class WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "";
  }
}; 

class COMPUTE_OPTIMAL_CHANNEL_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to compute optimal channel";
  }
};


class DETECT_INTERFERENCE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to detect interference";
  }
};

class GET_CHANNEL_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to get channel";
  }
};
 
class CHANNEL_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure channel";
  }
};

class GET_SSID_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to get network name or ssid";
  }
};

class SSID_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure network name or ssid";
  }
};


class AUTH_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure authentication scheme";
  }
};


class GET_AUTH_OPTION_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to get authentication scheme";
  }
};

class GET_KEY_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to obtain WEP key information";
  }
};

class KEY_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure one or more WEP keys";
  }
};
    
class STANDARD_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure 802.11 standard";
  }
};

class STANDARD_CONFIGURE_UNSUPPORTED: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "configuration of 802.11 standard is unavailable";
  }
};

class GET_STANDARD_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to obtain standard or protocol information";
  }
};


class UNKNOWN_STANDARD: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "unknown standard/protocol";
  }
};

class MODE_CONFIGURE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to configure radio mode";
  }
};

class GET_MODE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to get radio mode";
  }
};

class INVALID_ARGUMENT: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "the argument is invalid";
  }
};

class BUFFER_TOO_SMALL_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "buffer is too small. please allocate more space";
  }
};

class GET_MAC_ADDRESS_FAILURE: public WLAN_EXCEPTION {
 public: 
  virtual char *description(void) {
    return "failed to retrieve the MAC address";
  }
};

class GET_LINK_STATUS_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to get link status from the radio";
  }
};
        
class INTERFACE_NAME_TOO_LONG_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "interface name is too long";
  }
};
  
class EMPTY_INTERFACE_NAME_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "interface name is an empty string";
  }
};
  
class ALLOCATE_MEMORY_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to allocate memory from heap";
  }
};
  
class WIRELESS_EXTENSION_UNSUPPORTED: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "wireless extension is unsupported";
  }
};

class WIRELESS_INTERFACE_DISABLED: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "wireless interface is disabled";
  }
};

class RADIO_INITIALIZATION_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to initialize the radio";
  }
};

class PARSE_CONFIGURATION_FILE_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to parse the configuration file";
  }
};

class SCAN_NETWORK_FAILURE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "failed to initiate scanning of the wireless networks";
  }
};


class SCAN_NETWORK_UNSUPPORTED: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "the radio does not support scanning";
  }
};
  
  
class CLEAN_CHANNEL_UNAVAILABLE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "all channels are being used";
  }
};

class SSID_UNAVAILABLE: public WLAN_EXCEPTION {
 public:
  virtual char *description(void) {
    return "all SSID's are being used";
  }
};

#endif // _WLAN_EXCEPTION_
