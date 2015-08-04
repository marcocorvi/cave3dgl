#include "TherionBitmap.h"

#include <stdlib.h>
#include <ctype.h>

#include "Image_PNG.h"
#include "Image_JPG.h"
#include "Log.h"

// desc : bitmap description
// bool 
// TherionBitmap::ParseDescription( const char * desc )
// {
//   if ( desc == NULL ) return false;
//   initialized = false;
//   const char * ch = desc;
//   while ( *ch != 0 && isspace( *ch ) ) ++ch;
//   if ( *ch == 0 ) return false;
//   const char * ch1 = ch+1;
//   while ( *ch1 != 0 && ! isspace( *ch1 ) ) ++ch1;
//   if ( *(ch1-1) == '[' ) --ch1;
//   int len = ch1 - ch + 1;
//   char * tmp = new char[ len ];
//   memcpy( tmp, ch, len-1 );
//   tmp[len] = 0;
// 
//   ch = ch1;
//   while ( *ch != 0 && isspace( *ch ) ) ++ch;
//   if ( *ch != '[' ) return false;
//   ++ch;
//   ch = ParseFloat( ch, x0, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, y0, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, e0, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, n0, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, x1, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, y1, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, e1, false ); if ( ch == NULL ) return false;
//   ch = ParseFloat( ch, n1, true  ); if ( ch == NULL ) return false;
// 
//   LOG("Bitmap %s %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f", tmp, x0, y0, e0, n0, x1, y1, e1, n1 );
// 
//   filename = tmp;
//   delete[] tmp;
//   return true;
// }

// const char * 
// TherionBitmap::ParseFloat( const char * ch, float & val, bool last )
// {
//   while ( *ch != 0 && isspace( *ch ) ) ++ch;
//   if ( *ch == 0 ) return NULL;
//   bool  inFrac = false;
//   float frac = 1;
//   val = 0;
//   while ( *ch != 0 && ! isspace( *ch ) ) {
//     if ( *ch == '.' ) {
//       if ( inFrac ) return NULL;
//       inFrac = true;
//       continue;
//     }
//     if ( last && *ch == ']' ) {
//       ++ch;
//       break;
//     }
//     if ( *ch < '0' || *ch > '9' ) return false;
//     if ( in Frac ) {
//       frac /= 10.0f;
//       val += frac * ( *ch - '0' );
//     } else {
//       val = val * 10 + ( *ch - '0' );
//     }
//   }
//   return ch;
// }

void 
TherionBitmap::GetRGB( float e, float n, float & r, float & g, float & b )
{
  float x = x0 + (x1-x0) * ( e - e0 )/( e1 - e0 );
  float y = y0 + (y1-y0) * ( n - n0 )/( n1 - n0 );
  if ( x < 0 || x > w-1 || y < 0 || y > h-1) {
    r = g = b = 1.0f;
  } else {
    int ix = (int)x;
    int iy = (int)y;
    r = red[iy * w + ix] / 255.0;
    g = green[iy * w + ix] / 255.0;
    b = blue[iy * w + ix] / 255.0;
  }
}

bool
TherionBitmap::ReadImage( const char * filename )
{
  if ( filename == NULL ) return false;
  size_t len = strlen( filename );
  if ( strcmp( filename+len-3, "png" ) == 0 ) return ReadPNGImage( filename );
  if ( strcmp( filename+len-3, "jpg" ) == 0 ) return ReadJPEGImage( filename );
  if ( strcmp( filename+len-4, "jpeg" ) == 0 ) return ReadJPEGImage( filename );
  LOGW("Unsupported file type \"%s\"\n", filename );
  return false;
}

bool
TherionBitmap::ReadPNGImage( const char * filename )
{
  Image_PNG png;
  if ( ! png.open( filename ) ) {
    return false;
  }
  // LOGI("PNG %d %d stride %d BPP %d", png.width(), png.height(), png.stride(), png.BPP() );

  w = png.width();
  h = png.height();
  unsigned char * img = png.image();
  if ( png.BPP() == 3 ) {
    red   = new unsigned char[ w * h ];
    green = new unsigned char[ w * h ];
    blue  = new unsigned char[ w * h ];
    for ( int k = 0; k < w*h; ++k ) {
      red[k]   = img[ 3*k + 0 ];
      green[k] = img[ 3*k + 1 ];
      blue[k]  = img[ 3*k + 2 ];
    }
    return true;
  } else if ( png.BPP() == 1 ) {
    red   = new unsigned char[ w * h ];
    green = new unsigned char[ w * h ];
    blue  = new unsigned char[ w * h ];
    for ( int k = 0; k < w*h; ++k ) {
      red[k]   = img[ k ];
      green[k] = img[ k ];
      blue[k]  = img[ k ];
    }
    return true;
  }
  return false;
}

bool
TherionBitmap::ReadJPEGImage( const char * filename )
{
  Image_JPG jpg;
  if ( ! jpg.open( filename ) ) {
    return false;
  }
  // LOGI("JPG %d %d stride %d BPP %d", jpg.width(), jpg.height(), jpg.stride(), jpg.BPP() );

  w = jpg.width();
  h = jpg.height();
  unsigned char * img = jpg.image();
  if ( jpg.BPP() == 3 ) {
    red   = new unsigned char[ w * h ];
    green = new unsigned char[ w * h ];
    blue  = new unsigned char[ w * h ];
    for ( int k = 0; k < w*h; ++k ) {
      red[k]   = img[ 3*k + 0 ];
      green[k] = img[ 3*k + 1 ];
      blue[k]  = img[ 3*k + 2 ];
    }
    return true;
  } else if ( jpg.BPP() == 1 ) {
    red   = new unsigned char[ w * h ];
    green = new unsigned char[ w * h ];
    blue  = new unsigned char[ w * h ];
    for ( int k = 0; k < w*h; ++k ) {
      red[k]   = img[ k ];
      green[k] = img[ k ];
      blue[k]  = img[ k ];
    }
    return true;
  }
  return false;
}

