#include "ConfigCommon.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES_TO_ADD             5
#define MAX_NUM_FILES_PLAY_LIST      200
#define MAX_FILE_SPACE		     5000

#define DISP1024   "R1024x768"
#define DISP800    "R800x600"
#define DISP640    "R640x480"

LdProp  rlist;		// refresh
LdFlist flist;		// banner
ConfigCommon cfg;

void ShowFailureMessage()
{
        cfg.putHeader("Pegasus Wireless CYNALYNX - Failure Message");
        cfg.startPage("Failure Message", "This page contains the possible failure reasons during uploading banner pages.");
	
	cfg.dispRow("<font class=f1 color=red>Upload failure</font>", "&nbsp;");
	cfg.dispRow("&nbsp;", "Several factors may cause the failure:<br>1. No enough space available.</br>2. Data transfer timedout due to fair or bad wireless connection.</br>3. System busy processing other application data.<br><br>please try it later.</br>");

	cfg.putTag("<form name=wlan_upload action=wlan_disp.cgi method=post enctype=multipart/form-data>");
        cfg.skipRow();
        cfg.actionRow("&nbsp;", "<input type=submit name=continue value=Continue>");
        cfg.putTag("</form>");
        cfg.endPage();
}

int getRefreshRate(int n)
{
	int r = 60;
	int i;
	switch(n) {
		case 0:		// 640x480
		i = rlist.getIndex("R640x480");
		if ( i != -1) r = rlist.getIntValue(i);
		break;
		case 1:		// 800x600
                i = rlist.getIndex("R800x600");
		if ( i != -1) r = rlist.getIntValue(i);
		break;
		case 2:		// 1024x768
                i = rlist.getIndex("R1024x768");
		if (i != -1) r = rlist.getIntValue(i);
		break;
	}
	return r;
}

char* itostr(int n)
{
	static char p[10];
	sprintf(p, "%d", n);
	return p;
}

