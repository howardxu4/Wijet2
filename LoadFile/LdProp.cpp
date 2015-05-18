
#include "LdProp.h"
/**
 *      The LdProp class load the properties from file 
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */
Namecell::Namecell(char *name)
{
	m_name = new char[strlen(name) + 1];
        strcpy(m_name, name);
	m_next = NULL;
}

/**
 *      The destructor
 *
 */
Namecell::~Namecell()
{
	if (m_name!=NULL) delete [] m_name;
	if (m_next!=NULL) delete m_next;
}

/**
 *      The constructor
 *
 */
Property::Property()
{
        m_name = NULL;
	m_value = NULL;
	m_comment = NULL;
}

/**
 *      The destructor
 *
 */
Property::~Property()
{
	if (m_name != NULL) delete [] m_name;
	if (m_value != NULL) delete [] m_value;
	if (m_comment != NULL) delete [] m_comment;
}

/**
 *	moveProp method
 *
 *	@param prop - property 
 */
void Property::moveProp(Property &prop)
{
	m_name = prop.m_name;
	m_value = prop.m_value;
	m_comment = prop.m_comment;
	prop.m_name = NULL;
	prop.m_value = NULL;
	prop.m_comment = NULL;
}

/**
 *	setProp method
 *
 *	@param  name - property name
 *	@param  value - property value
 */
void Property::setProp(char* name, char* value, char* comment)
{
	if (name != NULL) {
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);
	}
	if (value != NULL) {
		m_value = new char[strlen(value) +1];
		strcpy(m_value, value);
	}
	if (comment != NULL) {
		m_comment = new char[strlen(comment) + 1];
		strcpy(m_comment, comment);
	}
}

/**
 *	changeValue method
 *
 *      @param  value - property value
 */
void Property::changeValue(char* value)
{
	if (m_value != NULL) delete [] m_value;
        if (value != NULL) {
                m_value = new char[strlen(value) +1];
                strcpy(m_value, value);
        }
	else m_value = NULL;
}

/**
 *      The constructor
 *
 *	@param	n - predefined number of entry
 */
LdProp::LdProp(int max, int inc)
{
	m_nlist = NULL;
	m_array = NULL;
	init(max, inc);
}

/**
 *      The destructor
 *
 */
LdProp::~LdProp()
{
	delete [] m_array;
	if (m_nlist != NULL) delete m_nlist;
}

/**
 *	init method
 *
 *	@param max - number of entry
 *	@param inc - number of increase
 */
void LdProp::init(int max, int inc)
{
        m_max = (max < 1)? 20:max;
	m_inc = (inc < 1)? 10:inc;
	m_count = 0;
	if (m_array != NULL) delete[] m_array;
        m_array = new Property[m_max];
	if (m_nlist != NULL) {
		 delete m_nlist;
		m_nlist = NULL;
	}
}

/*	noComment method
 *
 *	@param name - name of property without comment
 */
void LdProp::noComment(char *name)
{
	if (m_nlist == NULL) 
		m_nlist =  new Namecell(name);
	else {
		Namecell* p = m_nlist;
		while (p->m_next !=NULL) p = p->m_next;
		p->m_next = new Namecell(name);
	}
}

/*
 *	chkName method
 *
 *	@param	name - property name
 */
bool LdProp::chkName(char *name)
{
	Namecell* p = m_nlist;
	while (p != NULL) {
		if (strcmp(name, p->m_name) == 0) return true;
		p = p->m_next;
	}
	return false;
}

/*
 *	expandArray method
 *
 */
void LdProp::LdProp::expandArray()
{
	Property* tarray;
	m_max += m_inc;
	tarray = new Property[m_max];
	for (int i=0; i<m_count; i++) 
		tarray[i].moveProp(m_array[i]);
	delete [] m_array;
	m_array = tarray;
}

/*
 *      setProperty method
 *
 *      @param  name - property name
 *      @param  value - property string value
 *	@param	comment - property comment (optional)
 *      @return index - number of entry
 */
int LdProp::setProperty(char* name, char* value, char* comment)
{
	int i = getIndex(name);
	if (i == -1) {
        	if (m_count == m_max) expandArray();   
        	m_array[m_count].setProp(name,  takeoffQuote(value), comment);
        	m_count++;
	}
	else 
		m_array[i].changeValue(takeoffQuote(value));
	return m_count;
}

/*
 *      setProperty method
 *
 *      @param  name - property name
 *      @param  value - property integer value
 *      @param  comment - property comment (optional)
 *      @return index - number of entry
 */
int LdProp::setProperty(char* name, int value, char* comment)
{
	char strvalue[20];
	sprintf(strvalue, "%d", value);
	return setProperty(name, strvalue, comment);
}

/**
 *	trim method
 *
 *	@param line
 *	@return trimmed string
 */
