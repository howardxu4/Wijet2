
////////////////////////////
// The Load property file  class

#if (!defined(_LOAD_PROPERTIES_FILE_DEFINED))
#define _LOAD_PROPERTIES_FILE_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Namecell
{
public:
	Namecell(char *name);
	~Namecell();
	char* m_name;
	Namecell* m_next;
};

class Property
{
public:
	Property();
	~Property();
	void moveProp(Property &prop);
	void setProp(char *name, char*strValue, char* comment);
	void changeValue(char* strValue);
	char* m_name;
	char* m_value;
	char* m_comment;
};

class LdProp
{
public:
	LdProp(int max=10, int inc=5);
	~LdProp();
	void init(int max=10, int inc=5);
	void noComment(char *name);
	int loadFromFile(char* filename);
	int saveToFile(char*filename, bool quote=false);
	int setProperty(char* name, char* value, char* comment=NULL); 
	int setProperty(char* name, int value, char* comment=NULL);
	int getIndex(char* name);
	char* getName(int index);
	char* getValue(int index);
	char* getCMT(int index);
	inline char* getValue(char* name) {
		 return getValue(getIndex(name));};
	int getIntValue(int index);
	inline int getIntValue(char* name) { 
		return getIntValue(getIndex(name));};
	void setValue(int index, char* value);
	inline void setValue(char* name, char* value) { 
		setValue(getIndex(name), value); };
	void setValue(int index, int intValue);
	inline void setValue(char* name, int intValue) { 
		setValue(getIndex(name), intValue); };
	inline int getCount() { return m_count; };
	void remove(int index);
private:
	inline bool isSpace(char c) { return (c == ' ' || c == '\t'); };
	char *trim(char *line);
	char *takeoffQuote(char *line);
	void parseProp(char *line);
	void expandArray();
	bool chkName(char *name);
	int m_count;
	int m_max;
	int m_inc;
	Namecell *m_nlist;
	Property *m_array;
};

#endif // _LOAD_PROPERTIES_FILE_DEFINED
