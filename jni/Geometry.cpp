#include "Geometry.h"

#include "Log.h"


void 
Geometry::InitBBox()
{
  float * v = (float *)vertex;
  // LOGI("Geometry::InitBBox()");
  xmin = xmax = v[0];
  ymin = ymax = v[1];
  zmin = zmax = v[2];
  for ( int k=1; k<nVertex; ++k ) {
    float x = v[3*k + 0];
    float y = v[3*k + 1];
    float z = v[3*k + 2];
    if ( x < xmin ) { xmin = x; }
    else if ( x > xmax ) { xmax = x; }
    if ( y < ymin ) { ymin = y; }
    else if ( y > ymax ) { ymax = y; }
    if ( z < zmin ) { zmin = z; }
    else if ( z > zmax ) { zmax = z; }
  }
#if 1
  xc = ( xmin + xmax ) / 2;
  yc = ( ymin + ymax ) / 2;
  zc = ( zmin + zmax ) / 2;

  for ( int k=0; k<nVertex; ++k ) {
    v[3*k+0] -= xc;
    v[3*k+1] -= yc;
    v[3*k+2] -= zc;
  }
#else
  xc = 0.0f;
  yc = 0.0f;
  zc = 0.0f;
#endif
}
    

