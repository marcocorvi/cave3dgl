#include "Image_PNG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>
#include "../../libpng-android/jni/png.h"

#include "MemorySource.h"
#include "Log.h"

#define NUMBER 4

bool
Image_PNG::open(const char * filename)
{
  FILE *fp;

  if ( (fp = fopen (filename, "rb")) == NULL) {
    LOGW("Image_PNG: unable to open file: %s\n", filename);
    return false;
  }
  bool ret = open( fp );
  fclose( fp );
  return ret;
}

void read_fct( png_structp png, png_bytep outbuf, png_size_t cnt )
{
  png_voidp io = png_get_io_ptr( png );
  MemorySource * psrc = (MemorySource*)io;
  size_t ret = psrc->read( (unsigned char *)outbuf, (size_t)cnt );
  if ( ret != (size_t)cnt ) {
    LOGW("MemorySource read error: read %d to read %d", ret, cnt );
  }
}

bool
Image_PNG::open( unsigned char * data, size_t size )
{
  // LOGI("Image PNG data size %d", size );
  bool ret = false;
  MemorySource src( data, size );
    
  png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
  if ( png_ptr == NULL ) return false;
  png_infop info_ptr = png_create_info_struct( png_ptr );
  if ( info_ptr == NULL ) {
    png_destroy_read_struct( &png_ptr, NULL, NULL );
    return false;
  }
  png_set_read_fn( png_ptr, &src, read_fct );

  // unsigned char png_sig[NUMBER];
  // src.read( png_sig, NUMBER );
  // png_set_sig_bytes( png_ptr, NUMBER );

  // png_read_info( png_ptr, info_ptr );
  // _width = 0;
  // _height = 0;
  // int bitDepth = 0; // bits per channel
  // int colType = -1;
  // png_uint_32 retval = png_getIHDR( png_ptr, indo, &_width, &_height, &bitDepth, &colType, NULL, NULL, Null );
  // if ( retvap != 1 ) {
  //   png_destroy_read_struct( &png_ptr, NULL, NULL );
  //   return false;
  // }

  ret = readPNGdata( png_ptr, info_ptr, 1 );

  png_destroy_read_struct( &png_ptr, NULL, NULL );

  return ret;
}

bool
Image_PNG::open( FILE * fp ) 
{
  bool ret = false;
  /* 
  png_byte buf[NUMBER]; // png needs 4 bytes to check
  if ((fread(buf, 1, NUMBER, fp) != NUMBER) || 
      (! png_sig_cmp(buf, (png_size_t)0, NUMBER))) {
    throw;
  }
  */
  
  png_structp png_ptr = png_create_read_struct (
    PNG_LIBPNG_VER_STRING,
    NULL,  // (png_voidp)user_error_ptr,
    NULL,  // user_error_fn,
    NULL   // user_warning_fn
  );
  if ( ! png_ptr ) return false;
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    return false;
  }

  png_init_io(png_ptr, fp);
  // png_set_sig_bytes(png_ptr, NUMBER);

  ret = readPNGdata( png_ptr, info_ptr, 1 );

  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

  return ret;
}

