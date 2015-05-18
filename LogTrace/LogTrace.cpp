

/**
 *	The LogTrace class provides the interface with application  
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#include "LogTrace.h"

/**
 *	The constructor
 *
 */
LogTrace::LogTrace(char *name)
{
	if (name == NULL || strlen(name)<1)
		init("System");
	else
		init(name);
}

/**
 *	The destructor
 *
 */
LogTrace::~LogTrace()
{
	delete this->m_name;
}

/**
 *	init method
 *
 *	@param	name - module name
 */
void LogTrace::init(char *name)
{
	this->m_name = new char[ strlen(name) + 1];
	strcpy(this->m_name, name);
//	this->m_log = Logger::getLogger();
}

/**
 *	log method
 *
 *	@param	level - the level of trace
 *	@param	format - the string format
 *	@param	vars - the variables to fill the format 
 */
void LogTrace::log(int level, char* format,...)
{
	if (this->checkLevel(level)) {
		char *type;
		char message[2048];	// time mName level message
        va_list ap;
        va_start(ap, format);
        vsprintf(message, format, ap);
        va_end(ap);
		switch(level) {
		case TRACE:
			type = "T"; break;
		case DEBUG:
			type = "D"; break;
		case INFO :
			type = "I"; break;
		case USER:
			type = "R"; break;
		case DEVLP:
			type = "P"; break;
		case WARN:
			type = "W"; break;
		case ERROR:
			type = "E"; break;
		case FATAL:
			type = "F"; break;
		default:
			type = "U"; break;			
		}
//		this->m_log->log(m_name, type, message);
		Logger::log(m_name, type, message);
	}
}
