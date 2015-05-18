
#include "LdConf.h"
/**
 *      The LdConf class load the confiurations from file 
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */
OneLine::OneLine()
{
        m_type = INIT;
	m_line = NULL;
}

/**
 *      The destructor
 *
 */
OneLine::~OneLine()
{
	if (m_line != NULL) delete [] m_line;
}

/**
 *	setLine method
 *
 *	@param  line - configuration line
 *	@return type = type of line
 */
int OneLine::setLine(char* line)
{
	if (line != NULL) {
		m_line = new char[strlen(line) + 2];
		strcpy(m_line, line);
		if (m_line[strlen(line)-1] != '\n')
                        strcat(m_line, "\n");
		parseLine();
	}
	return m_type;
}

/**
 *	substitute method
 *
 *      @param  line - line value
 *      @return type = type of line
 */
int OneLine::substitute(char* line)
{
	if (m_line != NULL) delete [] m_line;
        if (line != NULL) 
		return setLine(line);
	else {
		m_type = INIT;
		m_line = NULL;
	}
	return m_type;
}

/**
 *      moveLine method
 *
 *      @param line - OneLine object
 */
void OneLine::moveLine(OneLine &line)
{
	m_type = line.m_type;
	m_line = line.m_line;
	line.m_line = NULL;
}

void OneLine::toggleComm() {
	if (m_type == COMM) {
		char* p = m_line;
		char* q = p;
		while(*p && *p != '#') p++;
		if (*p) {
			m_line = NULL;
			substitute(++p);
			parseLine();
			delete [] q;			
		}			
	}
	else {
		char *p = new char[ strlen(m_line) + 2];
		*p = '#';
		strcpy(&p[1], m_line);
		delete [] m_line;
		m_line = p; 
		m_type = COMM;
	}
}

/**
 *      parseLine method
 *
 */
void OneLine::parseLine()
{
	m_type = INIT;
	if (*m_line == '#') m_type = COMM;
	else {
		char* p = m_line;
		bool empty = true;
		while(*p++) {
			if (*p == '=') m_type = PROP;
			else if (*p == '{') m_type = GSTR;
			else if (*p == '}') m_type = GEND;
			else if (*p == '#' && empty && m_type == INIT) {
				m_type = COMM;
				break;
			}
			else if (empty && !isSpace(*p)) 
				empty = false;
		}
		if (m_type == INIT && !empty) m_type = LINE;		
	}
}

/**
 *      The constructor
 *
 *	@param	n - predefined number of entry
 */
LdConf::LdConf(int max, int inc)
{
	m_array = NULL;
	init(max, inc);
}

/**
 *      The destructor
 *
 */
LdConf::~LdConf()
{
	delete [] m_array;
}

/**
 *	init method
 *
 *	@param max - number of entry
 *	@param inc - number of increase
 */
void LdConf::init(int max, int inc)
{
        m_max = (max < 1)? 50:max;
	m_inc = (inc < 1)? 25:inc;
	m_count = 0;
	if (m_array != NULL) delete[] m_array;
        m_array = new OneLine[m_max];
}

/*
 *	expandArray method
 *
 */
void LdConf::expandArray()
{
	OneLine* tarray;
	m_max += m_inc;
	tarray = new OneLine[m_max];
	for (int i=0; i<m_count; i++) 
		tarray[i].moveLine(m_array[i]);
	delete [] m_array;
	m_array = tarray;
}

/*      setLine method
 *
 *      @param line
 *      @return count - number of entry
 */
int LdConf::setLine(char* line)
{
	if (m_count == m_max) expandArray();
	m_array[m_count].setLine(line);
        return ++m_count;
}

/*	loadFromFile method
 *
 *	@param filename
 *	@return count - number of entry
 */
int LdConf::loadFromFile(char *filename)
{
	char line[1024];
	FILE *in = NULL;
// printf("Load File %s\n", filename);

	in = fopen(filename, "r");
	if (in) { 	
		while(fgets(line, 1023, in)) { 
			if(strlen(line) > 0) 
				setLine(line);
		}
		fclose(in);
	}
	return m_count;
}

/*      saveToFile method
 *
 *      @param filename
 *	@param nocomm - true|false
 *      @return count - number of entry
 */
int LdConf::saveToFile(char *filename,  bool nocomm)
{
        FILE *out = NULL;
	int i = 0;

        out = fopen(filename, "w");
        if (out) {
                while(i < m_count) {
			if (nocomm && m_array[i].getType() == OneLine::COMM);
			else if (m_array[i].getLine() != NULL) {
				fprintf(out, "%s", m_array[i].getLine());
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
 *	@param mask - type mask
 *	@param from - position
 *	@return index - unfound -1
 */
int LdConf::getIndex(char* name, int mask, int from)
{
	int i = from;
	while(i < m_count) {
		if(m_array[i].getType() & mask) 
			if(strstr(m_array[i].getLine(), name) != NULL)
			return i;
		i++;
	}
	return -1;
}

/**
 *      setComment method
 *
 plist.loadFromFile*      @param name
 *	@param on - comment on|off
 *      @return none
 */
void LdConf::setComment(int index, bool on)
{
	if (isValid(index)) {
		int type = getType(index);
		if (on && type == OneLine::PROP) {
			changeType(index);
		}
		else if (!on && type == OneLine::COMM) {
			changeType(index);;
		}	
	}
}

/**
 *      setValue method
 *
 *      @param name
 *	$param value
 *      @return none
 */
void LdConf::setValue(char* name, char* value)
{
	char line[1024];
        int index = getIndex(name, OneLine::COMM + OneLine::PROP);
	sprintf(line, "%s=%s\n", name, value);
	if (isValid(index)) {
		setComment(index, false);
		m_array[index].substitute(line);		
	}
	else 
		setLine(line);
}

/**
 *      parSection method -- parse Section block
 *
 *      @return none
 */
void LdConf::parSection()
{
        int i = 0;
        while(i < m_count) {
                if(m_array[i].getType() > OneLine::COMM) {
                        if(strstr(m_array[i].getLine(), "Section") != NULL) {
				if(strstr(m_array[i].getLine(), "End") != NULL)
					m_array[i].setType(OneLine::GEND);
				else
					m_array[i].setType(OneLine::GSTR);
			}
                }
//		printf("%d: [%d] %s", i, m_array[i].getType(), m_array[i].getLine());
		i++;
        }	
}
