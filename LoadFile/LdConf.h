
////////////////////////////
// The Load configuration file  class

#if (!defined(_LOAD_CONFIGURATION_FILE_DEFINED))
#define _LOAD_CONFIGURATION_FILE_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class OneLine
{
public:
	enum {
		INIT=0,
		COMM=1,
		PROP=2,
		GSTR=4,
		GEND=8,
		LINE=16,
		MASK=30,	// NO COMM
		ALLS=31
	};
	OneLine();
	~OneLine();
	int setLine(char* line);
	int substitute(char* line);
	void moveLine(OneLine &line);
	void toggleComm();
	inline void setType(int type) { m_type = type; };
	inline int getType() { return m_type; };
	inline char* getLine() { return m_line; };
private:
	inline bool isSpace(char c) { return (c == ' ' || c == '\t' ||
		c == '\r' || c=='\n' || c == 0); };
	void parseLine();
	char  m_type;
	char* m_line;
};

class LdConf
{
public:
	LdConf(int max=100, int inc=50);
	~LdConf();
	void init(int max=100, int inc=50);
	int loadFromFile(char* filename);
	int saveToFile(char* filename, bool nocomm=false);
	int getIndex(char* name, int mask=OneLine::MASK, int from=0);
	void setComment(int index, bool on=true);
	int setLine(char* line);
	inline void setComment(char* name, bool on=true) {
	    setComment(getIndex(name, OneLine::COMM + OneLine::PROP), on);};
	void setValue(char* name, char* value);
	void parSection();

	inline char* getLine(int index) { return isValid(index)? m_array[index].getLine(): (char*)""; };
	inline int getType(int index) { return isValid(index)? m_array[index].getType(): OneLine::INIT; }; 
	inline int getCount() { return m_count; };
	inline int chgLine(int index, char* value) { return isValid(index)? m_array[index].substitute(value): OneLine::INIT; }; 
	inline void changeType(int index) { if (isValid(index)) m_array[index].toggleComm(); };

private:
        inline bool isValid(int index) { return (index>=0 && index<m_count); };
//	inline void changeType(int index) { if (isValid(index)) m_array[index].toggleComm(); };
	void expandArray();
	int m_count;
	int m_max;
	int m_inc;
	OneLine *m_array;
};

#endif // _LOAD_CONFIGURATION_FILE_DEFINED
