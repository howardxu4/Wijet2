#include "ConfigCommon.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
LdProp	wlist;
LdProp  clist;		// certificate list
LdProp  alist;		// wpa_pa | wpa_ws list

#define RADIOMODE       "radioMode"
#define SECUOPTION      "dot11PrivacyInvoked"

#define WPKENABLED	"dot11PrivacyInvoked"
#define AUTHTYPE	"AuthType"
#define WEPKEYLEN	"PRIV_KEY128"
#define WEPKEY0		"dot11WEPDefaultKey0"
#define WEPKEY1         "dot11WEPDefaultKey1"
#define WEPKEY2         "dot11WEPDefaultKey2"
#define WEPKEY3         "dot11WEPDefaultKey3"
#define KEYINDEX	"dot11WEPDefaultKeyID"

#define	WEP_KEY_LEN		29 

#define WPAMODE		"WPAMODE"
#define CIPHERTYPE	"CIPHERTYPE"
#define PSK		"PSK"

#define GKTIME		"GKTIME"
#define AUTHTIME	"AUTHTIME"
#define RADIUSIP	"RADIUSIP"
#define RADIUSPORT	"RADIUSPORT"
#define SHAREDSECRIT	"SHAREDSECRIT"
#define RADIUSIP2	"R2IP"
#define RADIUSPORT2	"R2PORT"
#define SHAREDSECRIT2	"SS2"

#define PRIORITY	"PRIORITY"
#define EAP		"EAP"
#define ID		"ID"
#define PSWD		"PSWD"
#define CACERT		"CACERT"
#define CLCERT		"CLCERT"
#define PKCERT		"PKCERT"
#define PKPSWD		"PKPSWD"
#define PHASE2		"PHASE2"

#define CERTFORMAT      "/banners/certs/%s"

char* aSecuOptions[] = {
                "None",
                "WEP",
                "WPA",
                NULL
};

char* aAuthTypes[] = {
		"Open system",
		"Shared key",
		NULL
};
char* aWepKlens[] = {
		"64 bits",
		"128 bits",
		NULL
};
char* aTransKeys[] = {
		"Key 1",
		"Key 2",
		"Key 3",
		"Key 4",
		NULL
};		

char* aWpaModes[] = {
		"WPA",
		"WPA2",
		"WPA and WPA2",
		NULL
};

char* aCipherTypes[] = {
		"TKIP",
		"AES",
		"TKIP and AES",
		NULL
};

char* aEAPs[] = {
		"PEAP",
		"TLS",
		"TTLS",
		NULL
};

char* aPhase2s[] = { " ",
                "EAP-MSCHAPV2",
                "EAP-TLS/Smart card",
		"MD5-Challenge",
                NULL
};

char* aTypes[] = {
                "CA-certificate",
                "Client-cert",
                "Private-Key-cert",
                NULL
};

char* aCerts[40];

int getCert(int type)
{
	int k = 1;		// atleast have one default space
	aCerts[0] = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	for(int i=0; i < clist.getCount(); i++) 
		if(clist.getIntValue(i) == type)
			aCerts[k++] = clist.getName(i);
	aCerts[k] = NULL;
	return  k;		
}

int getSelOpt(char** a, int n, char* s)
{
	if (s != NULL && *s != 0)
	for(int i=0; i<n; i++)
		if (a[i] != NULL)
			if (strcmp(s, a[i])==0)
				return i;
	return 0;
}

char* getWEPKey(char* name)
{
static 	char p[28];
	char* t = wlist.getValue(name);
	int i, j=0;
	for(i=0; i<26 && t[i]; i++)
		if(t[i] != ':')
			p[j++] = t[i];
	p[j] = 0;
	return p; 
}

char* getWEPKetTag(int n, char* name, char* enabled, int len)
{
static char ptag[200];
	sprintf(ptag, "<input type=text name=wepKey%d maxlength=%d size=28 value=%s %s>", n, len?26:10, getWEPKey(name), enabled);
	return ptag;
}  