int showPage()
{
	int i, j, k=0;
	char* tHeader[] = {
		"Folder",
		"File",
		"Duration",
		"Order",
		"Playable &nbsp;",
		"Delete",
		NULL
	};
	j = flist.getCount();
	for (i = 0; i<j; i++) {
		k += flist.getSize()/1024;
		flist.nextFile();
	}
	flist.setHead();

	cfg.putHeader("Pegasus Wireless CYNALYNX - Display");
	cfg.putScript("bnupload.js");
	cfg.startPage("Display", "This page contains options to upload banner pages, and change refresh rate settings.");
	cfg.putTag("<form name=wlan_upload action=wlan_disp.cgi method=post enctype=multipart/form-data>");

	cfg.putSubtitle("Banners upload");
        cfg.skipRow();
	cfg.actionRow("Space available", cfg.getFValue("<input value=%s name=space %s> Kilobytes", itostr(MAX_FILE_SPACE - k), "disabled"));
	cfg.dispRow("&nbsp;", "This is the total amount of space available for uploading banners. Upload a collection of banners consisting of multiple JPEG files, and these banners will be displayed in the order and the duration as specified. <B>Please make sure that the uploaded files do not take more space than what is available.");
        cfg.skipRow();
	
	cfg.startTable("Current banners", 6, tHeader); 
		
	for(i = 0; i < j; i++) {
		cfg.putFmtTag("<tr><td><input disabled size=7 value=images name=cfolder%d></td>\n", i);
		cfg.putFmtTag("<td><input disabled size=14 value=%s name=cfile%d></td>\n", flist.getFilename(true), i);
		cfg.putFmtTag("<td><input onchange=ValidateDuration(document.wlan_upload.cduration0.value) size=7 value=%d name=cduration%d></td>\n", flist.getDuration(),i);
		cfg.putFmtTag("<td><input onchange=ValidateOrder(document.wlan_upload.corder0.value) size=5 value=%d name=corder%d></td>\n", i+1, i);
		cfg.putFmtTag("<td><input disabled type=checkbox checked  name=playable%d</td>\n", i);
		cfg.putFmtTag("<td><input type=checkbox name=cdelete%d></td>\n</tr>\n", i);
		flist.nextFile();
	}	

	cfg.endTable();

	cfg.dispRow("&nbsp;", "This table shows the current banners on the unit. The &quot;Folder &quot; and  &quot;File &quot; columns indicate the location of the banner. The  &quot;Duration &quot; and  &quot;Order &quot; columns specify the display duration (in seconds) and display order of the banners. The  &quot;Playable &quot; column indicates whether this banner is to be included in the current display sequence, while the  &quot;Delete &quot; column allows the banner to be deleted. The display order must be unique. A negative value for &quot;Duration&quot; is used for displaying a banner indefinitely. ");

        cfg.skipRow();
	
	j = i+1;
	tHeader[4] = "From";
        cfg.startTable("Add new banners", 5, tHeader);
	
	for(i = 0; i < MAX_FILES_TO_ADD; i++) {
                cfg.putFmtTag("<tr><td><input onchange=ValidateFolderName(document.wlan_upload.folder%d.value) size=7 value=images name=folder%d></td>\n", i, i);
                cfg.putFmtTag("<td><input onchange=ValidateFileName(document.wlan_upload.file%d.value) size=7 name=file%d></td>\n", i, i);
                cfg.putFmtTag("<td><input onchange=ValidateDuration(document.wlan_upload.duration%d.value) size=7 value=10 name=duration%d></td>\n", i, i);
                cfg.putFmtTag("<td><input onchange=ValidateOrder(document.wlan_upload.order%d.value) size=5 value=%d name=order%d></td>\n", i, i+j, i);
                cfg.putFmtTag("<td colspan=2> <input type=file onchange=return AddPlayListHandler(%d) name=local%d></td>\n</tr>\n", i, i);
	}
        cfg.endTable();

        cfg.dispRow("&nbsp;", "This table allows you to upload new banners to the the unit. Click &quot;Browse...&quot; to select a JPEG file from your PC. Then change the file, duration, and order if necessary. Click &quot;Upload and Apply Changes&quot; to upload them to the unit. <B>The only folder allowed is &quot;images&quot;. Please make sure that the uploaded files will not take more space than what is available.</B>");

        cfg.skipRow();

        cfg.actionRow("Upload changes", "<input type=submit name=play value='Upload and Apply Changes'  onclick='return PlayListHandler();' > <input type=reset value=Cancel id=reset1 name=reset1>", true);
        cfg.skipRow();
/*
	cfg.skipArea();
        cfg.putTag("</form>");

        cfg.putSubtitle("Screen refresh rate");
        cfg.skipRow();

	cfg.putTag("<form name=refresh action=wlan_disp.cgi method=post enctype=multipart/form-data>");

	cfg.actionRow("Refresh rate for 1024x768", cfg.getIValue( "<input type=text class=f1 name=ref1024 size=4 value=%d  onChange=ValidateFrequency(document.refresh.ref1024.value)> <font class=f0> Normal monitor using refresh rate (60 - 70)</font>", getRefreshRate(2)));
	cfg.dispRow("&nbsp;", "Select the screen refresh rate closest to one       available for your monitor or projector.  Or, choose the highest rate         that renders good screen image on your monitor or projector.");

        cfg.skipRow();

        cfg.actionRow("Refresh rate for 800x600", cfg.getIValue( "<input type=text class=f1 name=ref800 size=4 value=%d  onChange=ValidateFrequency(document.refresh.ref800.value)> <font class=f0> Normal monitor using refresh rate (60 - 70)</font>", getRefreshRate(1)));
        cfg.dispRow("&nbsp;", "Select the screen refresh rate closest to one    available for your monitor or projector.  Or, choose the highest rate  that renders good screen image on your monitor or projector.");

        cfg.skipRow();

        cfg.actionRow("Refresh rate for 640x480", cfg.getIValue( "<input type=text class=f1 name=ref640 size=4 value=%d onChange=ValidateFrequency(document.refresh.ref640.value)> <font class=f0> Normal monitor using refresh rate (60 - 70)</font>", getRefreshRate(0)));
        cfg.dispRow("&nbsp;", "Select the screen refresh rate closest to one    available for your monitor or projector.  Or, choose the highest rate  that renders good screen image on your monitor or projector.");

        cfg.skipRow();

        cfg.actionRow("Apply settings", "<input type=submit name=applyRefresh value=Apply > <input type=reset value=Cancel id=reset2 name=reset2>");
        cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");

*/
	cfg.putTag("</form>");
	cfg.endPage();
	return 0;
}

