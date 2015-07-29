#include "TherionSurface.h"

#include "TherionFlip.h"
#include "Log.h"

/**
 *        ^
 * d2 = 2 |-----------+P2 = (e2,n2)
 *        |   |   |   |
 *      1 |---+---+---|
 *        |P1 |   |   |
 *      0 +------------->
 *        0   1   2   3 = d1
 *
 * d1 number of centers of the grid in X (east) direction
 * d2 number of centers of the grid in Y (north) direction
 *
 * mZ[ Y * n1 + X ] is the elevation of point
 *    E = e1 + X * mDim1
 *    N = n1 + Y * mDim2
 */
TherionSurface::TherionSurface( float e1, float n1, float e2, float n2, int d1, int d2 )
  : mEast1( e1 )
  , mNorth1( n1 ) 
  , mEast2( e2 )
  , mNorth2(n2 )
  , mNr1( d1 )
  , mNr2( d2 )
  , mDim1( (e2-e1)/(mNr1-1) )
  , mDim2( (n2-n1)/(mNr2-1) )
{
  // LOGI("Therion Surface %d x %d", mNr1, mNr2 );
  mZ = new float[ mNr1*mNr2 ]; 
}


float 
TherionSurface::ComputeZ( float e, float n )
{
  if ( e < mEast1 || n < mNorth1 || e > mEast2 || n > mNorth2 ) return -9999.0;
  int i1 = (int)((e-mEast1)/mDim1);
  int j1 = (int)((n-mNorth1)/mDim2);
  float dx2 = e - ( mEast1 + i1 * mDim1 );
  float dx1 = 1.0 - dx2;
  float dy2 = n - ( mNorth1 + j1 * mDim2 );
  float dy1 = 1.0 - dy2;
  int i2 = i1 + 1;
  int j2 = j1 + 1;
  return ( j2 < mNr2 ) ?
      ( (i2 < mNr1 )? mZ[j1*mNr1+i1] * dx1 + mZ[j1*mNr1+i2] * dx2 : mZ[j1*mNr1+i1] ) * dy1 
    + ( (i2 < mNr1 )? mZ[j2*mNr1+i1] * dx1 + mZ[j2*mNr1+i2] * dx2 : mZ[j2*mNr1+i1] ) * dy2
    : ( (i2 < mNr1 )? mZ[j1*mNr1+i1] * dx1 + mZ[j1*mNr1+i2] * dx2 : mZ[j1*mNr1+i1] ) ;
}
 

// the DEM is stored as
//    (e1,n1)   (e1+1,n1)   ... (e2,n1) 
//    (e1,n1+1) (e1+1,n1+1) ... (e2,n1+1)
//    ...
// with no flip the storing is straightforward
// with flip horizontal storing is by-row but
//   each row is filled from e2 to e1 backward
// with flip vertical the rows of the matrix are filled
//   from the bottom (n2) to the top (n1)
//   each row being filled left (e1) to right (e2)
//
void 
TherionSurface::ReadGridData( float units, int flip, FILE * br )
{
  // LOGI("Theiron Surface read grid data (flip %d)", flip );
  int x, y;
  int x1 = 0;
  int x2 = mNr1;
  int dx = 1;
  int y1 = mNr2-1;
  int y2 = -1;
  int dy = -1;
  if ( flip == FLIP_HORIZONTAL ) {
    x1 = mNr1-1;
    x2 = -1;
    dx = -1;
  } else if ( flip == FLIP_VERTICAL ) {
    y1 = 0;
    y2 = mNr2;
    dy = 1;
  }
  x = x1;
  y = y1;

  char line[4096];
  int size = 4096;
  std::vector< const char * > vals;

  while ( y != y2 ) {
    char * ch = fgets( line, size, br );
    if ( ch == NULL ) {
      // ERROR
      break;
    }
    ch = Trim( ch );
    RemoveComment( ch );
    if ( *ch == 0 ) continue;
    int vals_length = SplitOnSpaces( ch, vals );
    if ( vals.size() > 0 ) {
      int idx = 0;
      if ( EQUALS( vals[idx], "grid_flip" ) ) {
        if ( ++idx < vals_length ) {
          flip = ParseFlip( vals[idx] );
          x1 = 0;
          x2 = mNr1;
          dx = 1;
          y1 = mNr2-1;
          y2 = -1;
          dy = -1;
          if ( flip == FLIP_HORIZONTAL ) {
            x1 = mNr1-1;
            x2 = -1;
            dx = -1;
          } else if ( FLIP_VERTICAL ) {
            y1 = 0;
            y2 = mNr2;
            dy = 1;
          }
          x = x1;
          y = y1;
        } 
      } else if ( EQUALS( vals[idx], "grid-units" ) ) {
        // FIXME TODO units not parsed yet
      } else { // data
        while ( idx < vals_length ) {
          mZ[ y*mNr1+x ] = TO_FLOAT( vals[idx] );
          x += dx;
          if ( x == x2 ) {
            x = x1;
            y += dy;
            if ( y == y2 ) break;
          }
          idx++;
        }
      }
    }
  }
}

