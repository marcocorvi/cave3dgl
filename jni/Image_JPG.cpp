#include "Image_JPG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// #include "cdjpeg.h"     /* Common decls for cjpeg/djpeg applications */
// #include "jversion.h"       /* for version message */
extern "C" {
  #include "../../libjpeg-android/jni/jpeglib.h"
}

#include "Log.h"

bool
Image_JPG::open(const char * filename)
{
  FILE * fp;
  if ( (fp = fopen (filename, "rb")) == NULL) {
    LOGI("Image_pgm: unable to open file: %s\n", filename);
    return false;
  }
  bool ret = open( fp );
  fclose( fp );
  return ret;
}

bool 
Image_JPG::open( unsigned char * data, size_t size )
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_mem_src(&cinfo, data, size );
  (void) jpeg_read_header(&cinfo, TRUE);

  _height = cinfo.image_height;
  _width  = cinfo.image_width;
  _bpp    = cinfo.num_components;
  _stride = _width * _bpp;
  _img = (unsigned char *)malloc( _stride * _height );
  // LOGI("JPEG %dx%d stride %d BPP %d", _width, _height, _stride, _bpp );

  // djpeg_dest_ptr dest_mgr = NULL;
  // dest_mgr = jinit_write_bmp(&cinfo, FALSE);

  (void) jpeg_start_decompress(&cinfo);
  JSAMPLE * lines[_height];
  for (unsigned int j=0; j<_height; j++) 
    lines[j] = _img + j*_stride;
  unsigned int n_lines = 0;
  while (n_lines < _height) {
    int n = jpeg_read_scanlines(&cinfo, 
                    lines + n_lines,     // dest_mgr->buffer,
                    (_height - n_lines)  // dest_mgr->buffer_height
            );
    // (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    n_lines += n;
  }

  // (*dest_mgr->finish_output) (&cinfo, dest_mgr);
  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  _owner = true;
  return true;
}

bool
Image_JPG::open( FILE * fp )
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, fp);
  (void) jpeg_read_header(&cinfo, TRUE);

  _height = cinfo.image_height;
  _width  = cinfo.image_width;
  _bpp    = cinfo.num_components;
  _stride = _width * _bpp;
  _img = (unsigned char *)malloc( _stride * _height );

  // djpeg_dest_ptr dest_mgr = NULL;
  // dest_mgr = jinit_write_bmp(&cinfo, FALSE);

  (void) jpeg_start_decompress(&cinfo);
  JSAMPLE * lines[_height];
  for (unsigned int j=0; j<_height; j++) 
    lines[j] = _img + j*_stride;
  unsigned int n_lines = 0;
  while (n_lines < _height) {
    int n = jpeg_read_scanlines(&cinfo, 
                    lines + n_lines,     // dest_mgr->buffer,
                    (_height - n_lines)  // dest_mgr->buffer_height
            );
    // (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    n_lines += n;
  }

  // (*dest_mgr->finish_output) (&cinfo, dest_mgr);
  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  _owner = true;
  return true;
}


// bool 
// Image_JPG::save_image (const char * filename)
// {
//   FILE *  fp;
// 
//   if ( (fp = fopen (filename, "wb")) == NULL) {
//     fprintf(stderr, "Image_pgm: unable to write file %s\n", filename);
//     return false;
//   }
// 
//   struct jpeg_error_mgr jerr;
//   struct jpeg_compress_struct cinfo;
//   cinfo.err = jpeg_std_error( & jerr );
//   jpeg_create_compress( & cinfo );
// 
//   jpeg_stdio_dest( &cinfo, fp );
//   cinfo.image_width  = _width;
//   cinfo.image_height = _height;
//   cinfo.input_components = 1; // one sample per pixel;
//   cinfo.in_color_space = JCS_GRAYSCALE;
//   // cinfo.in_color_space = JCS_RGB;
// 
//   jpeg_set_defaults( & cinfo );
//   jpeg_start_compress( & cinfo, TRUE );
// 
//   int stride = cinfo.image_width; // * 3 if RGB
//   JSAMPROW row[1];
//   while ( cinfo.next_scanline < cinfo.image_height ) {
//     row[0] = & (_img[cinfo.next_scanline * stride ]);
//     jpeg_write_scanlines( & cinfo, row, 1 );
//   }
// 
//   jpeg_finish_compress( & cinfo );
//   
//   fclose (fp);
// 
//   return true;
// }

// ==================================================================
// #ifdef TEST
// #include "gui++.h"
// 
// int
// main (int argc, char **argv)
// {
//   if (argc < 3) {
//     fprintf(stderr, "usage: image_pgm input output\n");
//     exit(0);
//   }
//   Image_JPG im (argv[1]);
//   GUI gui (1, im.width (), im.height (), 1 );
//   gui.displayImage (im.image (), im.width (), im.height (), 0, 0, 1);
//   gui.waitKeyPress ();
//   im.save_image (argv[2]);
//   return 0;
// }
// #endif