char* LdProp::trim(char *line)
{
	int i = 0, j;
	while(isSpace(line[i])) i++;
	j = strlen(&line[i]);
        while(j-- > 1) if (!isSpace(line[i+j])) break;
	line[i+j+1] = 0;
	return(&line[i]);
}
 
/**
 *	takeoffQuote method
 *
 *	@param	line
 *	@return string between quotation marks
 */
char* LdProp::takeoffQuote(char *line)
{
        int i = 0, j = strlen(line);
        if (line[i]=='"') i++;
        if (line[j-1]=='"') line[j-1] = 0;
        return(&line[i]);
}

/**
 *	parseLine
 *
 *	@param 
 */
void LdProp::parseProp(char *line)
{
	int i = 0;
	while (line[i] != '=' && line[i] != 0) i++;
	if (line[i] == '=') {
		int j = i+1, k = 0;
		line[i] = 0;
		char *name = trim(&line[0]);
		bool noCMT = chkName(name);	// Add to support no comment 
		while(line[j] && line[j] != '\r' && line[j] != '\n') {
                	if (!noCMT && line[j] == '#')
				if (k == 0) k = j;
                        j++;
                }
		line[j] = 0;
		if (k != 0) line[k] = 0;
		setProperty(name, trim(&line[i+1]), k==0? NULL:&line[k+1]);
	}
}

/*	loadFromFile method
 *
 *	@param filename
 *	@return count - number of entry
 */
int LdProp::loadFromFile(char *filename)
{
	char line[1024];
	FILE *in = NULL;
// printf("Load File %s\n", filename);

	in = fopen(filename, "r");
	if (in) { 	
		while(fgets(line, 1023, in)) { 
			int i = 0;
			while(isSpace(line[i])) i++; 	
			if (line[i] == '#' || line[i] == '\n' || line[i] == 0)
			 	continue; 		// comment line
			parseProp(&line[i]);
		}
		fclose(in);
	}
	return m_count;
}

/*      saveToFile method
 *
 *      @param filename
 *      @return count - number of entry
 */
int LdProp::saveToFile(char *filename, bool quote)
{
        FILE *out = NULL;
	int i = 0;

        out = fopen(filename, "w");
        if (out) {
                while(i < m_count) {
			if (m_array[i].m_name[0] != 0) {	// has name?
			if (m_array[i].m_comment == NULL) {
				if (quote)
					fprintf(out, "%s=\"%s\"\n",
					m_array[i].m_name, m_array[i].m_value);
				else 
					fprintf (out, "%s=%s\n", 
					m_array[i].m_name, m_array[i].m_value?
						m_array[i].m_value:" ");
			}
			else {
				if (quote)
					fprintf(out, "%s=\"%s\"		#%s\n",
					m_array[i].m_name, m_array[i].m_value, m_array[i].m_comment);
				else
					fprintf (out, "%s=%s		#%s\n",
					m_array[i].m_name, m_array[i].m_value, m_array[i].m_comment);
			}
			}
			i++;
                }
                fclose(out);
        }
        return i;
}

/**
 *	getIndex method
 *
 *	@param name
 *	@return index - unfound -1
 */
int LdProp::getIndex(char* name)
{
	int i = 0;
	while(i < m_count)
		if(strcmp(m_array[i].m_name, name) == 0)
			return i;
		else i++;
	return -1;
}

/**
 *	getName method
 *	
 *	@param index
 *	@return name
 */
char* LdProp::getName(int index)
{
        if (index < m_count && index >= 0)
                return m_array[index].m_name;
        return NULL;
}

/**
 *      getValue method
 *
 *      @param index
 *      @return - value of property
 */
char* LdProp::getValue(int index)
{
	if (index < m_count && index >= 0)
		return m_array[index].m_value;
        return "";
}

/**
 *      getIntValue method
 *
 *      @param index
 *      @return - value of property
 */
int LdProp::getIntValue(int index)
{
        if (index < m_count && index >= 0){
		try {
                	return atoi(m_array[index].m_value);
		}
		catch(...){}
	}
        return -1;
}

/**
 *      setValue method
 *
 *      @param index
 *	@param value
 */
void LdProp::setValue(int index, char *value)
{
        if (index < m_count && index >= 0)
		m_array[index].changeValue(value);
}

/**
 *      setValue method
 *
 *      @param index
 *      @param intValue
 */
void LdProp::setValue(int index, int intValue)
{
        if (index < m_count && index >= 0){
		char p[10];
		sprintf(p, "%d", intValue);
                m_array[index].changeValue(p);
	}
}

/**
 *	remove method
 *
 *	@param index
 */
void LdProp::remove(int index)
{
	if (index < m_count && index >= 0) 
		m_array[index].m_name[0] = 0;
}

/**
 *      getCMT method
 *
 *      @param index
 *      @return comment
 */
char* LdProp::getCMT(int index)
{
        if (index < m_count && index >= 0)
                return m_array[index].m_comment;
        return NULL;
}


