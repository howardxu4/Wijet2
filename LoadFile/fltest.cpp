#include "LdFlist.h"
#include "LdProp.h"
#include "LdEnv.h"
#include "getProperty.h"
#include "LdConf.h"
#include "LdMagic.h"

void DEBUG (char *ui) { fprintf (stderr, ui); fflush(stderr);}

#define TEST

#ifdef TEST
int main (int argc, char **argv)
{
#define BANNER_LIST 	"/etc/banners.txt"
#define WLAN_CONFIG	"/etc/wlan.conf"
#define CERTIFICATES	"/etc/certificates.txt"
#define HOSTAPD_CONF    "/etc/hostapd.conf"
#define XORG_CONF	"/etc/X11/xorg.conf"
#define OTC_DEF		"/etc/otc_defaults.conf"

        char          display[128];
        LdFlist flist;
	LdProp  plist;
	LdConf  clist;

	getProperty gP;
	
        int     count = gP.check();
	
	if (count != 0) {
		printf("Missing file %d for testing!!!!\n", count);
		exit (-1);
	}

	count = 1;
        if (flist.loadList(BANNER_LIST) == 0){
                printf("error in file list %s\n", BANNER_LIST);
                return -1;
        }
DEBUG ("open file OK\n");

	flist.instFile("/hxu/myTest/banner.jpg", "JPG", 25, 0, 20);
        flist.instFile("/hxu/myTest/spring.jpg", "JPG", 14, 1, 8);
        flist.instFile("/hxu/myTest/autumn.jpg", "JPG", 15, 0, 15);
        flist.instFile("/hxu/myTest/untitled.jpg", "JPG", 13, 1, 21);
        flist.instFile("/hxu/myTest/winter.jpg", "JPG", 25, 0, 18);
        flist.instFile("/hxu/myTest/summer.jpg", "JPG", 7, 1, 10);

	flist.loadList("myflist.txt");

	flist.setHead();
        count = flist.getDuration();
	strcpy(display, flist.getFilename());
	
        while (1) {
        printf("%s %s [%d] %d %s (%d) {%d}\n", flist.getFilename(), flist.getType(), flist.getPlayable(), count, flist.getFilename(true), flist.getOrder(), flist.getSize());

//	No circle on list		
                if(flist.nextFile(false) > 0) {
                	count = flist.getDuration();
//			if (strcmp(display, flist.getFilename()) == 0)
//				break;
		}
		else break;
        }
//	flist.saveList("gggg");

printf("\n****************\n");
	gP.dump();


printf("\n******** otc_default.conf ********\n");
	gP.init(gP.P_OTC);
	gP.dump();


printf("\n******** wlan.conf ********\n");
        gP.init(gP.P_WLAN);
        gP.dump();
	
	printf(" REGION = %d\n", gP.getInt("region"));
	printf(" SSID = %s\n", gP.getStr("SSID"));

	plist.init(20);
	plist.loadFromFile(OTC_DEF);
	printf(" USER = [%s]\n", plist.getValue(plist.getIndex("USER")));
	
//  plist.saveToFile("lolo");

printf("\n****************\n");
        plist.init(20);
	plist.noComment("username");
	plist.noComment("SSID");
	plist.noComment("ipPnPmode");
	plist.noComment("password");
        count = plist.loadFromFile(WLAN_CONFIG);

	printf(" region = %d\n",plist.getIntValue(plist.getIndex("region")));
	printf(" SSID = %s\n", plist.getValue(plist.getIndex("SSID")));
	printf(" username = %s\n",plist.getValue(plist.getIndex("username")));
	printf(" password = %s\n",plist.getValue(plist.getIndex("password")));
	printf(" ipPnPmode = %s\n", plist.getValue(plist.getIndex("ipPnPmode")));
	printf(" CHANNEL = %d\n", plist.getIntValue(plist.getIndex("CHANNEL")));
//	plist.setValue("ipPnPmode", "");
//	 printf(" ipPnPmode = %s\n", plist.getValue(plist.getIndex("ipPnPmode")));
	
//  plist.saveToFile("haha");

printf("\n******* refresh.conf ********\n");

        gP.init(gP.P_REFRESH);
        gP.dump();

printf("\n******* scan.txt ********\n");
	gP.init(gP.P_SCAN);
	gP.dump();

// test LdEnv class
printf("\n*********env file *********\n");
	LdEnv Env;
        printf("Path = %s\n", Env.getPath());
	printf("USB = %s\n", Env.getUSB());
	printf("MAP = %s\n", Env.getMapping());
	printf("Image = %s\n", Env.getImage());

	plist.init(10);
	plist.loadFromFile(CERTIFICATES);
	plist.saveToFile(CERTIFICATES);

// test LdMagic class
printf("\n********magic file********\n");
	LdMagic mgc;
	int i = mgc.saveToFile("patch.txt", LdMagic::U_SYS, "/tmp/upgrade");
	printf("save Magic file return %d\n", i);
	
	i = mgc.loadFromFile("patch.txt");
	printf("load Magic file return %d\n", i);
	printf("size = %d\n", mgc.getSize());
	printf("type = %d\n", mgc.getType());
	printf("cksum = %d\n", mgc.getCksum());
	printf("upgrade = %s\n", mgc.getUpgrade());

// Test conf
	clist.loadFromFile(XORG_CONF);
	clist.parSection();
	i = clist.getIndex("\"Device\"", OneLine::GSTR, 0);
	printf("[%d] %s", i, clist.getLine(i));
	i = clist.getIndex("ActiveDevice", OneLine::COMM, i);
	printf("[%d] %s", i, clist.getLine(i));
	clist.changeType(i);
	clist.saveToFile("xxx");
}
#else
int main (int argc, char **argv)
{
        LdConf	plist;
        int     i, count = 1;
	if (argc == 1) {
		printf("Usage: %s {input_file} [saved file|-pattern] [mask] [all]\n", argv[0]);
		printf(" e.g. %s /etc/dhcpd.cof -subnet 31 all\n", argv[0]);
		printf(" pattern:	1  Comment\n");
		printf("                2  Property\n");
		printf("                4  Gstart \n");
		printf("                8  Gend\n");
		printf("               16  Line\n");
		printf("               31  All\n");
		return 0;
	}
        count = plist.loadFromFile(argv[1]);
	if (count <= 0) {
		printf("error in file list %s\n", argv[1]);
		return -1;
	}
        for(i = 0; i < count; i++) {
		if (plist.getType(i) != OneLine::COMM)
		 printf("%s", plist.getLine(i));
	}
	if (argc > 2) {
		if (argv[2][0] == '-') {
			int mask = OneLine::MASK;
			if (argc > 3) mask = atoi(argv[3]);
        		i = plist.getIndex(&argv[2][1], mask);
        		printf( "index %d: %s", i, plist.getLine(i));
			if (argc > 4) {
				while (i != -1) {
				i =  plist.getIndex(&argv[2][1], mask, i+1);
				printf( "index %d: %s", i, plist.getLine(i));
				}
			}
	//		plist.chgLine(i, "ha ha ha");
	//		printf( "index %d: %s", i, plist.getLine(i));
		}
		else
			plist.saveToFile(argv[2]);
	}
	return 0;
}
#endif

