function validateKey(key,keyLen,keyId) {
	var len=keyLen;
	var len2 = 13;
	var a = new Array;
	if (key.match (/^\s*$/)) {
		for (var i=0; i<len2; i++) a = a.concat (0);
		if (!a) {
			alert('Invalid WEP Key ' + keyId + '!');
			return false;
		}
		return true;
	}
	var got = key.match (/^\s*([0-9a-fA-F]+)\s*$/);
	if (got) {
		if (got[1].length != len) {
			alert('Invalid WEP Key ' + keyId + '!');
			return false;
		}
		a = new Array;
		var i;
		for (i=0; i < len; i++) {
			a = a.concat (parseInt(got[1].substr(i*2,2),16));
		}
		for (; i < len2; i++) {
			a = a.concat (0);
		}
		if (!a) {
			alert('Invalid WEP Key ' + keyId + '!');
			return false;
		}
		return true;
	} else {
		alert('Invalid WEP Key ' + keyId + '!');
		return false;
	}
}


function KeyLengthHandler() {
	var len = document.wlan_secu.wepKeyLen.selectedIndex;
	// maxLength is used, not maxlength
	if (len == 0) { // 64 bits
	    document.wlan_secu.wepKey1.maxLength = 10;
	    document.wlan_secu.wepKey2.maxLength = 10;
	    document.wlan_secu.wepKey3.maxLength = 10;
	    document.wlan_secu.wepKey4.maxLength = 10;
	    document.wlan_secu.wepKey1.value = '0000000000';
	    document.wlan_secu.wepKey2.value = '0000000000';
	    document.wlan_secu.wepKey3.value = '0000000000';
	    document.wlan_secu.wepKey4.value = '0000000000';
	} else { // 128 bits
	    document.wlan_secu.wepKey1.maxLength = 26;
	    document.wlan_secu.wepKey2.maxLength = 26;
	    document.wlan_secu.wepKey3.maxLength = 26;
	    document.wlan_secu.wepKey4.maxLength = 26;
	    document.wlan_secu.wepKey1.value = '00000000000000000000000000';
	    document.wlan_secu.wepKey2.value = '00000000000000000000000000';
	    document.wlan_secu.wepKey3.value = '00000000000000000000000000';
	    document.wlan_secu.wepKey4.value = '00000000000000000000000000';
	}
}


function RadioModeHandler() {
	var radiomode = document.wlan_secu.radiomode.selectedIndex;
	switch(radiomode) {
	case 1:  // in infrastructure mode only manual mode is allowed
	  document.wlan_secu.csmode[0].disabled = true;
	  document.wlan_secu.csmode[1].checked = true;
	  break;
	default: // in AP mode, both automatic and manual mode are allowed
	  document.wlan_secu.csmode[0].disabled = false;
	  document.wlan_secu.csmode[0].checked = true;
	  break;
	}
	CSModeHandler();
}


