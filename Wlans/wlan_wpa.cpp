
#include "myWlan.h"
/**
 *      The methods of WatchDog class 
 *
 *  	@author     Howard Xu
 *	@version        2.5
 */


#define WPA_AP_CONF             "/etc/wpa_ap.conf"
#define WPA_WS_CONF             "/etc/wpa_ws.conf"
#define HOSTAPD_CONF		"/etc/hostapd.conf"
#define SUPPLICANT_CONF		"/etc/supplicant.conf"

#define WPKENABLED      "dot11PrivacyInvoked"
#define SSID		"SSID"

#define WPAMODE		"WPAMODE"
#define CIPHERTYPE	"CIPHERTYPE"
#define PSK		"PSK"

#define AUTHTIME	"AUTHTIME"
#define GKTIME		"GKTIME"
#define RADIUSIP	"RADIUSIP"
#define RADIUSPORT	"RADIUSPORT"
#define SHAREDSECRET	"SHAREDSECRET"
#define RADIUSIP2        "R2IP"
#define RADIUSPORT2      "R2PORT"
#define SHAREDSECRET2    "SS2"

#define EAP		"EAP"
#define ID		"ID"
#define PASSWORD	"PSWD"
#define PRIORITY	"PRIORITY"
#define CACERT		"CACERT"
#define CLCERT		"CLCERT"
#define PKCERT		"PKCERT"
#define PKPSWD		"PKPSWD"
#define PHASE2		"PHASE2"

#define CERTFORMAT	"\"/banners/certs/%s\""

char* WpaMode[] = {
	"1",
	"2",
	"3",
	NULL
};
char* Proto[] = {
	"WPA",
	"RSN",
	"WPA RSN",
	NULL
};
char* CipherType[] = {
	"TKIP",
	"CCMP",
	"TKIP CCMP",
	NULL
};
char* EapType[] = {
	"PEAP",
	"TLS",
	"TTLS",
	NULL
};

char buffer[100];
char* getFullpath(char*p)
{
	sprintf(buffer, CERTFORMAT, p);
	return (buffer);
}

char* addQuote(char*p)
{
	sprintf(buffer, "\"%s\"", p);
	return (buffer);
}

