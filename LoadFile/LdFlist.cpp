
#include "LdFlist.h"
/**
 *      The LdFlist class load the banner file list
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */
BannerFile::BannerFile(char *filename, char* type, int duration, int playable, int order)
{
	m_filename = new char[strlen(filename) + 1];
	strcpy(m_filename, filename);
	m_type = new char[strlen(type) + 1];
	strcpy(m_type, type);
	m_duration = duration;
	m_playable = playable;
	m_order = order;
	m_next = NULL;
}

/**
 *      The destructor
 *
 */
BannerFile::~BannerFile()
{
	if (m_filename) delete m_filename;
	if (m_type) delete m_type;
}

/*
 *	getFileSize method
 *
 *	@return	file size
 */
int BannerFile::getFileSize()
{
        struct stat fstat;
        int size = 0;
        if (stat(m_filename, &fstat) >= 0)
                size = fstat.st_size;
        return size;
}

/**
 *      The constructor
 *
 */
LdFlist::LdFlist()
{
	m_count = 0;
	m_header = NULL;
	m_tail = NULL;
	m_curr = NULL;
}

/**
 *      The destructor
 *
 */
LdFlist::~LdFlist()
{
	cleanList();
}

/**
 *	cleanList method to clean the list of file list
 *
 */
void LdFlist::cleanList()
{
	while (m_header != NULL) {
		m_curr = m_header;
		m_header = m_curr->m_next;
		delete m_curr;
	}
        m_count = 0;
        m_tail = NULL;
        m_curr = NULL;
}

/**
 *	loadList method to load the list of banner files into the file list
 *
 *	@param filename - fullpath name
 *	@return count - number of file entries
 */
int LdFlist::loadList(char* filename)
{
	char line[512];
	char fname[512];
	char type[32];
  	int duration;
	int playable;
	int order = 1;
	if (m_tail != NULL) order = m_tail->m_order+1;
 	FILE *in = fopen(filename, "r");
	if (!in) return 0;
	while(fgets(line, 512, in)) { // more lines left in the file
    		sscanf(line, "%s%s%i%i\n", fname, type, &duration, &playable);
		if (duration < 0) duration = 24 * 3600;
		addFile(fname, type, duration, playable, order++);
	}
	fclose(in);
	return m_count;
}

/**
 *      saveList method to save the file list into a file 
 *
 *      @param filename - fullpath name
 *      @return count - number of file entries
 */
int LdFlist::saveList(char* filename)
{
        FILE *out = fopen(filename, "w");
	setHead();
        if (out && m_curr) {
		do {
			fprintf(out, "%s	%s	%d %d \n",
 				m_curr->m_filename,
				m_curr->m_type,
				m_curr->m_duration,
				m_curr->m_playable);
		} while( nextFile(false) > 0);
        	fclose(out);
	}
        return m_count;
}

/**
 *	addFile method to add a file entry in the file list
 *
 *	@param  filename - full path name
 *	@param  type - file type
 *	@param  duration - second on diplay
 *	@param	playable -
 *	@param	order - sequence
 *	@return index 
 *
 */
int LdFlist::addFile(char *filename, char *type, int duration, int playable, int order)
{
	if (m_count == 0) {
		m_header = new BannerFile(filename, type, duration, playable, order);
		m_tail = m_header;
		m_curr = m_header;
	}
	else {
		m_tail->m_next = new BannerFile(filename, type, duration, playable, order);
		m_tail = m_tail->m_next;
	}
	m_count++;
	return m_count;
}

/**
 *      instFile method to insert a file entry in order of file list
 *
 *      @param  filename - full path name
 *      @param  type - file type
 *      @param  duration - second on diplay
 *	@param	playable -
 *	@param	oder - random 
 *      @return index
 *
 */
int LdFlist::instFile(char *filename, char *type, int duration, int playable, int order)
{
	BannerFile *curr = m_header;
        if (m_count == 0) 
		return addFile(filename, type, duration, playable, order);
	if (curr->m_order >= order) {
// on the first
		m_header = new BannerFile(filename, type, duration, playable, order);
		m_header->m_next = curr;
	}
	else {
		BannerFile *prev = curr;
		curr = curr->m_next;
		while (curr != NULL) {
			if (curr->m_order >= order) {
// in the middle
				prev->m_next = new BannerFile(filename, type, duration, playable, order);
				prev = prev->m_next;
				prev->m_next = curr;
				break;
			}
			else {
				prev = curr;
				curr = curr->m_next;
			}
		}
		if (curr == NULL) {
// on the last
			m_tail->m_next = new BannerFile(filename, type, duration, playable, order);
			m_tail = m_tail->m_next;
		}
	}
	m_count++;
	return m_count;
}

/**
 *	delFile method to delete current file point
 *
 *	@return count - number of file entries
 */
int LdFlist::delFile()
{
	if (m_count > 0) {
		m_count--;
		if (m_count == 0) {			// only one
			delete m_curr;
		        m_header = NULL;
			m_tail = NULL;
			m_curr = NULL;
		}
		else if (m_curr == m_header) {		// first one
			m_header = m_header->m_next;
			delete m_curr;
			m_curr = m_header;
		}
		else {
			BannerFile *prev = m_header;
			while(prev->m_next && prev->m_next != m_curr) 
				prev = prev->m_next;
			prev->m_next = m_curr->m_next;
			if (m_curr == m_tail)		// last one 
				m_tail = prev;
			delete m_curr;
			if ( prev->m_next != NULL)
				m_curr = prev->m_next;
			else		 
				m_curr = m_header;
		}		
	}
	return m_count;
}

/**
 *	nextFile method to adjust current file point
 *
 *	@return	count - number 0f file entries
 */
int LdFlist::nextFile(bool circle)
{
	if (m_curr) {
		m_curr = m_curr->m_next;
		if (m_curr == NULL) {
			m_curr = m_header;
			if (!circle) return 0;
		}
	}
	return m_count;
}

/**
 * 	getFilenmae method
 *
 *	@param  fonly -- true for file only, false for full path
 *	@return	filename
 */
char* LdFlist::getFilename(bool fonly)
{
	char *p = NULL;
	if (m_curr) {
		p = m_curr->m_filename;
		if (fonly) {
			char *q = p;
			while( *q != '\0') {
				if (*q == '/') p = q+1;
				q++;
			}
		}
	}
        return p;
}

/**
 *	getType method
 *
 *	@return	type
 */
char* LdFlist::getType()
{
	if (m_curr) return m_curr->m_type;
	return NULL;
}

/**
 *      getSize method
 *
 *      @return size
 */
int LdFlist::getSize()
{
	if (m_curr) return m_curr->getFileSize();
	return 0;
}
