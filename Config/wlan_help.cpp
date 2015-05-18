#include "ConfigCommon.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PRODUCT "PRODUCT"
#define VERSION	"FIRMWARE_VER"
#define DATE	"VER_DATE"
#define REGION	"REGION"
#define WEBSITE "TS_WEB_SITE"

LdProp  vlist;
ConfigCommon cfg;

int cgiMain() {
	vlist.loadFromFile(VERSION_CONF);
	int r = vlist.getIntValue(REGION);
	cfg.putHeader("Pegasus Wireless CYNALYNX - Help");
	cfg.startPage("Help","Thank you for choosing Pegasus Wireless product");
	cfg.putSubtitle("Product");
	cfg.dispRow("Name", vlist.getValue(PRODUCT));
	cfg.dispRow("Firmware version", vlist.getValue(VERSION));
	cfg.dispRow("Region", r==0?(char*)"Europe":r==1?(char*)"Janpan":(char*)"USA");
	cfg.dispRow("Date", vlist.getValue(DATE));
	cfg.skipRow();
	cfg.putSubtitle("Tech support");
	cfg.dispRow("Getting Help", "For technical assistance with your product, please contact our Technical Support Department.<br>Visit our web site for additional resources, new product information, updates and file downloads.");
	cfg.dispRow("Web site", vlist.getValue(WEBSITE));
	cfg.skipRow();
        cfg.putSubtitle("Download");
	cfg.dispRow("<a href=download/wijetpsm.exe>Download PSM</a>", "Download the Presentation Session Manager software.");
	cfg.endPage();

	return 0;
}

