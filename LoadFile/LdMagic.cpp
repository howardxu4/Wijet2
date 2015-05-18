
#include "LdMagic.h"
/**
 *      The LdMagic class load the magic image from file
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */
LdMagic::LdMagic()
{
	m_magic = MAGICNUM;
	m_size = 0; 
	m_type = U_DATA;
	m_upgrade = NULL;
	m_tarfile = NULL;
}
/**
 *      The destructor
 *
 */
LdMagic::~LdMagic()
{
	if (m_upgrade != NULL) delete m_upgrade;
	if (m_tarfile != NULL) delete m_tarfile;
}

/*
 *	setTarFile method (full path name)
 *
 */
bool LdMagic::setTarFile(char *fname)
{
	char command[200];
	if (m_tarfile != NULL) { 
		delete m_tarfile;
		m_tarfile = NULL;
	}
    	sprintf(command, "cd %s; tar -zx %s -f %s > /dev/null 2>&1",
         	TEMP_DIR, PATCH_DESCRIPT, fname);
	if (system(command) != 512) {
		m_tarfile = new char[strlen(fname)+1];
		strcpy(m_tarfile, fname);	
	}
	return m_tarfile != NULL;
}

/*
 *	checkMagic method
 *
 */
int LdMagic::checkMagic()
{
    //
    // Check the patch-script.
    //
    if (m_tarfile != NULL) {
	char command[200];
        sprintf(command, "%s/%s", TEMP_DIR, PATCH_DESCRIPT);
        if (loadFromFile(command) == 0) {
           char* p =  getUpgrade(false);
           sprintf(command, "cd / ; tar -zx tmp/%s -f %s > /dev/null 2>&1",
                 p, m_tarfile);
           if (system(command) != 512) {
                sprintf(command, "%s/%s", TEMP_DIR, p);
                if (getSize() == getFileSize(command) &&
                        getCksum() == getCheckSum(command))
				return getType();
           }
        }
	// clean up
        sprintf(command, "rm -f %s/%s", TEMP_DIR, PATCH_DESCRIPT);
        system(command);
    }
    return -1;	
}

/*
 *	runScript method
 *
 */
void LdMagic::runScript()
{
	if (m_tarfile != NULL && m_upgrade != NULL) {
       		char command[200], *p;
		p = getUpgrade(false); 
                sprintf(command, "cd /tmp; ./%s %s > /dev/null 2>&1", p, m_tarfile);
                system(command);
		// clean up
                sprintf(command, "rm -f /tmp/%s", p);
                system(command);
		delete m_upgrade;
		m_upgrade = NULL;		
	}
}

/*
 *      setUpgrade method
 *
 */
void LdMagic::setUpgrade(char *name)
{
	if (m_upgrade != NULL) delete m_upgrade;
	if (name == NULL)
		m_upgrade = NULL;
	else {
		m_upgrade = new char[strlen(name)+1];
		strcpy(m_upgrade, name);
	}
}

/*
 *      getUpgrade method
 *
 */
char* LdMagic::getUpgrade(bool f)
{
	if (m_upgrade == NULL) return UPGRADE;
	if (f) return m_upgrade;
	int i, l = strlen(m_upgrade);
        for(i=l-1; i>=0; i--)
        	if (m_upgrade[i] == '/')
                break;
	++i;
        return( &m_upgrade[i]);
}

/*      loadFromFile method
 *
 *      @param filename
 *      @return 0 - OK: otherwise Error
 */
int LdMagic::loadFromFile(char* filename)
{
	int rtn = -1;
        char line[1024];
        FILE *in = NULL;
// printf("Load File %s\n", filename);
	
        in = fopen(filename, "r");
        if (in) {
		fread((char*)&m_magic, sizeof(int), 1, in);
		if (m_magic == MAGICNUM) {
			fread((char*)&m_size, sizeof(int), 1, in);
			fread((char*)&m_type, sizeof(int), 1, in);
			fread((char*)&m_cksum, sizeof(int), 1, in);
			if(fgets(line, 1023, in)) {
				setUpgrade(line);
				rtn = 0;
			}
			else rtn = -3;
		} else rtn = -2;
		fclose(in);
	}
	return rtn;
}

/*
 *	getFileSize method
 *
 *	@param fname -- file name
 *	$return size -- number byte of file
 */	
int LdMagic::getFileSize(char *fname)
{
	int n = 0;
	struct stat fstat;
	n = stat(fname, &fstat);
	if (n < 0) n = 0;
	else n = fstat.st_size;
	return n;
}

/*
 *      getCheckSum method
 *
 *      @param fname -- file name
 *      @return size -- checksum of file
 */
int LdMagic::getCheckSum(char *fname)
{
	bool order = false;
	int  rtn = 0, ln = 0, cn;
	char line[1024], *p;
        FILE *in = NULL;
	in = fopen(fname, "r");
        if (in) {
		while(fgets(line, 1023, in)) {
			ln ++;
			p = line;
			cn = 1;
			while (*p) {	
				if (order) rtn += (*p)*2;
				else rtn += (*p) + cn;
				order = !order;
				p++;
				cn ++;
			}	
			if (order) rtn += strlen(line)*2;
			else  rtn += strlen(line) + ln;
                        order = !order;
		}
		fclose(in);
	}
	return rtn;
}

/*      saveToFile method
 *
 *      @param filename -- saved file name
 *	@param type -- upgrade type
 *	@param upgrade -- upgrade batch file name
 *      @return 0 - OK: otherwise Error
 */
int LdMagic::saveToFile(char*filename, int type,  char* upgrade)
{
	FILE *out = NULL;
	int rtn = -1;
	if (upgrade != NULL) 
		setUpgrade(upgrade);
	else if (m_upgrade == NULL)
		setUpgrade(UPGRADE);

        m_size = getFileSize(m_upgrade);
	if (m_size > 0 && type >= U_DATA && type <= U_SYS) {
		m_type = type;
		m_cksum = getCheckSum(m_upgrade);

        	out = fopen(filename, "w");
        	if (out) {
			fwrite((char*)&m_magic, sizeof(int), 1, out);
			fwrite((char*)&m_size, sizeof(int), 1, out);
			fwrite((char*)&m_type, sizeof(int), 1, out);
			fwrite((char*)&m_cksum, sizeof(int), 1, out);
			fprintf(out, "%s", m_upgrade);
			fclose(out);
			rtn = 0;
		}
	}
	return rtn;
}
