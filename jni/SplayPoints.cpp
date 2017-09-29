#include "SplayPoints.h"

#include <assert.h>

#include "Log.h"

// vertices are Rotated-Z:
// x = - Y_orig
// y = + X_orig
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLES/gl.h>

#define SIGN_X( x ) -((x))
#define SIGN_Y( y ) -((y))
#define SIGN_Z( z ) -((z))


SplayPoints::SplayPoints( TherionModel * model, Shader * shader )
  : Geometry( GL_POINTS, "SplayPoints", FLAG_POINTS )
  , vertex( NULL ) // can use pointer in vertex array of the model
  , index( NULL )  // but point indices must be reset (to skip FROM index)
  , therion_nx( model->therion_nx )
  , x_offset( model->x_offset )
  , y_offset( model->y_offset )
  , z_offset( model->z_offset )
  , scale( model->scale )
{ 
  nVertex = therion_nx;
  nIndex  = therion_nx;
  CopyBBox( model );
  CopySpec( model );
  vertex = (float *)(model->Vertex()) + 3 * model->therion_ns; 
  index = new unsigned short[ nIndex ];

  unsigned short * mindex = (unsigned short *)(model->Index()) + (2 * model->therion_nl + 1);
  unsigned short off = (unsigned short)( model->therion_ns );
  for ( int k=0; k<nIndex; ++k ) index[k] = mindex[2*k] - off;

  SetVertex( vertex );
  SetIndex( index );
  SetNVertex( nVertex ); // number of vertexes
  SetNIndex( nIndex );   // number of indices used to render
  SetName( "splay_points" );
  SetNPos( 3 );
  SetVertexStride( sizeof(float) * 3 );

  SetGeometry( this );
  SetShader( shader );
}

SplayPoints::~SplayPoints() 
{
  if ( index != NULL ) delete[] index;
}

