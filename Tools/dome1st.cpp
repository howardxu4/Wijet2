
#include "../LoadFile/LdMagic.h"

#define  ROOT_FIRMWARE_FILE "/root/upgrade.tar.gz"
#define  ROOT_START_FILE    "/root/.components/start.tar.gz"

#define  PRODUCT_VERSION

int main(int n, char* a[])
{
    LdMagic mgc;
#ifdef PRODUCT_VERSION
    char command[200];
    if (n == 1) {
    	if (mgc.setTarFile(ROOT_FIRMWARE_FILE)) {
		if (mgc.checkMagic() >= 0) {
#ifdef DEBUG
                printf("size = %d\n", mgc.getSize());
                printf("type = %d\n", mgc.getType());
                printf("cksum = %d\n", mgc.getCksum());
                printf("upgrade = %s\n", mgc.getUpgrade());
#endif
			mgc.runScript();
		}
    		sprintf(command, "cd ; rm -f %s; sync", ROOT_FIRMWARE_FILE);
    		system(command);
    	}
	if (mgc.setTarFile(ROOT_START_FILE)) {
		if (mgc.checkMagic() >= 0) 
			mgc.runScript();
	}
    }
#else
// crpatch -- Create a patch.txt depending on upgrade
    if (n == 4) {
	n = atoi(a[3]);
	if (strlen(a[1]) < 3) a[1] = "patch.txt";
	if (strlen(a[2]) < 3) a[2] = "/tmp/upgrade";
       	n = mgc.saveToFile(a[1], n, a[2]);
	if (n == 0) {
       		printf("save Magic file return %d\n", n);
       		n = mgc.loadFromFile("patch.txt");
       		printf("load Magic file return %d\n", n);
       		printf("size = %d\n", mgc.getSize());
       		printf("type = %d\n", mgc.getType());
       		printf("cksum = %d\n", mgc.getCksum());
       		printf("upgrade = %s\n", mgc.getUpgrade());
	}
   }
   if (n != 0) {
	printf("Wrong parameters! There must has three parameters.\nUsage:\n");
	printf("%s {patch.txt} {/tmp/upgrade} { 0|1|2|3|4|5 }\n", a[0]);
	printf("If the length of parameter less than 3 using default value.\n");
   }
#endif
   return 0;
};


