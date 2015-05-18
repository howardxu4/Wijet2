function ValidateFormat(str) {
  var p = /[^a-zA-Z0-9]/;
  return !p.test(str);
}
function ValidatePassword() {
  var passwd = document.wlan_admin.npword.value;
  var confirm = document.wlan_admin.cpword.value;
  if (passwd.length == 0) {
    alert('No password entered!');
    return false;
  }
  if (!ValidateFormat(passwd)) {
    alert('Invalid format for Password! Please re-enter. Only alphanumeric characters allowed.');
    return false;
  }
  if (passwd != confirm) {
    alert('Inconsistent password. Please re-enter.')
	return false;
  }
  return true;
}
function ValidateIP(str) {
  var p = /[^0-9.]/;
  return !p.test(str);
}
function Reboot() {
	if (!confirm('The unit will be rebooted and the wireless connection will be terminated. Do you want to proceed?')) return false;
	return true;
}
function ValidatePatch() {
        var file = document.wlan_admin.firmware.value;
        var gzip = /.tar.gz$/i;
                if (!file.match(gzip)) {
                        alert('Invalid firmware file format. Please use official firmware image from vendors!');
                        return false;
                }
        return true;
}
function PatchHandler() {
       if (!ValidatePatch()) {
                return false;
        }
        if (!confirm('Please be sure that power is properly being supplied to the unit during the upload process. The unit will be rebooted and the wireless connection will be terminated. Do you want to proceed?')) return false;
        return true;
}
function Reset() {
        if (!confirm('Your current user account, wireless settings, and banner page settings will be reset and your current connection will be terminated. You will need to reconnect to this webpage by launching the Admin utility on your computer. Do you want to proceed?')) return false;
        return true;
}
function ValidateNet() {
  var addr     = document.wlan_conf.ipaddr.value;
  var mask     = document.wlan_conf.ipmask.value;
  var gway     = document.wlan_conf.gateway.value;
  if (addr.length == 0 || mask.length == 0 || gway.length == 0) {
    alert('You need to enter IP address, Subnet mask and Default gateway!')
    return false;
  }
  if (!ValidateIP(addr) || !ValidateIP(mask) || !ValidateIP(gway)) {
    alert('Invalid format of IP address, subnet mask or default gateway! Please re-enter.')
    return false;
  }
  if(!confirm("Clicking Apply will reboot the unit.  The wireless connection will be terminated.  Do you want to procceed?")) return false;
  return true;
}
function Validate() {
  var username = document.wlan_admin.uname.value;
  if (username.length == 0 ) {
    alert('No user name is entered!')
        return false;
  }
  if (!ValidateFormat(username)) {
    alert('Invalid format for User name! Please re-enter. Only alphanumeric characters allowed.')
        return false;
  }
  if (!ValidatePassword()) {
    return false;
  }
  return true;
}