function SSIDHandler() {
	var ssid = document.wlan_secu.ssid.value;
	var mode = document.wlan_secu.radiomode.selectedIndex;
	var pattern = /^\s*([0-9a-zA-Z _\-\.\#])+\s*$/i;
	if (mode == 0) { // AP
	  if (ssid.length == 0) {
	    alert('Invalid SSID for Access Point'); return false;
	  }
	}
	if (!ssid.match(pattern)) {
	  alert('Invalid SSID entered.');
	  return false;
	}
	return true; 
}

function ValidateSSIDlength() {
}

function ValidateChannel() {
	var st = document.wlan_secu.x80211mode.selectedIndex;
	var ch = document.wlan_secu.channel.selectedIndex;
	if (st <= 1 && ch > 17) {
		alert("Invalid channel " + document.wlan_secu.channel.options[ch].text + "\nOn 802.11A valid channel start from 36 \nSystem reset to default channel 40");
		document.wlan_secu.channel.selectedIndex = 16;
	}
	else if ( st > 1 && ch < 18) {
		alert("Invalid channel " + document.wlan_secu.channel.options[ch].text + "\nOn 802.11 G valid channel between (1-14)\nSystem reset to default channel 6");	
		document.wlan_secu.channel.selectedIndex = 23;
	}
	else return true;
	return false;
}

function Validate(n) {
	if (n == 1) {
	var enableWep = document.wlan_secu.wepEnabled.checked;
	var keyLen = document.wlan_secu.wepKeyLen.selectedIndex;
	var key1 = document.wlan_secu.wepKey1.value;
	var key2 = document.wlan_secu.wepKey2.value;
	var key3 = document.wlan_secu.wepKey3.value;
	var key4 = document.wlan_secu.wepKey4.value;
	var len = ((keyLen == 0)? 10: 26) + (0);
	if (!validateKey(key1, len, 1)) return false;
	if (!validateKey(key2, len, 2)) return false;
	if (!validateKey(key3, len, 3)) return false;
	if (!validateKey(key4, len, 4)) return false;
//	if(!confirm("Clicking Apply will reboot the unit. You will need to configure your wireless adapter card to match the WEP settings on the unit in order to reconnect. To reconnect to this Admin webpage, launch the Admin utility on your computer. Do you want to procceed?")) return false;
	}
	else if (n>1) return ValidatePSK();
	return true;
}

function x8021Handler(b){
	if (b != 0) {
		document.wlan_secu.authTimeout.disabled = true;
		document.wlan_secu.radiusIP.disabled = true;
		document.wlan_secu.radiusPort.disabled = true;
		document.wlan_secu.radiusSecret.disabled = true;
		document.wlan_secu.sradiusIP.disabled = true;
                document.wlan_secu.sradiusPort.disabled = true;
                document.wlan_secu.sradiusSecret.disabled = true;
	}
	else {
                document.wlan_secu.authTimeout.disabled = false;
                document.wlan_secu.radiusIP.disabled = false;
                document.wlan_secu.radiusPort.disabled = false;
                document.wlan_secu.radiusSecret.disabled = false;
                document.wlan_secu.sradiusIP.disabled = false;
                document.wlan_secu.sradiusPort.disabled = false;
                document.wlan_secu.sradiusSecret.disabled = false;
	}
}

function eapChanged()
{
	if (document.wlan_secu.psk.value.length != 0 ||
                !document.wlan_secu.wepEnabled.checked ||
		document.wlan_secu.eap.selectedIndex == 1)
		document.wlan_secu.phase2.disabled = true;
	else
		document.wlan_secu.phase2.disabled = false;
}

function eapHandler(b)
{
	if (b != 0){
		document.wlan_secu.eap.disabled = true;
		document.wlan_secu.id.disabled = true;
		document.wlan_secu.passwd.disabled = true;
		document.wlan_secu.ca_cert.disabled = true;
		document.wlan_secu.cl_cert.disabled = true;
		document.wlan_secu.priv_key.disabled = true;
		document.wlan_secu.priv_key_pswd.disabled = true;
	}
	else {
                document.wlan_secu.eap.disabled = false;
                document.wlan_secu.id.disabled = false;
                document.wlan_secu.passwd.disabled = false;
                document.wlan_secu.ca_cert.disabled = false;
                document.wlan_secu.cl_cert.disabled = false;
                document.wlan_secu.priv_key.disabled = false;
                document.wlan_secu.priv_key_pswd.disabled = false;
	}
	eapChanged();
}

function validatePSK()
{
	n = document.wlan_secu.psk.value.length;
	if (n > 0 && n < 8) {
		alert("The WPA pre-shared keys require a text pass phrase between 8 and 63 characters");
		document.wlan_secu.psk.focus();
		return false;
	}
	return true; 
}

function pskChanged(b) {
	if (validatePSK() == true) 	
        if (document.wlan_secu.psk.value.length != 0 ||
		!document.wlan_secu.wepEnabled.checked)
		if (b == 0)  x8021Handler(1);
		else eapHandler(1);
        else
                if (b == 0) x8021Handler(0);
		else eapHandler(0);

}

function wpaHandler(b) {
	if (!document.wlan_secu.wepEnabled.checked) {
		document.wlan_secu.wpamode.selectedIndex = 0;
		document.wlan_secu.wpamode.disabled = true;
		document.wlan_secu.ciphermode.disabled = true;
		document.wlan_secu.psk.disabled = true;
		if (b == 0)
		document.wlan_secu.wpagkey.disabled = true;
		else
		document.wlan_secu.prioity.disabled = true;
	} else {
		document.wlan_secu.wpamode.disabled = false;
		document.wlan_secu.ciphermode.disabled = false;
		document.wlan_secu.psk.disabled = false;
		if (b == 0)
		document.wlan_secu.wpagkey.disabled = false;
		else
		document.wlan_secu.prioity.disabled = false;
	}
	pskChanged(b);
}

function wepHandler() {
	if (!document.wlan_secu.wepEnabled.checked) {
//		document.wlan_secu.authOption.selectedIndex = 0;
		document.wlan_secu.authOption.disabled = true;
		document.wlan_secu.wepKeyLen.disabled = true;
		document.wlan_secu.wepKey1.disabled = true;
		document.wlan_secu.wepKey2.disabled = true;
		document.wlan_secu.wepKey3.disabled = true;
		document.wlan_secu.wepKey4.disabled = true;
		document.wlan_secu.useKey.disabled = true;
	} else {
		document.wlan_secu.authOption.disabled = false;
		document.wlan_secu.wepKeyLen.disabled = false;
		document.wlan_secu.wepKey1.disabled = false;
		document.wlan_secu.wepKey2.disabled = false;
		document.wlan_secu.wepKey3.disabled = false;
		document.wlan_secu.wepKey4.disabled = false;
		document.wlan_secu.useKey.disabled = false;
	}
}

function CSModeHandler() {
/*
	var radiomode = document.wlan_secu.radiomode.selectedIndex;
	var csmode = document.wlan_secu.csmode[0].checked;
	if(csmode) {
		document.wlan_secu.channel.disabled = true;
		document.wlan_secu.channel.onfocus = document.wlan_secu.channel.blur;
	} else {
		document.wlan_secu.channel.disabled = false;
		document.wlan_secu.channel.onfocus = null;
	}

	 if(csmode && radiomode == 0) {
		 document.wlan_secu.ssid.maxLength = 30;
		 if (document.wlan_secu.ssid.value.length > 30) 
		   document.wlan_secu.ssid.value = document.wlan_secu.ssid.value.substring(0, 30);
	 } else {
		 document.wlan_secu.ssid.maxLength = 32;
	 }
		document.wlan_secu.ssid.disabled = false;
		document.wlan_secu.ssid.onfocus = null;
	if(radiomode == 1) {
		document.wlan_secu.channel.disabled = true;
		document.wlan_secu.channel.onfocus = document.wlan_secu.channel.blur;
	}
*/
}

function ValidateUpload()
{
	var ok = true;
	if(document.wlan_upload.local0.value.length > 0)
		if(document.wlan_upload.file0.value.length == 0) {
			document.wlan_upload.file0.focus(); ok = false; }
	if(document.wlan_upload.local1.value.length > 0)
                if(document.wlan_upload.file1.value.length == 0) {
			document.wlan_upload.file1.focus(); ok = false; }
	if(document.wlan_upload.local2.value.length > 0)
                if(document.wlan_upload.file2.value.length == 0) {
			document.wlan_upload.file2.focus(); ok = false; }
	if (ok == false) 
		alert("The Certificate name can't be empty");
	return ok;
}