void showPage()
{	
	int n = wlist.getIntValue(SECUOPTION);
        char *pChecked = n? (char*)"checked": (char*)" ";
        char *pEnabled = n? (char*)" ": (char*)"disabled";

			// HEADER
        cfg.putHeader("Pegasus Wireless CYNALYNX -  Security Configuration");
        cfg.putScript("config.js");
        cfg.startPage("Security Options","The settings on this page controls the wireless security policies enforced.");
        cfg.putTag("<form name=wlan_secu action=wlan_secu.cgi method=post enctype=multipart/form-data>");

	switch(n) {
	case 0:		// None
	{
        cfg.putSubtitle("Wireless Security settings");
        cfg.skipRow();
        cfg.actionRow("Security option", cfg.getSelect("wepEnabled", 3, aSecuOptions, wlist.getIntValue(WPKENABLED)));
        cfg.dispRow("&nbsp;", "Select the wireless encryption policy that will be enforced <br>(WEP/WPA/WPA2)");
        cfg.skipRow();
	}
	break;
	case 1: 	// WEP
	{
	int keylen = wlist.getIntValue(WEPKEYLEN);
        char pSelect[40];
        sprintf(pSelect, "%s onChange='KeyLengthHandler();'", pEnabled);

	cfg.putSubtitle("Wep Configuration settings");
        cfg.skipRow();
	cfg.actionRow("WEP encryption enabled", cfg.getFValue( "<input type=checkbox name=wepEnabled %s onClick='wepHandler();'>", pChecked));
	cfg.dispRow("&nbsp;", "Enable or disable WEP feature for data encryption.");
	cfg.skipRow();
	cfg.actionRow("Authentication type", cfg.getSelect("authOption", 2, aAuthTypes, wlist.getIntValue(AUTHTYPE), pEnabled));
	cfg.dispRow("&nbsp;", "When &quot;Open system&quot; is chosen, no authentication is performed. When &quot;Shared key&quot; is chosen, the linked radio must also use &quot;Shared key&quot; to allow for proper authentication.");
        cfg.skipRow();
	cfg.actionRow("WEP key length", cfg.getSelect("wepKeyLen", 2, aWepKlens, keylen, pSelect)); 
	cfg.dispRow("&nbsp;", "Enter 10 Hex digits (0-9,a-f) for 64-bit encryption or 26 Hex digits for 128-bit encryption.");
	cfg.skipRow();
	cfg.actionRow("WEP key 1", getWEPKetTag(1, WEPKEY0, pEnabled, keylen));
        cfg.actionRow("WEP key 2", getWEPKetTag(2, WEPKEY1, pEnabled, keylen));
        cfg.actionRow("WEP key 3", getWEPKetTag(3, WEPKEY2, pEnabled, keylen));
        cfg.actionRow("WEP key 4", getWEPKetTag(4, WEPKEY3, pEnabled, keylen));

        cfg.actionRow("Transmission key select", cfg.getSelect("useKey", 4, aTransKeys, wlist.getIntValue(KEYINDEX), pEnabled)); 
	cfg.dispRow("&nbsp;", "The radio can decrypt the received data that is encrypted in any one of four default transmission keys. Select desired key for transmitting data."); 
        cfg.skipRow();
	}
	break;
	default:	// WPA
	{	
	bool isAP = wlist.getIntValue(RADIOMODE) == 0;

	alist.loadFromFile((char*)(isAP? WPA_AP_CONF: WPA_WS_CONF));

        cfg.putSubtitle("WPA Configuration settings");
        cfg.skipRow();
	
	if (isAP)
	cfg.actionRow("WPA enabled", cfg.getFValue( "<input type=checkbox name=wepEnabled %s onClick='wpaHandler(0);'>", pChecked));
	else
	cfg.actionRow("WPA enabled", cfg.getFValue( "<input type=checkbox name=wepEnabled %s onClick='wpaHandler(1);'>", pChecked));

	cfg.dispRow("&nbsp;", "Enable WPA Authenticator to require stations to use high grade encryption and authentication.");
	cfg.skipRow();

	cfg.actionRow("WPA Mode", cfg.getSelect("wpamode", 3, aWpaModes, alist.getIntValue(WPAMODE), pEnabled)); 
	cfg.dispRow("&nbsp;", "Select the WPA Mode.");
	cfg.skipRow();

	cfg.actionRow("Cipher Type", cfg.getSelect("ciphermode", 3, aCipherTypes, alist.getIntValue(CIPHERTYPE), pEnabled));
	cfg.dispRow("&nbsp;", "Select the cipher type.");
	cfg.skipRow();

	if (isAP) {        // AP

	cfg.actionRow("PSK", cfg.getFValue("<input type=text name=psk size=40 maxlength=64 value='%s' onchange='pskChanged(0);'>", alist.getValue(PSK)));
	cfg.dispRow("&nbsp;", "Enter a text pass phrase between 8 and 63 characters or a WPA pre-shared keys in hex format (64 hex digits). Leave blank to enable 802.1X Authentication.");
	cfg.skipRow();

        cfg.actionRow("WPA Group Key Update Interval", cfg.getFValue("<input type=text name=wpagkey size=10 value='%s'>", alist.getValue(GKTIME)));
	cfg.dispRow("&nbsp;","seconds.");
	cfg.skipRow();

	cfg.putSubtitle("802.1X configuration");
	cfg.skipRow();

	cfg.actionRow("Authentication timeout ", cfg.getFValue("<input type=text name=authTimeout size=10 value='%s'>", alist.getValue(AUTHTIME)));
	cfg.dispRow("&nbsp;","seconds.");

	cfg.dispRow("RADIUS Server", "&nbsp;");
	cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; IP address", cfg.getFValue("<input type=text name=radiusIP size=20 maxlength=15 value='%s'>", alist.getValue(RADIUSIP)));
	cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; Port number", cfg.getFValue("<input type=text name=radiusPort size=10 value='%s'>", alist.getValue(RADIUSPORT)));
	cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; Shared secret", cfg.getFValue("<input type=text name=radiusSecret size=20 maxlength=64 value='%s'>", alist.getValue(SHAREDSECRIT)));
	cfg.dispRow("Secondary RADIUS Server", "&nbsp;");
	cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; IP address", cfg.getFValue("<input type=text name=sradiusIP size=20 maxlength=15 value='%s'>", alist.getValue(RADIUSIP2)));
        cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; Port number", cfg.getFValue("<input type=text name=sradiusPort size=10 value='%s'>", alist.getValue(RADIUSPORT2)));
        cfg.actionRow("&nbsp;&nbsp;&nbsp;&nbsp; Shared secret", cfg.getFValue("<input type=text name=sradiusSecret size=20 maxlength=64 value='%s'>", alist.getValue(SHAREDSECRIT2)));
        cfg.skipRow();	
	 cfg.runScript("wpaHandler(0)");
	}
	else {		// Infrastructure

	int i = 0;	
	clist.loadFromFile(CERTIFICATES);

	cfg.actionRow("PSK", cfg.getFValue("<input type=text name=psk size=40 maxlength=64 value='%s' onchange='pskChanged(1);'>", alist.getValue(PSK)));
        cfg.dispRow("&nbsp;", "Enter a text pass phrase between 8 and 63 characters. Leave blank to enable EAP Authentication.");
        cfg.skipRow();

	cfg.actionRow("Priority", cfg.getFValue("<input type=text name=prioity size=2 value=%s>", alist.getValue(PRIORITY)));
        cfg.dispRow("&nbsp;", "Priority group (integer). The priority groups will be iterated in decreasing priority (i.e., the larger the priority value, the sooner the network is matched against the scan results).");

	cfg.putSubtitle("EAP configuration");
        cfg.skipRow();

	cfg.actionRow("EAP Mode", cfg.getSelect("eap", 3, aEAPs, alist.getIntValue(EAP), "onchange=eapChanged();")); //pEnabled));
        cfg.dispRow("&nbsp;", "Select the EAP Mode.");

	cfg.actionRow("Identity", cfg.getFValue("<input type=text name=id size=20 maxlength=24 value='%s' >", alist.getValue(ID)));
	cfg.dispRow("&nbsp;", "Identity string for EAP");

	cfg.actionRow("Password", cfg.getFValue("<input type=password name=passwd size=20 maxlength=24 value='%s' >", alist.getValue(PSWD)));
        cfg.dispRow("&nbsp;", "Password string for EAP");

	n = getCert(0);
	i = getSelOpt(aCerts, n, alist.getValue(CACERT));
        cfg.actionRow("CA_certificates", cfg.getSelect("ca_cert", n, aCerts, i, " "));
	cfg.dispRow("&nbsp;", "File path to CA certificate file. This file can have one or more trusted CA certificates. If ca_cert is not included, server certificate will not be verified.");

	n = getCert(1);
	i = getSelOpt(aCerts, n, alist.getValue(CLCERT));
        cfg.actionRow("Client_certificate", cfg.getSelect("cl_cert", n, aCerts, i, " "));
	cfg.dispRow("&nbsp;", "File path to client certificate file (PEM/DER)");

	n = getCert(2);
	i = getSelOpt(aCerts, n, alist.getValue(PKCERT));
        cfg.actionRow("Private key",cfg.getSelect("priv_key", n, aCerts, i, " "));  
	cfg.dispRow("&nbsp;", "File path to client private key file (PEM/DER/PFX)");

	cfg.actionRow("Private key password", cfg.getFValue("<input type=password name=priv_key_pswd size=20 maxlength=64 value='%s' >", alist.getValue(PKPSWD)));
	cfg.dispRow("&nbsp;", "Password for private key file");

	i = alist.getIntValue(PHASE2);
        cfg.actionRow("Tunneled Authetication", cfg.getSelect("phase2", 4, aPhase2s, i, " "));
        cfg.dispRow("&nbsp;", "Phase2 (inner authentication with TLS tunnel)");
        cfg.skipRow();
	n = 4;
	cfg.runScript("wpaHandler(1)");
	}
	}
	break;
	}
			// SUBMIT BUTTON
        cfg.actionRow("Apply settings", cfg.getFValue("<input type=submit name=applySecConf value=Apply onClick='return Validate(%s);'> <input type=reset value=Cancel id=reset1 name=reset1>", cfg.convertS(n)));
	cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

	if(n == 4) {		// Only for infrastructure WPA
	int i;
	char* tHeader[] = {
                "Certificate",
                "Type",
                "Delete",
                NULL
        };
	char* tHeader1[] = {
                "Certificate",
                "Type",
                "From",
		" ",
                NULL
        };

	char buff[1000];

        cfg.putTag("</form>");
	cfg.putTag("<form name=wlan_upload action=wlan_secu.cgi method=post enctype=multipart/form-data>");

	cfg.skipArea();

	cfg.putSubtitle("Certificate upload");
	cfg.skipRow();

	if (clist.getCount() > 0) {	// NOT EMPTY LIST
	cfg.startTable("Current Certificates", 3, tHeader);
	for (i=0; i<clist.getCount(); i++) {
		cfg.putFmtTag("<td><input disabled size=30 value=%s name=cfile%d></td>\n", clist.getName(i), i);
		cfg.putFmtTag("<td><input disabled size=20 value=%s name=ctype%d></td>\n", aTypes[clist.getIntValue(i)], i);
	       	cfg.putFmtTag("<td colspan=2>&nbsp;&nbsp;&nbsp;&nbsp;<input type=checkbox name=cdelete%d></td>\n</tr>\n", i);
	}

	cfg.endTable();
	cfg.skipRow();
	}
	
	cfg.startTable("Add new Certificates", 4, tHeader1);
	for (i=0; i<3; i++) {
		cfg.putFmtTag("<td><input size=20 name=file%d></td>\n", i);
                sprintf(buff, "type%d", i);
                strcpy(buff, cfg.getSelect(buff, 3, aTypes, i, " "));
                cfg.putFmtTag("<td>%s</td>\n", buff);
                cfg.putFmtTag("<td colspan=2><input type=file size=10 name=local%d></td>\n</tr>\n", i);

	}
	cfg.endTable();
        cfg.dispRow("&nbsp;", "Click &quot;Browse...&quot; to select a certificate file to upload. Only official certificate files from the server should be used.");
	
        cfg.skipRow();
	cfg.actionRow("Apply settings", "<input type=submit name=upLoad value=Apply onClick='return ValidateUpload();'> <input type=reset value=Cancel id=reset1 name=reset1>");
        cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();
	}

	cfg.putTag("</form>");
	cfg.endPage();
	
}

