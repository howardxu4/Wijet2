

/**
 *      The sXwinObject class
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#include "XwinObject.h"

typedef struct
{
        long flags;
        long functions;
        long decorations;
        long input_mode;
        long status;
} MWMHints;

#define MWM_HINTS_DECORATIONS   (1L << 1)
#define PROP_MWM_HINTS_ELEMENTS 5

/**
 *      The constructor
 *
 */

XwinObject::XwinObject() 
{
	m_parent = 0;
	m_display = NULL;
	m_ximage = NULL;
	m_font = NULL;
}

/**
 *      The destructor
 *
 */
XwinObject::~XwinObject()
{
	closeWinObject();
}

/**
 *	openConnection method
 *
 *	@param - display name
 *	@return Display pointer
 */
Display* XwinObject::openConnection(char *display)
{
        m_display = XOpenDisplay (display);
        if (m_display) 
                m_screen = DefaultScreen (m_display);
	return m_display;
}

/**
 *	closeWinObject()
 */
void XwinObject::closeWinObject()
{
        if (m_display != NULL) {
		setNoborder(1);
        	if (m_ximage != NULL) XDestroyImage(m_ximage);
        	if (m_font != NULL) XUnloadFont(m_display, m_font->fid);
        	XFreeGC(m_display, m_gc);
        	XCloseDisplay(m_display);
		m_display = NULL;
	}
}

/**
 *	getScreenWidth
 */
int XwinObject::getScreenWidth()
{	
	if(m_display == NULL) openConnection(getenv("DISPLAY"));
	return DisplayWidth(m_display, m_screen);
}

/**
 *	getScreenHeight
 */
int XwinObject::getScreenHeight()
{
	if(m_display == NULL) openConnection(getenv("DISPLAY"));
	return DisplayHeight(m_display, m_screen);
}

/**
 *	initColor method
 */
void XwinObject::initColor()
{
       m_color[Black] =  "black";
       m_color[Blue]  =  "blue";
       m_color[Green] =  "green";
       m_color[Yellow] = "yellow";
       m_color[Red]   =  "red";
       m_color[White] =  "white";
}

/**
 *      initWindow method
 *
 *      @param  x - window x position
 *	@param  y - window y position
 *	@param  w - window witdh
 *	@param  h - window height
 *	@param  input - window input event mask
 *	@param  bd - border flag
 *	@return window id
 */
Window XwinObject::initWindow(int x, int y, int w, int h, long input, bool bd)
{
	if (m_parent == 0)
		m_parent  =  RootWindow(m_display, m_screen);
	XLockDisplay(m_display);

	m_window = XCreateSimpleWindow(m_display, m_parent,
		x, y, w, h, 0, 0, 0);
	XSelectInput (m_display, m_window, input);
	if (!bd) setNoborder();

	XSync(m_display, False);
	XUnlockDisplay(m_display);

	m_depth=DefaultDepth(m_display, m_screen);
	m_gc = XCreateGC (m_display, m_window, 0, &m_gcv);

        m_cmap=DefaultColormap(m_display, m_screen);
	initColor();

	return m_window;
}

/**
 *      loadImageFromFile method
 *
 *      @param  filename - full path of file name
 *	@return Ximage
 */
XImage* XwinObject::loadImageFromFile(char *fname)
{
	XImage  *ximage = NULL;
			
        char *translated_buffer = m_ldjpeg.getImageDataFromFile(fname, m_depth);
	if (translated_buffer != NULL) {
        	ximage = XCreateImage (m_display,
                CopyFromParent,
                m_depth,
                ZPixmap,
                0,
                translated_buffer,
                m_ldjpeg.getX(), 
                m_ldjpeg.getY(), 
                m_ldjpeg.getBPL()*8, 
                m_ldjpeg.getBPL() * m_ldjpeg.getX()); 
		if (m_ximage != NULL) XDestroyImage(m_ximage);
        	m_ximage = ximage;
	}
	else { fprintf(stderr,"Empty data, return NULL\n"); }
	return ximage;
}

/**
 *	setParent method
 *
 *	@param	w - parent window
 */
void XwinObject::setParent(Window w)
{
	m_parent = w;
}

/**
 *      setNoborder method
 *
 */
void XwinObject::setNoborder(int d)
{
	Atom          prop;
	MWMHints      mwmhints;
	prop = XInternAtom(m_display, "_MOTIF_WM_HINTS", False);
	mwmhints.flags = MWM_HINTS_DECORATIONS;
	mwmhints.decorations = d;

	XChangeProperty(m_display, m_window, prop, prop, 32,
		PropModeReplace, (unsigned char *) &mwmhints,
		PROP_MWM_HINTS_ELEMENTS);
}

/**
 *	showWindow method
 *
 */
