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

#include "Functions.h"

SplayPoints::SplayPoints( Model * model, Shader * shader )
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
  nSIndex = therion_nx;
  CopyBBox( model );
  CopySpec( model );
  vertex = (float *)(model->Vertex());
  index = (unsigned short *)(model->SIndex());

  SetVertex( vertex );
  SetSIndex( index );
  SetNVertex( nVertex ); // number of vertexes
  SetNSIndex( nSIndex ); // number of indices used to render
  
  SetName( "splay_points" );
  SetNPos( 3 );
  SetVertexStride( sizeof(float) * 3 );

  SetGeometry( this );
  SetShader( shader );

  // int ns = model->NSIndex();
  // for ( int k=0; k<ns; ++ k ) {
  //   int ik = index[k];
  //   LOGI("V[%d]: %.6f %.6f %.6f", ik, vertex[3*ik+0], vertex[3*ik+1], vertex[3*ik+2] );
  // }
}

void 
SplayPoints::UpdateDataBuffers( Model * model )
{
  int nv = model->therion_ns + model->therion_nx; // number of vertexes
  int ni = model->therion_nx;                     // number of indexes

  vertex = (float *)(model->Vertex()); // + 3 * model->therion_ns; 
  LOGI("SplayPoints update data buffer (nx %d -> %d) nIndex %d st %d leg %d scale %.2f",
    therion_nx, model->therion_nx, nSIndex, model->therion_ns, model->therion_nl, model->scale );

  index = (unsigned short *)(model->SIndex());
  nVertex = model->NVertex();
  nSIndex = model->NSIndex();
  therion_nx = nSIndex;

  SetVertex(  vertex );
  SetNVertex( nVertex );
  SetSIndex(  index );
  SetNSIndex( nSIndex );

  CopyBBox( model ); 
  // LOGI("update done");
}

SplayPoints::~SplayPoints() 
{
}