void ShowFailureMessage(int n)
{
        cfg.putHeader("Pegasus Wireless CYNALYNX - Failure Message");
        cfg.startPage("Failure Message", "This page contains the possible failure reasons during uploading certificates.");

        cfg.dispRow("<font class=f1 color=red>Upload failure</font>", "&nbsp;");        cfg.dispRow("&nbsp;", "Several factors may cause the failure:<br>1. No enough space available.</br>2. Data transfer timedout due to fair or bad wireless connection.</br>3. System busy processing other application data.<br><br>please try it later.</br>");

	cfg.putFmtTag("<b>error = %d</b><br>\n", n);
        cfg.putTag("<form name=wlan_upload action=wlan_secu.cgi method=post enctype=multipart/form-data>");
        cfg.skipRow();
        cfg.actionRow("&nbsp;", "<input type=submit name=continue value=Continue>");
        cfg.putTag("</form>");
        cfg.endPage();
}

char* trim(char*p)			// trim specail space or tailer space
{
	int i = strlen(p) - 1;
	if (i == 8 && p[0] == p[1] && p[1] == p[8]) {
		p[0] = 0;	i = 0;
	}
	while(i>0) 
		if(p[i] != ' ') break;
		else p[i--] = 0;
	return p;
}

int SetSecurity()
{

  int i, n = wlist.getIntValue(SECUOPTION);
  char p[4][100];
  if (n == 0) {		// NONE
        if (cgiFormSelectSingle("wepEnabled", aSecuOptions, 3, &i, 0+1) == cgiFormSuccess) 
        wlist.setValue(WPKENABLED, i);
  }
  else {
	i = 0;
  	if (cgiFormCheckboxSingle("wepEnabled") == cgiFormSuccess) 
		i = 1;
  	else wlist.setValue(WPKENABLED, 0);
  }
  if (n == 1 && i) {		// WEP
  	i = 0;
  	if (cgiFormSelectSingle("authOption", aAuthTypes, 2, &i, 0) == cgiFormSuccess)  
		wlist.setValue(AUTHTYPE, i);

  	i = 0;
  	if (cgiFormSelectSingle("wepKeyLen", aWepKlens, 2, &i, 0) == cgiFormSuccess) 
		wlist.setValue(WEPKEYLEN, i);
  
  	i = 0;
  	if (cgiFormSelectSingle("useKey", aTransKeys, 4, &i, 0) == cgiFormSuccess) 
	wlist.setValue(KEYINDEX, i);

  	if (cgiFormStringNoNewlines("wepKey1", p[0], WEP_KEY_LEN) == cgiFormSuccess &&
      cgiFormStringNoNewlines("wepKey2", p[1], WEP_KEY_LEN) == cgiFormSuccess &&
      cgiFormStringNoNewlines("wepKey3", p[2], WEP_KEY_LEN) == cgiFormSuccess &&
      cgiFormStringNoNewlines("wepKey4", p[3], WEP_KEY_LEN) == cgiFormSuccess)
	 {
		wlist.setValue(WEPKEY0, p[0]);
		wlist.setValue(WEPKEY1, p[1]);
		wlist.setValue(WEPKEY2, p[2]);
		wlist.setValue(WEPKEY3, p[3]);
  	}
  }
  else if (n == 2 && i) {	// WPA
	bool isAP = wlist.getIntValue(RADIOMODE) == 0;
        alist.loadFromFile((char*)(isAP? WPA_AP_CONF: WPA_WS_CONF));

	if (cgiFormSelectSingle("wpamode",aWpaModes, 3, &i, 0) == cgiFormSuccess)
		alist.setValue(WPAMODE, i);
	
	if (cgiFormSelectSingle("ciphermode", aCipherTypes, 3, &i, 0) == cgiFormSuccess)
                alist.setValue(CIPHERTYPE, i);

	if (cgiFormStringNoNewlines("psk", p[0], 65) == cgiFormSuccess) {
		i = strlen(p[0]);
		if (i > 0 && i < 8) p[0][0] = 0;
	}
	alist.setValue(PSK, p[0]);

	if (isAP) {		// Access Point Mode

	if (cgiFormStringNoNewlines("wpagkey", p[0], 8) == cgiFormSuccess)
                alist.setValue(GKTIME, p[0]);

	if (cgiFormStringNoNewlines("authTimeout", p[0], 8) == cgiFormSuccess)
                alist.setValue(AUTHTIME, p[0]);

	cgiFormStringNoNewlines("radiusIP", p[0], 20); 
        alist.setValue(RADIUSIP, p[0]);

	cgiFormStringNoNewlines("radiusPort", p[0], 8);
        alist.setValue(RADIUSPORT, p[0]);

	cgiFormStringNoNewlines("radiusSecret", p[0], 64);
        alist.setValue(SHAREDSECRIT, p[0]);

	cgiFormStringNoNewlines("sradiusIP", p[0], 20);
        alist.setValue(RADIUSIP2, p[0]);

        cgiFormStringNoNewlines("sradiusPort", p[0], 8);
        alist.setValue(RADIUSPORT2, p[0]);

        cgiFormStringNoNewlines("sradiusSecret", p[0], 64);
        alist.setValue(SHAREDSECRIT2, p[0]);

	alist.saveToFile(WPA_AP_CONF);
	}
	else {			// Infrastructure Station Mode

        if (cgiFormStringNoNewlines("prioity", p[0], 4) == cgiFormSuccess)
                alist.setValue(PRIORITY, p[0]);

	if (cgiFormSelectSingle("eap", aEAPs, 3, &i, 0) == cgiFormSuccess)
                alist.setValue(EAP, i);

	cgiFormStringNoNewlines("id", p[0], 24);
        alist.setValue(ID, p[0]);

	cgiFormStringNoNewlines("passwd", p[0], 24);
        alist.setValue(PSWD, trim(p[0]));
	
	if (cgiFormStringNoNewlines("ca_cert", p[0], 24) == cgiFormSuccess)
                alist.setValue(CACERT, trim(p[0]));

	if (cgiFormStringNoNewlines("cl_cert", p[0], 24) == cgiFormSuccess)
                alist.setValue(CLCERT, trim(p[0]));

	if (cgiFormStringNoNewlines("priv_key", p[0], 24) == cgiFormSuccess)
                alist.setValue(PKCERT, trim(p[0]));

	cgiFormStringNoNewlines("priv_key_pswd", p[0], 24);
        alist.setValue(PKPSWD, trim(p[0]));

	i = 0;
	cgiFormSelectSingle("phase2", aPhase2s, 4, &i, 0);
        alist.setValue(PHASE2, i);

	alist.saveToFile(WPA_WS_CONF);
	}
  }
  wlist.saveToFile(WLAN_CONFIG);
  return 1;
}

