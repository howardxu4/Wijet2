

/**
 *      The startUp WLAN application
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#include "myWlan.h"

/**
 *      Initialize Wlan 
 *
 */

void initWlan(int n, getProperty &gPw)
{
        char cmd[80];
	int r;
	
	system("ifconfig ath0 down 2>/dev/null");
	system("rmmod ath_pci 2>/dev/null");
	
	if (n==1) {				// OLD CARD
		r = gPw.getInt(REGION);
        	r = r==0? 276: r==1? 392: 840;
	        sprintf(cmd, "modprobe ath_pci rfkill=0 countrycode=%d", r);
	}
	else 
		sprintf(cmd, "modprobe ath_pci ");
        system(cmd);
	sleep(2);

	r = gPw.getInt(STANDARD);
        sprintf(cmd, "iwpriv ath0 mode %d", r); // Auto 0: A:1 B:2 G:3
        system(cmd);
        system("ifconfig ath0 up");
//        sleep(1);
}

/**
 *      The main program
 *
 */
#define DNAME           "DNAME"

int main(int n, char **argv)
{
	getProperty gPw(gPw.P_WLAN);
	
	if (gPw.getInt(RADIOMODE) > 1) {
		if ( n == 1) {
		        LdProp slist;
			getProperty gP(gP.P_OTC);
        		slist.setProperty("SSID", gP.getStr(DNAME));
        		slist.setProperty("Channel", 0);
        		slist.setProperty("RF activities detected", " ");
        		slist.setProperty("Interference","");
        		slist.setProperty("Mode", (char*)"Ethernet");
        		slist.setProperty("Security", 0);
        		slist.saveToFile(WJ_START_SCAN_FILE);
		}	
		return 0;
	}
	
	if (n <= 2) initWlan(0, gPw);

        myWlan wlan(WLAN_CONFIG);
        if (n == 1) {			// SCAN only
                wlan.setAll();
                wlan.setScan();
        }
        else if( n == 2 )		// Setup based on SCAN file
                wlan.startUp();
	else 
		wlan.wpaSetup();

        return 0;
}
