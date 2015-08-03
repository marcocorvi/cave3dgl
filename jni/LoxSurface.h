#ifndef LOX_SURFACE_H
#define LOX_SURFACE_H

#include <stdint.h>

class LoxSurface
{
  private:
    uint32_t id;
    uint32_t ww;
    uint32_t hh;
    double calib[6];
    double * grid;

  public:
    float East1() const  { return (float)(calib[0]); }
    float North1() const { return (float)(calib[1]); }
    int NrEast() const { return ww; }
    int NrNorth() const { return hh; }
    float DimEast() const  { return (float)(calib[2]); }
    float DimNorth() const { return (float)(calib[5]); }

    LoxSurface( uint32_t _id, uint32_t w, uint32_t h, double * c, double * g )
      : id( _id )
      , ww( w )
      , hh( h )
      , grid( g )
    {
      for ( int k=0; k<6; ++k ) calib[k] = c[k];
    }

    uint32_t Id() const     { return id; }
    uint32_t Width() const  { return ww; }
    uint32_t Height() const { return hh; }

    double Calib( int k ) const { return calib[k]; }
    double Z( int i, int j ) const { return grid[ j * ww + i ]; }
    double * Grid() { return grid; }

};

#endif
