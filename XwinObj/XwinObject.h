

/**
 *      The XwinObject class
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#if !defined(_XWINDOW_OBJECT_INCLUDED_DEFINED)
#define _XWINDOW_OBJECT_INCLUDED_DEFINED


#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include <math.h>

#include "LoadJpeg.h"

class XwinObject
{
public:
	XwinObject();
	~XwinObject();
	void closeWinObject();
	Display* openConnection(char *display);
	int getScreenWidth();
	int getScreenHeight();
	Window initWindow(int x, int y, int w, int h, long input, bool bd=0);
	XImage* loadImageFromFile(char *fname);
	void initColor();
	void setParent(Window w);
	void setNoborder(int d=0);
	void showWindow();
	void showWindow(int x, int y, int w, int h);
	void displayRect(int x, int y, int w, int h, unsigned int color=0);
	void getWindowWH(int *width, int *height, int erase=1);
	int displayImage(int width, int height, int center=1);
	int displayString(int x, int y, char *str, unsigned int color, unsigned backcolor=0);
        double getAspect();
	GC getTempGC(unsigned int color, unsigned int backcolor);
	char *getColor(int color);
	void movePointer(int x, int y);

	inline Display* getDisplay() { return m_display; };
	inline Window getWindow() { return m_window; };
	inline GC getGC() { return m_gc; };
	inline int getScreen() { return m_screen; };
	inline XImage* getXImage() { return m_ximage; };
	inline int getDepth() { return m_depth; };
	inline int getImageWidth() { return m_ldjpeg.getX(); };
	inline int getImageHeight() { return m_ldjpeg.getY(); };

	enum {
		Black = 0,
		Blue ,
		Green ,
		Yellow,
		Red ,
		White  
	};

private:
        Display*        m_display;
	Window		m_parent;
        Window          m_window;
        GC              m_gc;
	XGCValues       m_gcv;
        int             m_screen;
	XImage*		m_ximage;
	int		m_depth;
	LoadJpeg	m_ldjpeg;
	XFontStruct*	m_font;
	Colormap 	m_cmap;
	char*		m_color[White+1];
};
 
#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)

#endif	// _XWINDOW_OBJECT_INCLUDED_DEFINED

