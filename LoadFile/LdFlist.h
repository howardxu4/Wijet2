
////////////////////////////
// The Load Banner files  class

#if (!defined(_LOAD_BANNER_FILES_DEFINED))
#define _LOAD_BANNER_FILES_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

class BannerFile
{
public:
	BannerFile(char* filename, char* type, int duration, int playable, int order);
	~BannerFile();

	char* m_filename;
	char* m_type;
	int m_duration;
	int m_playable;
	int m_order;
	BannerFile *m_next;

	int getFileSize();
};

class LdFlist
{
public:
	LdFlist();
	~LdFlist();
	void cleanList();
	int loadList(char* filename);
	int saveList(char* filename);
	int addFile(char *filename, char *type, int duraion, int playable, int order);
	int instFile(char *filename, char *type, int duraion, int playable, int order);
	int delFile();
	int nextFile(bool circl=true);
	char* getFilename(bool fonly=false);
	char* getType();
	int getSize();
	inline void setHead() { m_curr = m_header; };
	inline int getDuration() {
		if (m_curr) return m_curr->m_duration;
		else return -1;
	};
	inline int getPlayable() {
        	if (m_curr) return m_curr->m_playable;
        	else return 0;
	};
	inline int getOrder() {
		if (m_curr) return m_curr->m_order;
		else return 0;
	};
	inline int getCount() { return m_count; };
private:
	int m_count;
	BannerFile *m_header;
	BannerFile *m_tail;
	BannerFile *m_curr;
};

#endif // _LOAD_BANNER_FILES_DEFINED