bool
Image_PNG::readPNGdata( png_structp png_ptr, png_infop info_ptr, int mode )
{
  // LOGI("PNG read png data, mode %d", mode );
  _bpp = 1; // bytes per pixel

  png_bytep * row_pointers;
  if ( mode == 1 ) {
    png_read_png( png_ptr, info_ptr, 
      PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_PACKING,
      (png_voidp)NULL);
    _width  = png_get_image_width( png_ptr, info_ptr );
    _height = png_get_image_height( png_ptr, info_ptr );
    int bits = png_get_bit_depth( png_ptr, info_ptr );
    if ( bits != 8 ) {
      LOGW("unsupported PNG bits %d", bits );
      return false;
    }
    int color = png_get_color_type( png_ptr, info_ptr );
    switch ( color ) {
      case PNG_COLOR_TYPE_GRAY:       _bpp=1; break;
      case PNG_COLOR_TYPE_GRAY_ALPHA: _bpp=2; break;
      case PNG_COLOR_TYPE_RGB:        _bpp=3; break;
      case PNG_COLOR_TYPE_RGB_ALPHA:  _bpp=4; break;
      default:
        LOGW("unsupported PNG color %d ", color );
        return false;
    }
    row_pointers = png_get_rows(png_ptr, info_ptr);
  } else {
    png_read_info(png_ptr, info_ptr);
    // _width  = png_get_image_width(png_ptr, info_ptr);
    // _height = png_get_image_height(png_ptr, info_ptr);
    int bitDepth;
    int colType;
    png_get_IHDR( png_ptr, info_ptr, &_width, &_height, &bitDepth, &colType, NULL, NULL, NULL );
    // LOGI("%d x %d bitDepth %d colType %d", _width, _height, bitDepth, colType );

    row_pointers = (png_bytep *)png_malloc( png_ptr, _height * sizeof(png_bytep));
    // png_bytep row_pointers[ _height ];
    for (int i=0; i<_height; i++) {
      row_pointers[i] = (png_bytep)png_malloc( png_ptr, png_get_rowbytes(png_ptr, info_ptr) );
      // row_pointers[i] = (png_bytep)png_malloc(png_ptr, _width * pixel_size );
    }
    png_read_image(png_ptr, row_pointers);

    // should destroy row_pointers afterwards
    // png_destroy_???
  }

  _stride = _bpp * _width;
  _img = (unsigned char *)malloc( _stride * _height );
  for ( unsigned int j=0; j<_height; j++) {
    for( unsigned int i=0; i<_stride; i++) {
      _img[j*_stride+i] = row_pointers[j][i];
    }
  }
  _owner = true;
  // LOGI("PNG %d x %d stride %d BPP %d", _width, _height, _stride, _bpp );
  return true;
}


// bool 
// Image_PNG::save_image (const char * filename)
// {
//   FILE * fp;
// 
//   if ( (fp = fopen (filename, "wb")) == NULL) {
//     fprintf(stderr, "Image_PNG: unable to write file %s\n", filename);
//     return false;
//   }
// 
//   // png_colorp palette;
// 
//   png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING,
//       NULL, // user_error_ptr,
//       NULL, // user_error_fn,
//       NULL  // user_warning_fn
//   );
//   if (png_ptr == NULL) {
//     fclose(fp);
//     return false;
//   }
// 
//   png_infop info_ptr = png_create_info_struct(png_ptr);
//   if (!info_ptr) {
//     fclose(fp);
//     png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
//     return false;
//   }
// 
//   if (setjmp(png_jmpbuf(png_ptr))) {
//     fclose(fp);
//     png_destroy_write_struct(&png_ptr, &info_ptr);
//     return false;
//   }
// 
//   png_init_io(png_ptr, fp);
//   png_set_filter(png_ptr, 0, PNG_FILTER_NONE );
//   // png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
//   // png_set_compression_mem_level(png_ptr, 8);
//   // png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
//   // png_set_compression_window_bits(png_ptr, 15);
//   // png_set_compression_method(png_ptr, 8);
//   // png_set_compression_buffer_size(png_ptr, 8192)
// 
//   png_bytep row_pointers[_height];
//   for (unsigned int k = 0; k < _height; k++)
//      row_pointers[k] = _img + k*_width;
//   png_set_rows(png_ptr, info_ptr, row_pointers);
// 
// 
//   png_set_IHDR(png_ptr, info_ptr, 
//        _width,
//        _height,
//        8,                            // bit_depth,
//        PNG_COLOR_TYPE_GRAY,          // color_type,
//        PNG_INTERLACE_NONE,           // interlace_type,
//        PNG_COMPRESSION_TYPE_DEFAULT, // compression_type,
//        PNG_FILTER_TYPE_DEFAULT       // filter_method
//   );
// 
//   png_write_png(png_ptr, info_ptr, 
//        PNG_TRANSFORM_IDENTITY, // png_transforms,
//        (png_voidp)NULL
//   );
// 
//   /*
//   png_free(png_ptr, palette);
//   palette=NULL;
//   png_free(png_ptr, trans);
//   trans=NULL;
//   */
// 
//   png_destroy_write_struct(&png_ptr, &info_ptr);
//   fclose(fp);
//       
//   return true;
// }

// =====================================================================
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
//   Image_PNG im (argv[1]);
//   GUI gui (1, im.width (), im.height () );
//   gui.displayImage (im.image (), im.width (), im.height (), 0, 0, 1);
//   gui.waitKeyPress ();
//   im.save_image (argv[2]);
//   return 0;
// }
// #endif