int AddFilesToPlayList(LdFlist &flist)
{
  bool added = false;
  char buffer[1024];
  int size, got;
  int totalSize = 0;
  int fd;
  int dur = 10;
  int ord = 1;
  int i, j, space = 0;

  char file[]     = "file0";     // file name element: file(0)-file(max_files_to_add-1)
  char local[]    = "local0";    // client-side file name element
  char folder[]   = "folder0";   // folder name element
  char order[]    = "order0";    // playing order
  char duration[] = "duration0"; // playing duration
  char localname[102];          // file name on the client side
  char foldername[102];         // folder to be stored for the uploaded file
  char filename[102];           // name to be used for the uploaded file
  char fullname[102];           // real full path file name used on the server side
  cgiFilePtr plocal;             // file pointer
  char command[204];

  j = flist.getCount();
  for (i = 0; i < j; i++) {
	space += flist.getSize()/1024;
	flist.nextFile();
  }
  space = MAX_FILE_SPACE - space;
  for(i = 0; i < MAX_FILES_TO_ADD; i++) {
    // handle ith uploaded file
    local[5]    = i + '0';
    folder[6]   = i + '0';
    file[4]     = i + '0';
    duration[8] = i + '0';
    order[5]    = i + '0';
    totalSize = 0;

    // does this ith uploaded file exist?
    if ((cgiFormFileName(local, localname, sizeof(localname)) != cgiFormSuccess) ||
        (cgiFormFileSize(local, &size) != cgiFormSuccess || size <= 0) ||
        (cgiFormFileOpen(local, &plocal) != cgiFormSuccess)) {
      continue;
    }

    // where is this uploaded file to be stored on the dom?
    if ((cgiFormStringNoNewlines(folder, foldername, sizeof(foldername)) != cgiFormSuccess)
        || (cgiFormStringNoNewlines(file, filename, sizeof(filename)) != cgiFormSuccess)
        || (cgiFormIntegerBounded(duration, &dur, -2, 24 * 3600, -2) != cgiFormSuccess)
        || (cgiFormIntegerBounded(order, &ord, -1, MAX_NUM_FILES_PLAY_LIST, -1) != cgiFormSuccess)
        ) {
      cgiFormFileClose(plocal);
      return -1;
    }
// ??    GenerateFullFileName(foldername, filename, fullname);
     sprintf(fullname, "/banners/images/%s", filename);
    // persistantly save the uploaded file on the dom
    if ((fd = creat(fullname, S_IRWXU)) <= 0) {
      cgiFormFileClose(plocal);
      return -2;
    }
    while (cgiFormFileRead(plocal, buffer, sizeof(buffer), &got) == cgiFormSuccess) {
      write(fd, buffer, got);
      totalSize += got;
    }
    if (totalSize != size) {
      cgiFormFileClose(plocal);
      return -3;
    }
    space = space - totalSize/1024;
    if (space < 0) {
      cgiFormFileClose(plocal);
      close(fd);
      // delete file if necessary
      sprintf(command, "rm -f %s; sync", fullname);
      system(command);
      break;
    }
    cgiFormFileClose(plocal);
    close(fd);
    
//??    GetVideoFileType(fullname, &type); // guess the video clip type
    flist.instFile(fullname, "jpg", dur, 1, ord);
    added = true;
  }
  if (added) flist.saveList(BANNER_LIST);
  return 0;
}

int UpdateCurrentPlayList()
{
  LdFlist delFlist, newFlist;
  int i, j;
  int dur = 10;
  int ord = 1;
  char corder[]    = "corder0";
  char cduration[] = "cduration0";
  char cdelete[]   = "cdelete0";
  char command[204];

  j = flist.getCount();
  for(i = 0; i < j; i++) {
    sprintf(corder, "corder%d", i);
    sprintf(cdelete, "cdelete%d", i);
    sprintf(cduration, "cduration%d", i);
    if (cgiFormCheckboxSingle(cdelete) == cgiFormSuccess) { // remove this file
        delFlist.addFile(flist.getFilename(false), flist.getType(), flist.getDuration(), flist.getPlayable(), flist.getOrder());
    }
    else {
        int duration = flist.getDuration();
        int order = flist.getOrder();
        if ((cgiFormInteger(cduration, &dur, 1) == cgiFormSuccess) &&
                (cgiFormInteger(corder, &ord, 10) == cgiFormSuccess)) {
                duration = dur;
                order    = ord;
        }
        newFlist.instFile(flist.getFilename(false), flist.getType(), duration, flist.getPlayable(), order);
    }
    flist.nextFile();
  }
  j = delFlist.getCount();
  delFlist.setHead();
  for (i = 0; i < j; i++) {
        sprintf(command, "rm -f %s; sync", delFlist.getFilename(false));
        system(command);
        delFlist.nextFile();
  }
#ifdef DEBUG
  if(j) delFlist.saveList("zzz");       // debug only
#endif
// replace on original banners list then reload to flist
  newFlist.saveList(BANNER_LIST);
  j = AddFilesToPlayList(newFlist);
  flist.cleanList();
  flist.loadList(BANNER_LIST);
  return j;
}

void UpdateRefresg()
{
	int r1024, r800, r640, i;

	cgiFormInteger("ref1024", &r1024, 68);
	cgiFormInteger("ref800", &r800, 62);
	cgiFormInteger("ref640", &r640, 64);
	i = rlist.getIndex(DISP1024);
	if (i != -1) rlist.setValue(i, r1024);
	i = rlist.getIndex(DISP800);
	if (i != -1) rlist.setValue(i, r800);
	i = rlist.getIndex(DISP640);
	if (i != -1) rlist.setValue(i, r640);
	rlist.saveToFile(REFRESH_CONFIG);
	// reqiure send update display message to watch dog !!!
}

int HandleUpload(void) {
  if (cgiFormSubmitClicked("play") == cgiFormSuccess) {  // playlist
     	if (UpdateCurrentPlayList() < 0) return -1;
  }
  if (cgiFormSubmitClicked("applyRefresh") == cgiFormSuccess) { // refresh 
    	UpdateRefresg();
  }
  return 0;
}

int cgiMain(void) {
  rlist.loadFromFile(REFRESH_CONFIG);
  flist.loadList(BANNER_LIST);
  if (HandleUpload() == 0)
	showPage();
  else
  	ShowFailureMessage();
  return 0;
}

