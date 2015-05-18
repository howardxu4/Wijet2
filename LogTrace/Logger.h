

/**
 *	The Logger class 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#if !defined(AFX_LOGGER_H__761290FC_BCD4_4BAF_B3A7_77370DE96282__INCLUDED_)
#define AFX_LOGGER_H__761290FC_BCD4_4BAF_B3A7_77370DE96282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

class Logger  
{
public:
	Logger();
	virtual ~Logger();
	bool setOutput(char *file);
	void setLimit(int numRecord, int numFile=NUMFL);
	inline void setLevel(int level) { mLevel = level; };
	inline int getLevel() { return mLevel; };
	inline bool checkLevel(int level){ return ((mLevel & level) != 0); };
	void log(char *name, char *type, char *message);
	inline void stopLog() { clsFile(); closeLog(); }; 
	void openLog();	

private:
	enum {
		LEVEL = 0,
		NUMFL = 3,
		RECDS = 500
	};
	void doOpen();
	void closeLog();
	void clsFile();
	int  getMillisecond();
	char* getTime();

	char* mFile;
	int	mLevel;
	int mNumRecs;
	int mNumFile;
	int mCount;
	FILE* mFp;
};

#endif // !defined(AFX_LOGGER_H__761290FC_BCD4_4BAF_B3A7_77370DE96282__INCLUDED_)