int ProcCert()
{
	int i, j=0;
	int size, got;
  	int totalSize = 0;
 	int fd;

	char cdelete[20];
	char file[20];
	char type[20];
	char local[20];

	char localname[100];
	char filename[100];
	int  ctype;
	char fullname[200];
	char buffer[1024];
	cgiFilePtr plocal;             // file pointer

	clist.loadFromFile(CERTIFICATES);
	for (i=0; i<clist.getCount(); i++) {
		sprintf(cdelete, "cdelete%d", i);
		if (cgiFormCheckboxSingle(cdelete) == cgiFormSuccess) { 
			// mark to delete
			sprintf(fullname, CERTFORMAT, clist.getName(i));
			sprintf(buffer, "rm -f %s ",fullname);
			system(buffer);
			j++;
			clist.remove(i);
		}
	}	
	if(j) {
		clist.saveToFile(CERTIFICATES);
		clist.init(10);
		clist.loadFromFile(CERTIFICATES);
		j = 0;
	}

	for (i=0; i< 3; i++) {
		sprintf(file, "file%d", i);
		sprintf(type, "type%d", i);
		sprintf(local, "local%d", i);
		    // does this ith uploaded file exist?
	 	if ((cgiFormFileName(local, localname, sizeof(localname)) != cgiFormSuccess) ||
        (cgiFormFileSize(local, &size) != cgiFormSuccess || size <= 0) ||
        (cgiFormFileOpen(local, &plocal) != cgiFormSuccess)) 
      		continue;

		// where is this uploaded file to be stored on the dom?
    		if ((cgiFormStringNoNewlines(file, filename, sizeof(filename)) != cgiFormSuccess)
        	|| (cgiFormSelectSingle(type, aTypes, 3, &ctype, 0) != cgiFormSuccess))	      {
			cgiFormFileClose(plocal);
		if (cgiFormStringNoNewlines(file, filename, sizeof(filename)) != cgiFormSuccess)
			return -5;
     			else  return -1;
    		}
     sprintf(fullname, CERTFORMAT, filename);
    // persistantly save the uploaded file on the dom
    if ((fd = creat(fullname, S_IRWXU)) <= 0) {
      cgiFormFileClose(plocal);
      return -2;
    }
    totalSize = 0;
    while (cgiFormFileRead(plocal, buffer, sizeof(buffer), &got) == cgiFormSuccess) {
      write(fd, buffer, got);
      totalSize += got;
    }
    if (totalSize != size) {
      cgiFormFileClose(plocal);
      return -3;
    }
    cgiFormFileClose(plocal);
    close(fd);
    // add to list
	clist.setProperty(filename, ctype);
	j++;
  }	
  if (j) clist.saveToFile(CERTIFICATES);
  return 0; 
}

int cgiMain() 
{
	wlist.loadFromFile(WLAN_CONFIG);
        int n = 0;

	if (cgiFormSubmitClicked("applySecConf") == cgiFormSuccess) {
		SetSecurity();	
	}
	else if (cgiFormSubmitClicked("upLoad") ==  cgiFormSuccess) {
		n = ProcCert();
	}
	if (n == 0)
		showPage();
	else
		ShowFailureMessage(n);
	return 0;
}
