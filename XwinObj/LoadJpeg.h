#if !defined(_LOAD_JPEG_FILE_DEFINED)
#define _LOAD_JPEG_FILE_DEFINE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

class LoadJpeg
{
public:
	LoadJpeg();
	~LoadJpeg();
	int readJpegFile (char *filename);
	char* getImageDataFromFile(char *fname, int depth);
	int getX() { return m_x; };
	int getY() { return m_y; };
	int getW() { return m_w; };
	int getBPL() { return m_bpl; };
	unsigned char* getBuffer() { return m_buffer; };
private:
        unsigned char *m_buffer;
        int m_x;
        int m_y;
        int m_w;
        int m_bpl;
};
#endif	// _LOAD_JPEG_FILE_DEFINE