int myWlan::wpaSetup()
{
	getProperty gP(gP.P_WLAN);
	
	if (gP.getInt(WPKENABLED) == 2) {	// WPA
		int i;
		char *p;
		char cmd[60];
		LdProp alist;
        	LdConf clist;
       		bool isAP = gP.getInt(RADIOMODE) == 0;
		alist.loadFromFile((char*)(isAP? WPA_AP_CONF: WPA_WS_CONF));
		bool isEAP = strlen(alist.getValue(PSK)) < 8;
		if (isAP) {
			// Fixed part
			clist.setLine("# HostAPd configuration");
		        clist.setLine("interface=ath0");
			clist.setLine("bridge=br0");		// Fixed 7-20-06
        		clist.setLine("driver=madwifi");
        		clist.setLine("logger_syslog=-1");
        		clist.setLine("logger_syslog_level=2");
        		clist.setLine("logger_stdout=-1");
        		clist.setLine("logger_stdout_level=2");
        		clist.setLine("debug=0");
        		clist.setLine("dump_file=/tmp/hostapd.dump");
			// SSID
        		// clist.setValue("ssid", addQuote(gP.getStr(SSID)));
			clist.setValue("ssid", gP.getStr(SSID));
			if (isEAP) {
        		clist.setLine("ieee8021x=1");
        		clist.setValue("eap_reauth_period", alist.getValue(AUTHTIME));
        		clist.setLine("own_ip_addr=127.0.0.1");
        		clist.setLine("nas_identifier=127.0.0.1");
			// RADIUS server
       			clist.setValue("auth_server_addr", alist.getValue(RADIUSIP));
        		clist.setValue("auth_server_port", alist.getValue(RADIUSPORT));
        		clist.setValue("auth_server_shared_secret", alist.getValue(SHAREDSECRET));
			// Secondary RADIUS server
			p = alist.getValue(RADIUSIP2);
			if (strlen(p) > 7) {		// 0.0.0.0
				sprintf(cmd, "auth_server_addr=%s", p);
				clist.setLine(cmd);
				sprintf(cmd, "auth_server_port=%s", alist.getValue(RADIUSPORT2));
                        	clist.setLine(cmd);
				sprintf(cmd, "auth_server_shared_secret=%s", alist.getValue(SHAREDSECRET2));
                        	clist.setLine(cmd);
			}
			}
			else {
				
			}
			i = alist.getIntValue(WPAMODE);
			if (i > 2) i = 2;
			clist.setValue("wpa", WpaMode[i]);

			if (isEAP)
        			clist.setLine("wpa_key_mgmt=WPA-EAP");
			else {
				if (strlen(alist.getValue(PSK)) == 64)
				clist.setValue("wpa_psk", alist.getValue(PSK));
				else
				clist.setValue("wpa_passphrase", alist.getValue(PSK));
                        	clist.setLine("wpa_key_mgmt=WPA-PSK");
			}
			i = alist.getIntValue(CIPHERTYPE);
			if (i > 2) i = 2;
        		clist.setValue("wpa_pairwise", CipherType[i]);
        		clist.setValue("wpa_group_rekey",alist.getValue(GKTIME));
        		clist.setLine("wpa_gmk_rekey=86400");

			clist.saveToFile(HOSTAPD_CONF);
			
			system("pkill hostapd");
			sprintf(cmd, "hostapd -B %s", HOSTAPD_CONF);
			system(cmd);

			return 0;
		}
		else {
			// Fixed part
			clist.setLine("# Supplicant Configuration");
			clist.setLine("ctrl_interface=/var/run/wpa_supplicant");
			clist.setLine("ctrl_interface_group=0");
			clist.setLine("eapol_version=1");
			clist.setLine("ap_scan=1");
			clist.setLine("fast_reauth=1");
			clist.setLine("network={");
			// Common part
			clist.setValue("    ssid", addQuote(gP.getStr(SSID)));
			//clist.setValue("    ssid", gP.getStr(SSID));
			i = alist.getIntValue(WPAMODE);
                        if (i > 2) i = 2; 
			clist.setValue("    proto", Proto[i]); 
			clist.setValue("    priority", alist.getValue(PRIORITY));
			i = alist.getIntValue(CIPHERTYPE);
                        if (i > 2) i = 2;
                        clist.setValue("    pairwise", CipherType[i]);
			clist.setValue("    group", CipherType[i]);

			if (isEAP) {
			clist.setLine ("    key_mgmt=WPA-EAP");
			i = alist.getIntValue(EAP);
			if (i > 2) i = 2;
			clist.setValue("    eap", EapType[i]);
			p = alist.getValue(ID);
			if (strlen(p) > 1) clist.setValue("    identity", addQuote(p));
			p = alist.getValue(PASSWORD);
			if (strlen(p) > 1) clist.setValue("    password", addQuote(p));
			p = alist.getValue(CACERT);
			if (strlen(p) > 1) clist.setValue("    ca_cert", getFullpath(p));
			if (i != 2) { 	// NO TTLS
			p = alist.getValue(CLCERT);
			if (strlen(p) > 1) clist.setValue("    client_cert", getFullpath(p));
			p = alist.getValue(PKCERT);
			if (strlen(p) > 1) clist.setValue("    private_key", getFullpath(p));
			p = alist.getValue(PKPSWD);
			if (strlen(p) > 1) clist.setValue("    private_key_passwd", addQuote(p));
			}
			if (i != 1) {	// NO TLS
				if (i == 0)	//PEAP 
				clist.setLine("    phase1=\"peaplabel=0\"");
				i = alist.getIntValue(PHASE2);
				if (i == 1)
                                clist.setLine("    phase2=\"auth=MSCHAPV2\"");
				else if (i == 2)
				clist.setLine("    phase2=\"auth=TLS\"");
				else if (i == 3)
				clist.setLine("    phase2=\"auth=MD5\"");
			}

			}
			else {
				p = alist.getValue(PSK);
				if (strlen(p) == 64)
					clist.setValue("    psk", p);
				else 
					clist.setValue("    psk",  addQuote(p));
				clist.setLine ("    key_mgmt=WPA-PSK");
			}
			clist.setLine("}");
			clist.saveToFile(SUPPLICANT_CONF);
			
			system("pkill wpa_supplicant");
			sprintf(cmd, "wpa_supplicant -c%s -Dmadwifi -iath0 &",SUPPLICANT_CONF);
			system(cmd);

			return 0;
		}
	}
	return -1;
}

