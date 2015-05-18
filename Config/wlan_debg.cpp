#include "ConfigCommon.h"
#include "ConfigTools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ConfigCommon cfg;
ConfigTools tool;
LdProp  clist;

// DE 2: IN 4: DV 16: WN 32: ER 64
// ST 1: AI 2: MD 4:  WD 8

#define DBGLVL          "DEBUG"
#define MODULES		"MODULES"

char* tHeader[] = {
		" Debug",
		" Info ",
		" Dvlp ",
		" Warn ",
		" Error",
		NULL
};

char* aDebugLevel[] = {
                "State&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;",
		"AutoIP&nbsp;&nbsp;&nbsp;&nbsp;",
		"MdPlayer&nbsp;&nbsp;",
		"WatchDog",
                NULL
}; 

char* pd(int on)
{
	return (on?(char*)"checked":(char*)"");
}

void showPage()
{
	char SNumber[10];
	char BTimes[20];
	int  l, m;
	l = clist.getIntValue(DBGLVL);
	m = clist.getIntValue(MODULES);
	
	tool.InitSB(SNumber, BTimes);
        cfg.putHeader("Pegasus Wireless CYNALYNX - Debug");
        cfg.startPage("Internal Use", "On this page you can set the burning test and system debug.");

        cfg.putTag("<form name=wlan_debug action=wlan_debg.cgi method=post enctype=multipart/form-data>");
	cfg.putSubtitle("Burning test");
	cfg.skipRow();
	cfg.dispRow("Series Number", SNumber);
	cfg.dispRow("Burning Times", BTimes);
	cfg.skipRow();
	cfg.actionRow("Burning test Enabled", "<input type=submit  name=burn value=Start > <input type=submit  name=refresh value=Refresh >");
	cfg.dispRow("&nbsp;", "Click &quot;Start&quot; to start the burning test record.  Click &quot;Refresh&quot; to update burning time showing.");
	cfg.skipRow();

        cfg.skipArea();
        cfg.putSubtitle("Debug settings");

        cfg.skipRow();
	cfg.startTable("Debug Level setting", 5, tHeader);
	cfg.putFmtTag("<tr><td><input type=checkbox name=dbg_debug  value=1  %s>", (char*)pd(l&2));
	cfg.putFmtTag("<td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=checkbox name=dbg_info  value=2  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(l&4));
	cfg.putFmtTag("<td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=checkbox name=dbg_dvlp  value=3  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(l&16));
	cfg.putFmtTag("<td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=checkbox name=dbg_warn  value=4  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(l&32));
	cfg.putFmtTag("<td>&nbsp;&nbsp;&nbsp;&nbsp;<input type=checkbox name=dbg_error  value=5  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(l&64));
	cfg.endTable();
	cfg.dispRow("&nbsp;", "Choose the debug level for output information.");

	cfg.skipRow();
	cfg.startTable("Debug Module", 4, aDebugLevel);
	cfg.putFmtTag("<td><input type=checkbox name=dbg_st  value=2  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(m&1));
	cfg.putFmtTag("<td><input type=checkbox name=dbg_ai  value=3  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(m&2));
	cfg.putFmtTag("<td><input type=checkbox name=dbg_md  value=4  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(m&4));
	cfg.putFmtTag("<td><input type=checkbox name=dbg_wd  value=1  %s>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;", (char*)pd(m&8));
	cfg.endTable();	

	cfg.dispRow("&nbsp;", "Choose the Module for debug.");

        cfg.skipRow();
        cfg.actionRow("Clean Debug Enabled", cfg.getFValue("<input type=checkbox name=cleandb value=CleanDebug >", (char*)""));
        cfg.dispRow("&nbsp;", "CYNALYNX will clean all previours debug information.");
	cfg.skipRow();
        cfg.actionRow("Apply settings", "<input type=submit name=applyDebug value=Apply > <input type=reset value=Cancel id=reset1 name=reset1>");
        cfg.dispRow("&nbsp;", "Click &quot;Apply&quot; to save the new settings. Click &quot;Cancel&quot; to abort changes.");
        cfg.skipRow();

        cfg.putTag("</form>");
        cfg.endPage();
}

int cgiMain()
{
	int i = 0;
        clist.loadFromFile(OTC_CONFIG);
	if (cgiFormSubmitClicked("burn") == cgiFormSuccess) {
		// inform watch dog for changing
		i = 1;
	}
        else if (cgiFormSubmitClicked("applyDebug") == cgiFormSuccess) {
		int m;
		m = 0;
		if (cgiFormCheckboxSingle("dbg_debug") == cgiFormSuccess)
			m |= 2;
		if (cgiFormCheckboxSingle("dbg_info") == cgiFormSuccess)
                        m |= 4;
		if (cgiFormCheckboxSingle("dbg_dvlp") == cgiFormSuccess)
                        m |= 16;
		if (cgiFormCheckboxSingle("dbg_warn") == cgiFormSuccess)
                        m |= 32;
		if (cgiFormCheckboxSingle("dbg_error") == cgiFormSuccess)
                        m |= 64;
		if (clist.getIntValue(DBGLVL) != m) {
			clist.setValue(DBGLVL, m);
			i = 2;
		}
		m = 0;
		if (cgiFormCheckboxSingle("dbg_st") == cgiFormSuccess)
                        m |= 1;
		if (cgiFormCheckboxSingle("dbg_ai") == cgiFormSuccess)
                        m |= 2;
		if (cgiFormCheckboxSingle("dbg_md") == cgiFormSuccess)
                        m |= 4;
		if (cgiFormCheckboxSingle("dbg_wd") == cgiFormSuccess)
                        m |= 8;
		if (clist.getIntValue(MODULES) != m) {
                        clist.setValue(MODULES, m);
                        i |= 4;
                }
		if (i != 0) clist.saveToFile(OTC_CONFIG);
		if (cgiFormCheckboxSingle("cleandb") == cgiFormSuccess)
                        i |= 8;
        }
	if (i != 0) tool.Changes(tool.TESTD, i);
        showPage();
        return 0;
}
