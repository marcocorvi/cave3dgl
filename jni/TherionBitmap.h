#ifndef THERION_BITMAP_H
#define THERION_BITMAP_H

#include <stdlib.h>

class TherionBitmap
{
  private:
    int   x0, y0;
    float e0, n0;
    int   x1, y1;
    float e1, n1;
    bool  initialized;
    int w, h;
    unsigned char * red;
    unsigned char * green;
    unsigned char * blue;

  public:
    TherionBitmap( )
      : initialized( false )
      , red( NULL )
      , green( NULL )
      , blue( NULL )
    { }

    ~TherionBitmap()
    {
      if ( red )   delete[] red;
      if ( green ) delete[] green;
      if ( blue )  delete[] blue;
    }
    
    // desc : bitmap description
    // bool ParseDescription( const char * desc );

    bool Load( const char * filename, int x00, int y00, float e00, float n00,
                                              int x11, int y11, float e11, float n11 )
    {
      initialized = ReadImage( filename );
      if ( initialized ) {
        x0 = x00;
        y0 = y00;
        e0 = e00;
        n0 = n00;
        x1 = x11;
        y1 = y11;
        e1 = e11;
        n1 = n11;
      }
      return initialized;
    }

    void GetRGB( float e, float n, float & r, float & g, float & b );

  private:
    bool ReadImage( const char * filename );
    bool ReadPNGImage( const char * filename );
    bool ReadJPEGImage( const char * filename );
  
  //   const char * ParseFloat( const char * ch, float & val, bool last );
          
};

#endif
