#include "Geometry.h"

#include "Log.h"
#include "Functions.h"


void 
Geometry::CopyBBox( const Geometry * geom )
{
  // LOGI("copy BBox");
  xmax = geom->xmax;  xmin = geom->xmin;
  ymax = geom->ymax;  ymin = geom->ymin;
  zmax = geom->zmax;  zmin = geom->zmin;
  xc = geom->xc;
  yc = geom->yc;
  zc = geom->zc;
}

void 
Geometry::CopySpec( const Geometry * geom )
{
  // LOGI("copy Spec");
  vertexStride = geom->vertexStride;
  nVertex = geom->nVertex;
  nLIndex = geom->nLIndex;
  nSIndex = geom->nSIndex;
  nPos    = geom->nPos;
  nCol    = geom->nCol;
  nTex    = geom->nTex;
  nStations = geom->nStations;
  gVertex  = geom->gVertex;
  gLIndex  = geom->gLIndex;
  gSIndex  = geom->gSIndex;
}

void 
Geometry::InitBBox( bool moveVertexes )
{
  float * v = (float *)gVertex;
  // LOGI("init BBox vertex %p nr %d ", gVertex, nVertex);
  if ( moveVertexes ) {
    xmin = xmax = v[0];
    ymin = ymax = v[1];
    zmin = zmax = v[2];
  } else {
    xmin = xmax = ymin = ymax = zmin = zmax = 0;
  }

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

  if ( moveVertexes ) {
    xc = ( xmin + xmax ) / 2;
    yc = ( ymin + ymax ) / 2;
    zc = ( zmin + zmax ) / 2;

    // OFFSET vertexes by the center (xc, yc, zc)
    for ( int k=0; k<nVertex; ++k ) {
      v[3*k+0] -= xc;
      v[3*k+1] -= yc;
      v[3*k+2] -= zc;
    }
  } else {
    xc = 0;
    yc = 0;
    zc = 0;
    if ( xmax < -xmin ) { xmax = -xmin; } else { xmin = -xmax; }
    if ( ymax < -ymin ) { ymax = -ymin; } else { ymin = -ymax; }
    if ( zmax < -zmin ) { zmax = -zmin; } else { zmin = -zmax; }
  }
  zc   += Z_OFFSET;
  zmax += Z_OFFSET;
  // zmin += Z_OFFSET;

  LOGI("BBox X %.6f %.6f   Y %.6f %.6f   Z %.6f %.6f C %.6f %.6f %.6f", xmin, xmax, ymin, ymax, zmin, zmax, xc, yc, zc );
}

    

