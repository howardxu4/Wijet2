
#include "WatchDog.h"
#include <pthread.h>

/**
 *      The methods of WatchDog class 
 *
 *  	@author     Howard Xu
 *	@version        2.5
 */

#define  PATCH_DESCRIPT	    "patch.txt"
#define  TEMP_DIR	    "/tmp"
#define  TEMP_FIRMWARE_FILE "/tmp/wlan.tar.gz"
#define  USB_UPGRADE_FILE   "upgrade.tar.gz"

void WatchDog::uploadIMG()
{
	LdMagic mgc;
        int n = m_shmm->Changes;
	char command[60];
	if (mgc.setTarFile(TEMP_FIRMWARE_FILE)) {
                n = mgc.checkMagic();
                if (n >= 0) {
		        if (n > LdMagic::U_SHELL) system("ka");
                        mgc.runScript();
                }
        }
	// clean up
	sprintf(command, "rm -f %s; sync", TEMP_FIRMWARE_FILE); 
	system(command);
	if (n >= LdMagic::U_SYS) 
		system("ka 1");       // REBOOT
}

char* mp[2] = { "usb", "mnt" };
 
void WatchDog::usbMount(int n, bool f)
{
	char cmd[60];
	sprintf(cmd, "umount -f /%s 2> /dev/null", mp[n==1?1:0]);
	m_Log->log(LogTrace::INFO, cmd);
        system(cmd);
        if (f ) {
		if(n == 2) system("mkdir /usb"); 
		sprintf(cmd,"mount /dev/sd%c1 /%s",n==1?'a':'b',mp[n==1?1:0]);
		m_Log->log(LogTrace::INFO, cmd);
		system(cmd);
		callSys("xwinfo \" USB is mounted \"");
                // start a thread to check magic and run script
		initUSB(n);
		m_mount |= n;
        }
        else {
		m_mount &= (n==1?2:1);
		callSys("xwinfo \" USB is unmounted \" 1");
	}
}

void* startUSB(void *which)
{
        LdMagic mgc;
        char command[200];
	int n = (*(int*)(which) == 1)?1:0;

        sprintf(command, "/%s/%s", mp[n], USB_UPGRADE_FILE);
        if (mgc.setTarFile(command)) {
		if (mgc.checkMagic() >= 0) 	// full control in script
                        mgc.runScript();
        }
	pthread_exit(0);
}

void WatchDog::initUSB(int n)
{
	pthread_t myThread;
	pthread_create( &myThread, NULL, startUSB, (void*)&n);
}
