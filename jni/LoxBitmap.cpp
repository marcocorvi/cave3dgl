#include "LoxBitmap.h"

#include <stdio.h>
#include <stdlib.h>

#include "Image_PNG.h"
#include "Image_JPG.h"
#include "Log.h"

void 
LoxBitmap::GetRGB( float e, float n, float & r, float & g, float & b )
{
  double id, jd;
  ENtoIJ( e, n, id, jd );
  int i = (int)id;
  int j = (int)jd;
  r = b = g = 1.0f;
  if ( i < 0 || i >= width || j < 0 || j >= height ) return;
  r = red[ j*width+i ] / 255.0f;
  g = green[ j*width+i ] / 255.0f;
  b = blue[ j*width+i ] / 255.0f;
}

void
LoxBitmap::Data2RGB()
{
  FILE * fp = tmpfile();
  if ( fp == NULL ) {
    LOGW("Cannot create temp file");
    return;
  }
  fwrite( data, 1, size, fp );
  rewind( fp );

  Image * img = NULL;
  if ( type == 0 ) {
    img = new Image_JPG();
  } else if ( type == 1 ) {
    img = new Image_PNG();
  }
  if ( img != NULL ) {
    img->open( fp );
    width  = img->width();
    height = img->height();
    unsigned char * image = img->image();
    for ( int j=0; j<height; ++j ) {
      for ( int i=0; i<width; ++i ) {
        red[ j*width + i ]   = image[ 3*(j*width+i) + 0 ];
        green[ j*width + i ] = image[ 3*(j*width+i) + 1 ];
        blue[ j*width + i ]  = image[ 3*(j*width+i) + 2 ];
      }
    }
    delete img;
  }
}      