void XwinObject::showWindow()
{
	XFlush(m_display);
	XMapWindow (m_display, m_window);
}

/**
 *	showWindow method
 *
 *      @param  x - window x position
 *      @param  y - window y position
 *      @param  w - window witdh
 *      @param  h - window height
 */
void XwinObject::showWindow(int x, int y, int w, int h)
{
	showWindow();
	XMoveResizeWindow (m_display, m_window, x, y, w, h);
}

/**	fillRect
 *
 *	@param	x - rectangle x position
 *	@param	y - rectangle y position
 *	@param	w - rectangle width
 *	@param	h - rectangle height
 *	@param	color 
 */
void XwinObject::displayRect(int x, int y, int w, int h, unsigned int color)
{
        GC gc = getTempGC(color, color);
        XFillRectangle(m_display, m_window, gc, x, y, w, h);
        XFreeGC(m_display, gc);
}

/**
 *	getWindowWH
 *
 *	@param	*width	- window width
 *	@param	*height - window height
 *	@param	erase -	if true fill whole window
 */
void  XwinObject::getWindowWH(int *width, int *height, int erase)
{
	XWindowAttributes attr;
	XGetWindowAttributes(m_display, m_window, &attr);
	*width = attr.width;
	*height = attr.height;
	if (erase)
		XFillRectangle(m_display, m_window, m_gc, 
			0, 0, attr.width, attr.height);
}

/**
 *	displayImage method to show the loaded image on screen center
 *
 *      @param  width  - window width
 *      @param  height - window height
 *      @param  center - if false width is X, height is Y
 */
int XwinObject::displayImage(int width, int height, int center)
{
	if (center) {
		width = (width-m_ldjpeg.getX())/2;
		height = (height-m_ldjpeg.getY())/2;
	}
	XPutImage (m_display, m_window, m_gc, m_ximage,
		0,0,width, height, m_ldjpeg.getX(), m_ldjpeg.getY());
	return 0;
}

/**
 * 	getColor method
 *
 *	@param	color
 *	@return string of color name
 */
char *XwinObject::getColor(int color)
{
	if (color < Black) color = Black;
	if (color > White) color = White;
	return m_color[color];
}

GC XwinObject::getTempGC(unsigned int color, unsigned int backcolor)
{
	unsigned long valuemask = 0;
	XGCValues values;
	XColor fcolor, bcolor;
//	unsigned int line_width = 1;
//	int line_style = LineSolid; /*LineOnOffDash;*/
//	int cap_style = CapButt;
//	int join_style = JoinMiter;
//	int dash_offset = 0;
//	static char dash_list[] = {20, 40};
//	int list_length = sizeof(dash_list);
	GC gc = XCreateGC(m_display, m_window, valuemask, &values);

        if (m_font == NULL) 
                m_font=XLoadQueryFont(m_display,"9x15");
	if (m_font == NULL) { 
		printf("stderr, basicwin: cannot open 9x15 font\n");
		return gc;
	}

	XParseColor(m_display,m_cmap,getColor(backcolor),&bcolor);
	XAllocColor(m_display,m_cmap,&bcolor);

        XParseColor(m_display,m_cmap,getColor(color),&fcolor);
        XAllocColor(m_display,m_cmap,&fcolor);

	XSetFont(m_display, gc, m_font->fid);

	XSetForeground(m_display, gc, fcolor.pixel);
	XSetBackground(m_display, gc, bcolor.pixel);

//	XSetLineAttributes(m_display, gc, line_width, line_style, cap_style, join_style);
//	SetDashes(m_display, gc, dash_offset, dash_list, list_length);
	return gc;
}

/**
 *	displayString method to show a string on screen
 *
 *	@param	x - horizontal position
 *	@param	y - vertical position
 *	@param	str - character string
 *	@param	color - 
 *	@return 
 */
int XwinObject::displayString(int x, int y, char *str, 
	unsigned int color, unsigned int backcolor)
{
	// set color first
	GC gc = getTempGC(color, backcolor);
	XDrawImageString(m_display, m_window, gc,
		x, y, str, strlen(str));
	XFreeGC(m_display, gc);
	return 0;
} 

/**
 *	getAspect() method
 *
 *	@return - window's aspect
 */
double XwinObject::getAspect()
{
	double   res_h, res_v, aspect;
	res_h = (DisplayWidth(m_display, m_screen) * 1000 / DisplayWidthMM(m_display, m_screen));
	res_v = (DisplayHeight(m_display, m_screen) * 1000 / DisplayHeightMM(m_display, m_screen));
	aspect  = res_v / res_h;

	if(fabs(aspect - 1.0) < 0.01) aspect = 1.0;
	return aspect;
}

void XwinObject::movePointer(int x, int y)
{
	XWarpPointer(m_display, None, None, 0, 0, 0, 0,  x, y);
}

