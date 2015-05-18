#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;

#define USERID		"USER"
#define PASSWD		"PASSWORD"

void showPage() 
{
	cfg.putHeader("Pegasus Wireless CYNALYNX -  Administration");
	cfg.putScript("validate.js");
	cfg.startPage(" Administration","This page contains controls to reboot the CYNALYNX unit, reset it back to factory default settings, upload firmware and change administrative access settings.");
	cfg.putTag("<form name=wlan_admin action=wlan_admin.cgi method=post enctype=multipart/form-data>");

	cfg.putSubtitle("Device Control");
        cfg.skipRow();
	cfg.actionRow("Reboot the unit", "<input type=submit name=reboot value='Proceed with reboot' onClick='return Reboot();'>");
	cfg.dispRow("&nbsp;", "Rebooting the unit will apply all saved changes.");
	cfg.skipRow();
	cfg.actionRow("Reset to factory default", "<input type=submit name='resettodefault' value='Reset to factory default' onClick='return Reset();'>");
	cfg.dispRow("&nbsp;", "Click to restore factory defaults.");
        cfg.skipRow();

	cfg.skipArea();

	cfg.putSubtitle("Firmware upload");
        cfg.skipRow();
	cfg.actionRow("Select firmware", "<input type=file onchange='return ValidatePatch();'  name=firmware>");
	cfg.dispRow("&nbsp;", "Use this button to select firmware patches to upload to the CYNALYNX. Only official patches issued by Pegasus Wireless Corporation should be used.");
        cfg.skipRow();
	cfg.actionRow("Upload firmware", "<input type=submit onclick='return PatchHandler();' value='Upload firmware' name=patch> <input type=reset value=Cancel id=reset1 name=reset1>"); 
	cfg.skipRow();

        cfg.skipArea();

        cfg.putSubtitle("User account settings");
        cfg.actionRow("User name", cfg.getFValue("<input type=text name=uname maxlength=16 size=18 value='%s'>", clist.getValue(USERID)));
	cfg.dispRow("&nbsp;", "This field allows the login account to be changed. The factory default account name is  &quot;admin&quot;. ");
        cfg.skipRow();
        cfg.actionRow("Enter new password", "<input type=password name=npword maxlength=16 size=18 value=''>");
	cfg.dispRow("&nbsp;", "This field allows the access password to be changed. The factory default password is&quot;public&quot;.");
        cfg.skipRow();
        cfg.actionRow("Confirm new password", "<input type=password name=cpword maxlength=16 size=18 value=''>");
	cfg.dispRow("&nbsp;", "Confirm the new password.");
        cfg.skipRow();
        cfg.actionRow("Apply account settings", "<input type=submit name=applyuseraccounts value=Apply onClick='return Validate();'> <input type=reset name=canceluseraccounts value=Cancel>");
        cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new user account settings. Click &quot;Cancel&quot; to abort changes.");

	cfg.putTag("</form>");
	cfg.endPage();
}

void ShowFirmwareUploadFailureMessage(int n)
{         
	cfg.putHeader("Pegasus Wireless CYNALYNX - Failure Message");
        cfg.startPage("Failure Message", "This page contains the possible failure reasons during uploading firmware.");
        cfg.dispRow("<font class=f1 color=red>Firmware upload failure</font>", "&nbsp;");
        cfg.dispRow("&nbsp;", "Several factors may cause the failure:<br>1. Wrong patch script file format or name.</br>2. No enough space available to store the patch file.</br> 3. Data transfer timedout due to fair or bad wireless connection.</br><br>Select the correct upload file.  Try the upload again.");
 	cfg.putTag("<form name=wlan_admin action=wlan_admin.cgi method=post enctype=multipart/form-data>");

	cfg.skipRow();
	cfg.actionRow("&nbsp;", "<input type=submit name=continue value=Continue>");

	cfg.putTag("</form>");
        cfg.endPage();
}

void  SetUserAccounts()
{
#define MAXLENGTH               17
	char username[MAXLENGTH+1];
	char newpassword[MAXLENGTH+1];
	char confirmpassword[MAXLENGTH+1];
        char            command[1024];
        FILE            *temp = NULL;

        cgiFormStringNoNewlines("uname", username, MAXLENGTH);
        cgiFormStringNoNewlines("npword", newpassword, MAXLENGTH);
        cgiFormStringNoNewlines("cpword", confirmpassword, MAXLENGTH);

// this check was done on web page client-side validation already
	if (strcmp(newpassword, confirmpassword) == 0) {
          temp = fopen(TEMP_PASSWD, "wr");
          fprintf(temp, "%s\n", newpassword);
          fprintf(temp, "%s\n", newpassword);
          fclose(temp);
	  clist.setValue(USERID, username);
	  clist.setValue(PASSWD, newpassword);
	  clist.saveToFile(OTC_CONFIG);
          sprintf(command, "htpasswd -c %s %s > %s < %s", HTPASS_FILE, username, TEMP_FILE, TEMP_PASSWD);

          system(command);
          sprintf(command, "rm -f %s", TEMP_FILE);
          system(command);
          sprintf(command, "rm -f %s; sync", TEMP_PASSWD);
          system(command);
	}
}

int UploadFirmware()
{
  char command[1024];
  char buffer[1024];
  int size, got;
  int totalSize = 0;
  int fd;
  cgiFilePtr file;
  char firmware[] = "firmware";
  char filename[1024];
    if ((cgiFormFileName(firmware, filename, sizeof(filename)) != cgiFormSuccess) ||
        (cgiFormFileSize(firmware, &size) != cgiFormSuccess || size <= 0) ||
        (cgiFormFileOpen(firmware, &file) != cgiFormSuccess)) {
      return -1;
    }

    if ((fd = creat(TEMP_FIRMWARE_FILE, S_IRWXU)) <= 0) {
      return -1;
    }

    while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) == cgiFormSuccess) {
      write(fd, buffer, got);
      totalSize += got;
    }
    if (totalSize != size) {
      return -1;
    }
    cgiFormFileClose(file);
    close(fd);

    //
    // Extract the script file "patch-script" from uploaded file.
    //
    fd = -1;
    {
	LdMagic mgc;
	if (mgc.setTarFile(TEMP_FIRMWARE_FILE)) {
		got = mgc.checkMagic();
		if (got >= 0) fd = 0;
	}
    }
    if (fd == -1) {
	sprintf(command, "sync; cd %s; rm -fr %s", TEMP_DIR, TEMP_FIRMWARE_FILE);
        system(command);
	return -1;
    }

    //
    // tell Watchdog to handle it
    //
    tool.Changes(tool.ULOAD, got);

  return 0;
}

int HandleUpdate()
{
  if (cgiFormSubmitClicked("reboot") == cgiFormSuccess) {  
	tool.Changes(tool.ADMIN, 0);
  }
  if (cgiFormSubmitClicked("resettodefault") == cgiFormSuccess) {
	tool.Changes(tool.ADMIN, 1);
  }
  if (cgiFormSubmitClicked("patch") == cgiFormSuccess) {
	return UploadFirmware();
  }
  if (cgiFormSubmitClicked("applyuseraccounts") == cgiFormSuccess) {  
	SetUserAccounts();
  }
  return 0;
} 

int cgiMain() 
{
	clist.noComment(USERID);
	clist.noComment(PASSWD);
        clist.loadFromFile(OTC_CONFIG);
	int code = HandleUpdate();
	if (code == 0)
		showPage();
	else 
		ShowFirmwareUploadFailureMessage(code);
	return 0;	
}
