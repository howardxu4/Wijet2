#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;

#define VDEVICE		"VDEVICE"
#define TVMODE		"TVMODE"
#define VOUTMODE 	"VOUTMODE"
#define BANNERSHOW 	"BNSHOW"
#define AIPENABLED	"AIPENABLED"
#define COLORDEPTH	"COLORDEPTH"
#define AUDIOTYPE	"AUDIOTYPE"

char* aVdevices[] = {
		"Auto",
		"CRT",
		"TV",
		NULL
};
char* aTVformats[] = {
		"NTSC",
                "PAL",
                NULL
};
char* aVOutformats[] = {
                "Component",
                "Composite/S-video",
                NULL
};
char* aColorformats[] = {
		"Medium (16 bit)",
		"Highest (24 bit)",
		NULL
};
char* aAudioformats[] = {
		"Stereo 2.0",
		"Stereo 2.1",
		"Surround 5.0",
		"Surround 5.1",
		"Pass Through",
		NULL
};

void showPage() 
{
	const char*pb, *pa;
	pb = clist.getIntValue(BANNERSHOW) ? "checked" : " ";
	pa = clist.getIntValue(AIPENABLED) ? "checked" : " ";	

	cfg.putHeader("Pegasus Wireless CYNALYNX - Advanced");
	cfg.startPage("Advanced", "The settings on this page are used to set the advance controls.");
	cfg.putTag("<form name=wlan_advc action=wlan_advc.cgi method=post enctype=multipart/form-data>");

	cfg.putSubtitle("Advanced Configuration settings");
        cfg.skipRow();
	cfg.actionRow("Video output device", cfg.getSelect("vdevices", 3, aVdevices, clist.getIntValue(VDEVICE), ""));
        cfg.dispRow("&nbsp;", "This control can be used to turn off a video port that will not be used.");
        cfg.skipRow();
	cfg.actionRow("TV format", cfg.getSelect("tvmode", 2, aTVformats, clist.getIntValue(TVMODE), ""));
	cfg.dispRow("&nbsp;", "This control selects the correct TV format to match the required input of your television set.");
        cfg.skipRow();
//	cfg.actionRow("Video output format", cfg.getSelect("voutmode", 2, aVOutformats, clist.getIntValue(VOUTMODE), ""));
//	cfg.dispRow("&nbsp;", "Choose the video output format to match the target video input.");
//	cfg.skipRow();
	cfg.actionRow("Audio output option", cfg.getSelect("audiotype", 5, aAudioformats, clist.getIntValue(AUDIOTYPE), ""));
	cfg.dispRow("&nbsp;", "This control affects how the CYNALYNX will handle audio signals.");
	cfg.skipRow();
	cfg.actionRow("Color Quality", cfg.getSelect("colordepth", 2, aColorformats, clist.getIntValue(COLORDEPTH), ""));
	cfg.dispRow("&nbsp;", "This control affects the color depth supported. Setting this field to a high value can adversely affect slower systems.");
        cfg.skipRow();
	cfg.actionRow("Banner information enabled", cfg.getFValue("<input type=checkbox name=hinfoEnabled %s>", (char*)pb)); 
	cfg.dispRow("&nbsp;", "Use this control to enable or display the start up page system status reports.");
	cfg.skipRow();
	cfg.actionRow("Auto IP enabled", cfg.getFValue("<input type=checkbox name=aipmode value=Automatic  %s>", (char*)pa));
	cfg.dispRow("&nbsp;", "If enabled, the CYNALYNX is permitted to change its IP address to match the IP address of the computer initiating a session if this is necessary to allow TCP/IP communications."); 
        cfg.skipRow();
        cfg.actionRow("Apply settings", "<input type=submit name=applyAdvConf value=Apply > <input type=reset value=Cancel id=reset1 name=reset1>");
	cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

	cfg.putTag("</form>");
	cfg.endPage();
}

void HandleUpdate()
{
	int i = 0, n = 0;
    	if (cgiFormCheckboxSingle("hinfoEnabled") == cgiFormSuccess) 
      		i = 1;
	if (clist.getIntValue(BANNERSHOW) != i) {
		n += 1;
		clist.setValue(BANNERSHOW, i);
	}
	i = 0;
	if (cgiFormCheckboxSingle("aipmode") == cgiFormSuccess)
		i = 1;
	if (clist.getIntValue(AIPENABLED) != i) {
		n += 2;
		clist.setValue(AIPENABLED, i);
	}
        i = 0;
        if (cgiFormRadio("vdevices", aVdevices, 3, &i, 0) == cgiFormSuccess) {
		if (clist.getIntValue(VDEVICE) != i) {
			n |= 4;
                	clist.setValue(VDEVICE, i);
		}
	}	
	i = 0;
	if (cgiFormRadio("tvmode", aTVformats, 2, &i, 0) == cgiFormSuccess) {
		if (clist.getIntValue(TVMODE) != i) {
			n |= 4;
			clist.setValue(TVMODE, i);
		}
	}
	i = 0;
        if (cgiFormRadio("voutmode", aVOutformats, 2, &i, 0) == cgiFormSuccess)
        	clist.setValue(VOUTMODE, i);

	i = 0;
	if (cgiFormRadio("colordepth", aColorformats, 2, &i, 0) == cgiFormSuccess) {
		if (clist.getIntValue(COLORDEPTH) != i) {
			n |= 4;
			clist.setValue(COLORDEPTH, i);
		}
	}
	i = 0;
	if (cgiFormSelectSingle("audiotype", aAudioformats, 5, &i, 0)  == cgiFormSuccess) {
		if (clist.getIntValue(AUDIOTYPE) != i) {
			n |= 8;
			clist.setValue(AUDIOTYPE, i);
		}
	}

	clist.saveToFile(OTC_CONFIG);
	// inform watch dog for changing
	if (n != 0) tool.Changes(tool.ADVNC, n);
}

int cgiMain()
{
        clist.loadFromFile(OTC_CONFIG);
	if (cgiFormSubmitClicked("applyAdvConf") == cgiFormSuccess) 
		HandleUpdate();
	showPage();
	return 0;
}
