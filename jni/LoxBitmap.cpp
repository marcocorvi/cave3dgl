#include "LoxBitmap.h"

#include <stdio.h>
#include <stdlib.h>

#include "Image_PNG.h"
#include "Image_JPG.h"
#include "Log.h"

LoxBitmap::~LoxBitmap()
{
  if ( red )   delete[] red;
  if ( green ) delete[] green;
  if ( blue )  delete[] blue;
}

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

bool isPNG( unsigned char * data )
{
  return data[0] == 0x89 
      && data[1] == 0x50
      && data[2] == 0x4e
      && data[3] == 0x47;
}

bool isJPG( unsigned char * data ) 
{
  return data[4] == 0x00 
      && data[5] == 0x10
      && data[6] == 0x4a
      && data[7] == 0x46;
}

void
LoxBitmap::Data2RGB()
{
  LOGI("Lox BItmap data2rgb type %d", type);
  // FILE * fp = tmpfile();
  // if ( fp == NULL ) {
  //   LOGW("Cannot create temp file");
  //   return;
  // }
  // fwrite( data, 1, size, fp );
  // rewind( fp );

  Image * img = NULL;
  if ( isJPG( data ) ) {
    img = new Image_JPG();
  } else if ( isPNG( data ) ) {
    img = new Image_PNG();
  } else {
    LOGW("Unexpected image type %d", type );
  }
  if ( img != NULL && img->open( data, size ) ) {
    width  = img->width();
    height = img->height();
    LOGI("Lox bitmap image %dx%d stride %d BPP %d", width, height, img->stride(), img->BPP() );
    unsigned char * image = img->image();
    red   = new unsigned char[ width * height ];
    green = new unsigned char[ width * height ];
    blue  = new unsigned char[ width * height ];
    for ( int j=0; j<height; ++j ) {
      for ( int i=0; i<width; ++i ) {
        red[ j*width + i ]   = image[ 3*(j*width+i) + 0 ];
        green[ j*width + i ] = image[ 3*(j*width+i) + 1 ];
        blue[ j*width + i ]  = image[ 3*(j*width+i) + 2 ];
      }
    }
    delete img;
  } else {
    LOGW("LoxBitmap failed to read image data");
  }
  LOGI("Lox bitmap %dx%d ", width, height );
}      
