

/**
 *	The Logger class provides recording of application trace on the system 
 *
 *  @author	Howard Xu
 *	@version	2.5
 */

#include "Logger.h"

// The time and system call releated includes
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>


void* threadOpen(void *data)
{
	Logger* log = (Logger*)data;
	log->openLog();
	pthread_exit(0);
}
#endif

/**
 *	The constructor
 *
 */
Logger::Logger()
{
	// set default
	mLevel = this->LEVEL;
	mNumRecs = this->RECDS;
	mNumFile = this->NUMFL;
	mFp = NULL;
	mFile = NULL;
}

/**
 *	The destructor
 *
 */
Logger::~Logger()
{
	closeLog();
	clsFile();
}

/**
 *	setOutput method
 *
 *	@param	file - full path of file name
 */
bool Logger::setOutput(char *file)
{
	if (file == mFile) return true;
	if (file != NULL && mFile != NULL)
		if (strcmp(file, mFile) == 0) return true;
	clsFile();
	if (file == NULL) {
		closeLog();
	}
	else {
		mFile=new char[ strlen(file) + 1];
		strcpy(mFile, file);
		doOpen();
	}
	return (mFp != NULL);
}

/**
 *	setLimit method
 *
 *	@param	limit - the record number per trace file
 */
void Logger::setLimit(int numRecord, int numFile)
{
	mNumRecs = numRecord;
	mNumFile = numFile;
}

/**
 *	getMillisecond method
 *
 *	@return millisecond in current second
 */
int Logger::getMillisecond()
{
	int millisecond;
#ifndef WIN32
  struct timeval timestamp;
  gettimeofday(&timestamp, 0);
  millisecond = timestamp.tv_usec;
#else
  LARGE_INTEGER ticksPerSecond;
  LARGE_INTEGER tick;   // A point in time

  // get the high resolution counter's accuracy
  QueryPerformanceFrequency(&ticksPerSecond);

  // what time is it?
  QueryPerformanceCounter(&tick);

  millisecond = (tick.QuadPart % ticksPerSecond.QuadPart)*1000000/ticksPerSecond.QuadPart;
#endif
  return millisecond;
}

/**
 *	getTime method
 *
 *	@return time format string yyyy-mm-dd hh:mm:ss:iiiiii
 */
char* Logger::getTime()
{
	static char sTime[30];
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	sprintf(sTime, "%d-%02d-%02d %02d:%02d:%02d.%u ", 
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1,
		timeinfo->tm_mday,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec,
		getMillisecond() );
	return sTime;
}

/**
 *	log method
 *
 *	@param	name - the module name
 *	@param	type - the level type
 *	@param	message - the body of trace
 */
void Logger::log(char *name, char *type, char *message)
{
	if (mFp) {
		try {
		fprintf(mFp,"%s %s [%s] %s\n", getTime(), name, type, message);
		fflush(mFp);
		if (this->mCount++ > this->mNumRecs) doOpen();
		}
		catch(...) {
			mFp = NULL;
		}
	}
	else
		fprintf(stderr, "%s %s [%s] %s\n", getTime(), name, type, message);
}

/**
 *	doOpen method
 *
 */
void Logger::doOpen()
{
#ifndef WIN32
	pthread_t myThread;
        pthread_create( &myThread, NULL, threadOpen, (void*)this);

#else
        openLog();
#endif
}

/**
 *	openLog method
 *
 */
void Logger::openLog()
{
	FILE *fp;
	char fname[250], tname[250];
	closeLog();
	for (int i=this->mNumFile; i>0; i--) {
		if (i == 1) 
			sprintf(fname, "%s.txt", mFile);
		else
			sprintf(fname, "%s_%d.txt", mFile, i-1);
		sprintf(tname, "%s_%d.txt", mFile, i);

		// if file exist save to circl
		fp = fopen(fname, "r");
		if (fp != NULL) {
			char cmd[500];
			fclose(fp);
#ifndef	WIN32
			sprintf(cmd, "cp -f %s %s", fname, tname);
#else
			sprintf(cmd, "copy /Y %s %s", fname, tname);
#endif
			system(cmd);
		}
	}
	mFp = fopen(fname, "w");
	if (mFp) fprintf(mFp,"%s ---- File %s start ----\n", getTime(), fname);
	mCount = 0; 
}

/**
 *	closeLog method
 */
void Logger::closeLog()
{
	if (mFp) fclose(mFp);
	mFp = NULL;
}

/**
 *	clsFile method
 */
void Logger::clsFile()
{
	if (mFile) delete mFile;
	mFile = NULL;
}

