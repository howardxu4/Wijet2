// global variables
var localfile0 = '';
var localfile1 = '';
var localfile2 = '';
var localfile3 = '';
var localfile4 = '';
function IsSupportedBrowser() {
	var browser = navigator.userAgent.toLowerCase();
	if (browser.indexOf('msie') != -1 && browser.indexOf('win') != -1 &&
	    browser.indexOf('opera') == -1 && browser.indexOf('netscape') == -1 &&
	    browser.indexOf('omni') == -1 && browser.indexOf('safari') == -1) {
		return true; // IE on Windows
	}
	return false;
}
function IsSupportedVideoType(filename) {
	var vob = /.vob$/i;
	var mpg = /.mpe?g$/i;
	var wav = /.wav$/i;
	var ac3 = /.ac3$/i;
	if (filename.match(vob) || filename.match(mpg) || filename.match(wav) || filename.match(ac3)) return true;
	return false;
}
function GetFileNameFromFullName(filename) {
	var os = navigator.userAgent.toLowerCase();
		var pathArray;
	if (os.indexOf('win') == -1) { // not windows
		pathArray = filename.split('/');
	} else { // windows
		pathArray = filename.split('\\');
	}
	var len = pathArray.length; 
	var name = pathArray[len-1].replace(' ', '_');
	return name;
}
function ValidateDuration(duration) {
	var pattern = /^(\+|\-)?([1-9])([0-9])*$/i;
	if (!duration.match(pattern) && (duration != 0)) {
		alert('Invalid duration: ' + duration + '. only integer numbers are allowed.');
		return false;
	}
	if (duration > 0 && duration < 5) {
		alert('The minimum duration is 5 seconds!');
		return false;
	}
	if (duration > 0 && duration > 24 * 3600) {
		alert('Duration too long. Please use -1 instead for long duration!');
		return false;
	}
	if (duration < 0 && duration != -1) {
		alert('Please use -1 instead for long duration!');
		return false;
	}
	return true;
}
function ValidateOrder(order) {
	var pattern = /^([1-9])([0-9])*$/i;
	if (!order.match(pattern)) {
		alert('Invalid order: ' + order + '. Only positive integer numbers are allowed.');
		return false;
	}
	if (order > 200) {
		alert('Maximum value for order is ' + 200 + '!');
		return false;
	}
	if (order < 0) {
		alert('Order must be a non-negative integer!');
		return false;
	}
	return true;
}
function ValidateFileName(name) {
	var pattern = /^[a-zA-Z_]([0-9a-zA-Z_.\-])+$/i;
	var jpg = /.jpg$/i;
	if (!name.match(pattern)) {
		alert('Invalid file name: ' + name +'! Please note that -, _, . and alphanumeric characters are allowed and the first character must not be - or numeric.');
		return false;
	}
	if (!name.match(jpg)) {
		alert('Only JPEG files are allowed!');
		return false;
	}
	return true;
}
function ValidateFolderName(name) {
	var pattern = /^[a-zA-Z_]([0-9a-zA-Z_\-])+$/i;
	if (!name.match(pattern)) {
		alert('Invalid folder name: ' + name +'! Please note that -, _ and alphanumeric characters are allowed and the first character must not be - or numeric.');
		return false;
	}
	return true;
}
function ValidateNamesForFoldersAndFiles() {
	 if (document.wlan_upload.local0.value.length != 0) {
		 if (!ValidateFileName(document.wlan_upload.file0.value)) return false;
		 if (!ValidateFolderName(document.wlan_upload.folder0.value)) return false;
	 }
	 if (document.wlan_upload.local1.value.length != 0) {
		 if (!ValidateFileName(document.wlan_upload.file1.value)) return false;
		 if (!ValidateFolderName(document.wlan_upload.folder1.value)) return false;
	 }
	 if (document.wlan_upload.local2.value.length != 0) {
		 if (!ValidateFileName(document.wlan_upload.file2.value)) return false;
		 if (!ValidateFolderName(document.wlan_upload.folder2.value)) return false;
	 }
	 if (document.wlan_upload.local3.value.length != 0) {
		 if (!ValidateFileName(document.wlan_upload.file3.value)) return false;
		 if (!ValidateFolderName(document.wlan_upload.folder3.value)) return false;
	 }
	 if (document.wlan_upload.local4.value.length != 0) {
		 if (!ValidateFileName(document.wlan_upload.file4.value)) return false;
		 if (!ValidateFolderName(document.wlan_upload.folder4.value)) return false;
	 }
	 var cnfiles = 6;
	 var nfiles = 5;
	 for(var i = 0; i < cnfiles; i++) {
		 var cfilename = eval('document.wlan_upload.cfile'+ i +'.value');
		 var cdelete   = eval('document.wlan_upload.cdelete'+ i +'.checked');
		 if (!cdelete) {  // not delete
		 for(var j = 0; j < nfiles; j++) {
			 var filename = eval('document.wlan_upload.file'+ j +'.value');
			 if (filename.match(cfilename)) { // same file name?
				 alert('Duplicate file names in column File are found!');
				 return false;
			 }
		 }
		 }
	 }
	 for(var i = 0; i < nfiles; i++) {
		 var fname = eval('document.wlan_upload.file'+ i +'.value');
		 if (fname.length == 0) continue;
		 for(var j = i+1; j < nfiles; j++) {
			 var fname1 = eval('document.wlan_upload.file'+ j +'.value');
			 if (fname1.match(fname)) { // same file name?
				 alert('Duplicate file names in column File are found!');
				 return false;
			 }
		 }
	 }
	 return true;
}
function ValidatePlayingOrders() {
	if (!ValidateOrder(document.wlan_upload.order0.value)) return false;
	if (!ValidateOrder(document.wlan_upload.order1.value)) return false;
	if (!ValidateOrder(document.wlan_upload.order2.value)) return false;
	if (!ValidateOrder(document.wlan_upload.order3.value)) return false;
	if (!ValidateOrder(document.wlan_upload.order4.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder0.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder1.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder2.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder3.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder4.value)) return false;
	if (!ValidateOrder(document.wlan_upload.corder5.value)) return false;
	 var cnfiles = 6;
	 var nfiles = 5;
	 for(var i = 0; i < cnfiles; i++) {
		 var order = eval('document.wlan_upload.corder'+ i +'.value');
		 for(var j = 0; j < nfiles; j++) {
			 if (order == eval('document.wlan_upload.order'+ j +'.value')) {
				 alert('Duplicate values for orders are found!');
				 return false;
			 }
		 }
	 }
	 return true;
}
function ValidatePlayingDurations() {
	if (!ValidateDuration(document.wlan_upload.duration0.value)) return false;
	if (!ValidateDuration(document.wlan_upload.duration1.value)) return false;
	if (!ValidateDuration(document.wlan_upload.duration2.value)) return false;
	if (!ValidateDuration(document.wlan_upload.duration3.value)) return false;
	if (!ValidateDuration(document.wlan_upload.duration4.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration0.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration1.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration2.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration3.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration4.value)) return false;
	if (!ValidateDuration(document.wlan_upload.cduration5.value)) return false;
	return true;
}
function ValidatePlayingList() {
	var nimages = 0;
	var jpg = /.jpg$/i;
	var filename = document.wlan_upload.file0.value;
	if (filename.match(jpg)) { // jpeg image file
		nimages++;
	}
	var filename = document.wlan_upload.file1.value;
	if (filename.match(jpg)) { // jpeg image file
		nimages++;
	}
	var filename = document.wlan_upload.file2.value;
	if (filename.match(jpg)) { // jpeg image file
		nimages++;
	}
	var filename = document.wlan_upload.file3.value;
	if (filename.match(jpg)) { // jpeg image file
		nimages++;
	}
	var filename = document.wlan_upload.file4.value;
	if (filename.match(jpg)) { // jpeg image file
		nimages++;
	}
	var filename = document.wlan_upload.cfile0.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete0.checked) { // not deleted
			nimages++;
		}
	}
	var filename = document.wlan_upload.cfile1.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete1.checked) { // not deleted
			nimages++;
		}
	}
	var filename = document.wlan_upload.cfile2.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete2.checked) { // not deleted
			nimages++;
		}
	}
	var filename = document.wlan_upload.cfile3.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete3.checked) { // not deleted
			nimages++;
		}
	}
	var filename = document.wlan_upload.cfile4.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete4.checked) { // not deleted
			nimages++;
		}
	}
	var filename = document.wlan_upload.cfile5.value;
	if (filename.match(jpg)) { // jpeg image file
		if (!document.wlan_upload.cdelete5.checked) { // not deleted
			nimages++;
		}
	}
	if (nimages == 0) {
		alert('No banner in the play list. The play list must contain at least one image file!');
		return false;
	}
	return true;
}
function AddPlayListHandler(whichfile) {
	var filename = '';
	var jpg = /.jpg$/i;
		switch(whichfile) {
		case 0: 
			filename = document.wlan_upload.local0.value;
			break;
		case 1: 
			filename = document.wlan_upload.local1.value;
			break;
		case 2: 
			filename = document.wlan_upload.local2.value;
			break;
		case 3: 
			filename = document.wlan_upload.local3.value;
			break;
		case 4: 
			filename = document.wlan_upload.local4.value;
			break;
		}
	if (IsSupportedBrowser()) { // IE on windows
		var oas = new ActiveXObject("Scripting.FileSystemObject");
		var e = oas.getFile(filename);
		var cf = 0;
		var cfile = eval('localfile'+whichfile);
		if (cfile.length != 0) cf = Math.ceil(oas.getFile(cfile).size/1024);
		var f = Math.ceil(e.size/1024);
		document.wlan_upload.space.value -= ( cf);
	}
	if(whichfile == 0) localfile0 = filename;
	if(whichfile == 1) localfile1 = filename;
	if(whichfile == 2) localfile2 = filename;
	if(whichfile == 3) localfile3 = filename;
	if(whichfile == 4) localfile4 = filename;
	var folder = eval('document.wlan_upload.folder'+whichfile);
	var file = eval('document.wlan_upload.file'+whichfile);
	file.value = GetFileNameFromFullName(filename);
	if(filename.match(jpg)) {
		folder.value = 'images';
	} else if (IsSupportedVideoType(filename)) {
		folder.value = 'videos';
	} else {
		alert('Unsupported file type: ' + filename);
		folder.value=''; file.value = '';
		var local = eval('document.wlan_upload.local'+whichfile); 
		local.value='';
		return false;
	}
	return true;
}
function ValidateLocalPlayList() {
	 var nfiles = 5;
	 var jpg = /.jpg$/i;
	 for(var i = 0; i < nfiles; i++) {
		 var filename = eval('document.wlan_upload.local' + i +'.value');
		 if (filename.length != 0) { 
			 if (!filename.match(jpg)) { // jpeg image file
				 alert('Unsupported file type: ' + filename);
				 return false;
			 }
		 }
	 }
	 return true;
}
function PlayListHandler() {
	if (!ValidateNamesForFoldersAndFiles()) return false;
	if (!ValidatePlayingOrders()) return false;
	if (!ValidatePlayingDurations()) return false;
	if (!ValidatePlayingList()) return false;
	if (!ValidateLocalPlayList()) return false;
	if (document.wlan_upload.space.value < 0) {
		alert('no enough space available for upload all the files!');
		return false;
	}
	if (!confirm('Please be sure that power is properly being supplied to the unit during the upload process. The unit will be rebooted and the wireless connection will be terminated. Do you want to proceed?')) return false;
	return true;
}
