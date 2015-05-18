
/**
 *	The LogTrace class 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#if !defined(AFX_LOGTRACE_H__22639A05_0AF9_463F_9A91_838F2415033C__INCLUDED_)
#define AFX_LOGTRACE_H__22639A05_0AF9_463F_9A91_838F2415033C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Logger.h"

class LogTrace : public Logger  
{
public:
	enum {
		OFF =   0,
		TRACE = 1,
		DEBUG = 2,
		INFO =  4,
		USER =  8,
		DEVLP = 16,
		WARN =  32,
		ERROR = 64,
		FATAL = 128,
		ALL =   255
	};
	LogTrace(char *name=NULL);
	virtual ~LogTrace();
	inline void setLog(int level, char* file=NULL) {
		setLevel(level);
		setOutput(file);
	}
	void log(int level, char* format,...);

private:
	void init(char *name);

	char *m_name;
};

#endif // !defined(AFX_LOGTRACE_H__22639A05_0AF9_463F_9A91_838F2415033C__INCLUDED_)
