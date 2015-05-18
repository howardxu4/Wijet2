#include "ConfigCommon.h"

ConfigCommon::ConfigCommon() {}
ConfigCommon::~ConfigCommon() {}

void ConfigCommon::runScript(char *func)
{
	fprintf(cgiOut, "<script type=text/javascript>");
	fprintf(cgiOut, func);
	fprintf(cgiOut, ";</script>\n"); 
}

void ConfigCommon::putHeader(char* title)
{
        /* Send the content type, letting the browser know this is HTML */
        cgiHeaderContentType("text/html");

	fprintf(cgiOut, "<html><head>\n");
	fprintf(cgiOut, "<meta http-equiv=\"pragma\" content=\"no-cache\">\n");
	fprintf(cgiOut, "<meta http-equiv=\"expires\" content=\"-1\">\n");
	fprintf(cgiOut, "<title>%s</title>\n", title);
        fprintf(cgiOut, "<link rel=stylesheet type=text/css href=styles/style.css>\n");
	putScript("common.js");
}

void ConfigCommon::putScript(char* script)
{
	fprintf(cgiOut, "<script type=text/javascript src=\"scripts/");
	fprintf(cgiOut, script);
	fprintf(cgiOut, "\"></script>\n");
}

void ConfigCommon::writeHeader(char* name, char *descript)
{
        fprintf(cgiOut, "<br><script type=text/javascript>putHeader( \"");
        fprintf(cgiOut, name);
        fprintf(cgiOut, "\", \"");
        fprintf(cgiOut, descript);
        fprintf(cgiOut, "\" );  </script>\n");
}

void ConfigCommon::startArea()
{
        fprintf(cgiOut, " <script type=text/javascript> startArea(); </script>\n");
        fprintf(cgiOut, "<table width=90%% class=fb border=0 cellspacing=0 cellpadding=1 align=center>\n\n");
}

void ConfigCommon::endArea()
{
        fprintf(cgiOut, "</table>");
        fprintf(cgiOut, "<script type=text/javascript> endArea(); putFooter(); </script>\n");
	fprintf(cgiOut, "<br>");
}

void ConfigCommon::skipArea()
{
	fprintf(cgiOut, "</table>");
	fprintf(cgiOut, "<script type=text/javascript> skipArea();</script>\n");
	fprintf(cgiOut,  "<table width=90%% class=fb border=0 cellspacing=0 cellpadding=1 align=center>\n\n");
}

void ConfigCommon::startPage(char* name, char *descript)
{
	fprintf(cgiOut, "</head>\n\n");
	fprintf(cgiOut, "<body bgColor=#eeeee3 leftMargin=0 topMargin=0 marginwidth=0 marginheight=0>\n");
	writeHeader(name, descript);
	startArea();
}

void ConfigCommon::putSubtitle(char* subtitle)
{
	fprintf(cgiOut, "<script type=text/javascript>putSubtitle(\"");
	fprintf(cgiOut, subtitle);
	fprintf(cgiOut, "\" ); </script>\n\n");
}

void ConfigCommon::skipRow()
{
	fprintf(cgiOut, "<tr><td>&nbsp;</td><td>&nbsp;</td></tr>\n\n");
}

void ConfigCommon::dispRow(char* name, char* value, bool bold)
{
	fprintf(cgiOut, "<tr><td valign=top width=30%%><font class=f1 color='blue'>");
	fprintf(cgiOut, name);
	fprintf(cgiOut, "</font></td>\n<td><font class=f0>");
	if (bold) fprintf(cgiOut, "<b>");
	fprintf(cgiOut, value);
	fprintf(cgiOut, "</font></td></tr>\n\n");
}

void  ConfigCommon::actionRow(char* name, char* action, bool right)
{
        fprintf(cgiOut, "<tr><td valign=top width=30%%><font class=f1 color='blue'>");
        fprintf(cgiOut, name);
	if (right)
		fprintf(cgiOut, "</font></td>\n<td align=right>");
	else
        	fprintf(cgiOut, "</font></td>\n<td>");
        fprintf(cgiOut, action);
        fprintf(cgiOut, "</td></tr>\n\n");
}

void ConfigCommon::endPage()
{
	endArea();
	fprintf(cgiOut, "</body></html>\n");
}

char* ConfigCommon::getSelect(char* name, int size, char* array[], int sel, char* opt)
{
	char temp[60];
	int i;
	sprintf(m_buffer, "<select class=f1 name='%s' size=1 %s>\n", name, opt);
	for (i = 0; i < size && array[i] != NULL; i++) {
		sprintf(temp, " <option %s>%s</option>\n", i == sel? "selected":" ", array[i]);
		strcat(m_buffer, temp);
	}
	strcat(m_buffer, "</select>\n");
	return m_buffer;
}

void ConfigCommon::startTable(char* name, int size, char* headers[])
{
	int i=1;
        fprintf(cgiOut, "<tr>");
	if (name != NULL && name[0] != 0) {
	fprintf(cgiOut, "<td valign=top width=30%%><font class=f1 color='blue'>");
        fprintf(cgiOut, name);
        fprintf(cgiOut, "</font></td>\n");
	i = 0;
	}
	if (i == 1) 
	fprintf(cgiOut,"<td colspan=2><table align=center cellspacing=1 cellpadding=1 border=1 class=th>\n<tr>");
	else
	fprintf(cgiOut,"<td><table align=left cellspacing=0 cellpadding=0 border=0 class=th>\n<tr>");
	for (i = 0; i < size && headers[i] != NULL; i++) 
		fprintf(cgiOut, "  <th>%s</th>\n", headers[i]);
	fprintf(cgiOut, "</tr>\n");
}

void ConfigCommon::endTable()
{
	fprintf(cgiOut,"</td></table>  </tr>\n");
}

void ConfigCommon::putFmtTag(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(cgiOut, fmt, ap);
	va_end(ap);
}

char* ConfigCommon::convertS(int n)
{
	sprintf(m_conv, "%d", n);
	return (m_conv);
}
