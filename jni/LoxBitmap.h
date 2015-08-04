#ifndef LOX_BITMAP_H
#define LOX_BITMAP_H

#include <stdlib.h>
#include <stdint.h>

#include "Log.h"

class LoxBitmap
{
  private:
    uint32_t sid; // surface
    uint32_t type;
    uint32_t size;
    double calib[6];
    double det; // calib det
    unsigned char * data;

    int width;
    int height;
    unsigned char * red;
    unsigned char * green;
    unsigned char * blue;

  public:
    LoxBitmap( uint32_t _sid, uint32_t tp, uint32_t sz, double * c, unsigned char * d )
      : sid( _sid )
      , type( tp )
      , size( sz )
      , data( d )
      , width( 0 )
      , height( 0 )
      , red( NULL )
      , green( NULL )
      , blue( NULL )
    {
      for ( int k=0; k<6; ++k ) calib[k]= c[k];
      det = calib[2] * calib[5] - calib[3] * calib[4];
      Data2RGB();
      // LOGI("Bitmap calib %.2f %.2f %.2f   %.2f %.2f %.2f", c[0], c[2], c[3], c[1], c[4], c[5] );
    }

    ~LoxBitmap();

    uint32_t Surface() const { return sid; }
    uint32_t Type() const { return type; }
    double Calib( int k ) const { return calib[k]; }

    void GetRGB( float e, float n, float & r, float & g, float & b );

  private:
    void ENtoIJ( double e, double n, double & i, double & j )
    {
      e -= calib[0];
      n -= calib[1];
      i = ( calib[5] * e - calib[3] * n )/det;
      j = ( calib[2] * n - calib[4] * e )/det;
    }
    uint32_t DataSize() const { return size; }
    unsigned char * Data() { return data; }

    void Data2RGB();

};

#endif
