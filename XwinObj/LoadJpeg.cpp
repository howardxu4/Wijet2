/* Gleicon S. Moraes - gleicon@terra.com.br */
/* cc -o xputjpeg xputjpeg.c  -L/usr/X11R6/lib -lX11 -lXpm -lXext -lm -ljpeg */
/* just shows a jpeg image in the screen, using xlib */
/* for now, I am just translating from the 24 bits (JPEG) to 16 bits ( my X display) */

#include "LoadJpeg.h"

#include <jpeglib.h>
#include <setjmp.h>

/* decompress jpeg
 * Gleicon - 05/10/99
 * cc -o jdec jdec.c -ljpeg
 */

/* Error handling */

struct error_mgr {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
        };

typedef struct error_mgr * my_error_ptr;


METHODDEF(void) my_error_exit (j_common_ptr cinfo) {
        my_error_ptr myerr = (my_error_ptr) cinfo->err;
        (*cinfo->err->output_message) (cinfo);
        longjmp(myerr->setjmp_buffer, 1);
        }

GLOBAL(int) read_JPEG_file (char *filename, int *px, int *py, int *pw, unsigned char **pbuffer) {
          struct jpeg_decompress_struct cinfo;
          struct error_mgr jerr;
          FILE *infile;
          JSAMPARRAY buffer;
          int row_stride, lines=0, total_size;
         unsigned char *buf_tmp;


          if ((infile = fopen(filename, "rb")) == NULL) {
                fprintf(stderr, "Can't open: %s\n", filename);
                return 0;
                }


          cinfo.err = jpeg_std_error(&jerr.pub);
          jerr.pub.error_exit = my_error_exit;

          if (setjmp(jerr.setjmp_buffer)) {
                jpeg_destroy_decompress(&cinfo);
                jpeg_destroy_decompress(&cinfo);
                fclose(infile);
                return 0;
                }

          jpeg_create_decompress(&cinfo);
          jpeg_stdio_src(&cinfo, infile);
          (void) jpeg_read_header(&cinfo, TRUE);
          (void) jpeg_start_decompress(&cinfo);
          row_stride = cinfo.output_width * cinfo.output_components;
          total_size= row_stride * cinfo.output_height;


          buffer = (*cinfo.mem->alloc_sarray)
                          ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);



          *px=cinfo.output_width;
          *py=cinfo.output_height;
          *pw=cinfo.output_components;

          *pbuffer= (unsigned char*) malloc (total_size * sizeof (char ));
          buf_tmp=*pbuffer;

          /* read line per line  */

          while (cinfo.output_scanline < cinfo.output_height) {
              (void) jpeg_read_scanlines(&cinfo, buffer, 1);
                memcpy (buf_tmp, buffer[0], row_stride);
                buf_tmp+=row_stride;
              lines++;
              }


          (void) jpeg_finish_decompress(&cinfo);
          jpeg_destroy_decompress(&cinfo);

          fclose(infile);

          return 1;
        }


LoadJpeg::LoadJpeg()
{
	m_buffer = NULL;
}

LoadJpeg::~LoadJpeg()
{
	if(m_buffer) free (m_buffer);
}

int LoadJpeg::readJpegFile (char *filename)
{
	return read_JPEG_file (filename, &m_x, &m_y, &m_w, &m_buffer);
}

char* LoadJpeg::getImageDataFromFile(char *fname, int depth)
{
	char *translated_buffer = NULL;

        if(m_buffer) free (m_buffer);
	m_buffer = NULL;

        if(readJpegFile (fname)==0) {
		return NULL;
	}
        switch(depth) {
                case 24:
                        m_bpl=4; break;
                case 16:
                case 15:
                        m_bpl=2; break;
                default:
                        // in this default, you must allocate colors!
                        m_bpl=1; break;
        }

        translated_buffer=(char *)malloc(m_bpl*m_x*m_y);

        switch(depth) {

                case 8:
                        {
                        int x,y,z,k,pixel;

                        for(z=k=y=0;y!=m_y;y++)
                              for(x=0;x!=m_x;x++) {

                                    // for grayscale-only 8 bit depth
                                    // can't work in 8 bit color display

                                    pixel=(m_buffer[z++]+
                                    m_buffer[z++]+            
                                    m_buffer[z++])/3;

                                    translated_buffer[k++]=pixel;
                              }
                        }
                        break;

                case 16:
                        {
                        unsigned x,y,z,k,r,g,b;
                        unsigned short *word;

                        word=(unsigned short*)translated_buffer;

                        for(z=k=y=0;y!=(unsigned)m_y;y++)
                             for(x=0;x!=(unsigned)m_x;x++) {

                                    // for 16 bit depth, organization 565

                                    r=m_buffer[z++]<<8;
                                    g=m_buffer[z++]<<8;       
                                    b=m_buffer[z++]<<8;

                                    r &= 0xf800;
                                    g &= 0xfc00;                     
                                    b &= 0xf800;

                                    word[k++]=r|g>>5|b>>11;
                              }
                        }
                        break;

                case 32:
                case 24:
                        {
                        unsigned x,y,z,k;
                        //unsigned buffer;

                        //translated_buffer;

                        for(z=k=y=0;y!=(unsigned)m_y;y++)
                             for(x=0;x!=(unsigned)m_x;x++) {

                                   // for 24 bit depth, organization BGRX

                                   translated_buffer[k+0]=m_buffer[z+2];
                                   translated_buffer[k+1]=m_buffer[z+1];
                                   translated_buffer[k+2]=m_buffer[z+0];
                                   k+=4; z+=3;
                             }
                        }
                        break;

                default:
                        fprintf(stderr, "unsuported depth for now.");
                        break;

        }
	return translated_buffer; 
}

