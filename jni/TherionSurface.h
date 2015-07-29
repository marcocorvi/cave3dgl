#ifndef THERION_SURFACE_H
#define THERION_SURFACE_H

#include <stdio.h>

class TherionSurface
{
  private:
    float mEast1, mNorth1, mEast2, mNorth2;
    float * mZ;  // vertical (upwards)
    int mNr1;     // number of centers in East 
    int mNr2;     // number of centers in North
    float mDim1, mDim2; // spacing between grid centers

  public:
    float East1() const { return mEast1; }
    float East2() const { return mEast2; }

    float North1() const { return mNorth1; }
    float North2() const { return mNorth2; }

    int NrEast() const { return mNr1; }
    int NrNorth() const { return mNr2; }

    float DimEast() const { return mDim1; }
    float DimNorth() const { return mDim2; }

  TherionSurface( float e1, float n1, float e2, float n2, int d1, int d2 );

  ~TherionSurface()
  {
    if ( mZ ) delete[] mZ;
  }

  float ComputeZ( float e, float n );
 
  void ReadGridData( float units, int flip, FILE * br );

  const float * GetZ() const { return mZ; }

};

#endif
