#include "StationTexture.h"

#include <stdlib.h>
#include "Vector3.h"
#include "CharMap.h"
#include "Log.h"

#define COLS 16

    // ns    number of stations
    // geom  model geometry
    // names station names
    // len   max nr chars in station names
StationTexture::StationTexture( int ns, Geometry * geom, const Station * names, unsigned int len )
  : length( len + 1 )
  , texData( NULL )
  , nStation( ns )
  , modelGeometry( geom )
  , mNames( names )
  , vertex( NULL )
  , index( NULL )
  , geometry( GL_TRIANGLES, "stations", FLAG_STATIONS )
{
  bpp = BPP;
  // LOGI("Station Texture cstr: max name length %d, stations %d, BPP %d", length, nStation, bpp );
  name_width = length * (WIDTH+2);
  rows = (nStation + COLS-1) / COLS;
  width  = name_width * COLS;
  height = rows  * HEIGHT;
  table_stride = width * bpp;
  name_stride  = name_width * bpp;

  if ( nStation > 0 ) {
    texData = (unsigned char *)malloc( table_stride * height * sizeof(unsigned char) );
    memset( texData, 0, table_stride * height ); 
    vertex = (float*)malloc( 4 * 5 * nStation * sizeof(float) );      // 4 vertices/label, 5 float/vertex, nStation labels
    index  = (unsigned short *)malloc( 6 * nStation * sizeof(unsigned short) ); // 6 int/label 
  }
  SetTexture( width, height, bpp, texData );
  geometry.SetNVertex( 4 * nStation );
  geometry.SetNSIndex( 6 * nStation );
  geometry.SetVertex( (void *)vertex );
  geometry.SetSIndex( (void *)index );
  geometry.SetNPos( 3 );  // nr. vertex position elements
  geometry.SetNCol( 1 );  // not used
  geometry.SetNTex( 2 );  // nr. texture coord elements
  geometry.SetVertexStride( sizeof(float) * 5 );
  // LOGI("Station Texture nr. stations %d", nStation );
  for ( int k=0; k<nStation; ++k ) {
    if ( mNames[k].name == NULL ) {
      LOGW("WARNING station %d / %d has NULL name", k, nStation );
      break;
    }
    AddStation( k, mNames[k].name );
  }
}

void 
StationTexture::SetStationNames( int ns, const Station * names )
{
  if ( ns > nStation ) {
    mNames = names;
    vertex = (float*)realloc( vertex, 4*5*ns*sizeof(float) );
    index  = (unsigned short *)realloc( index, 6*ns* sizeof(unsigned short) ); // 6 int/label 
    for ( int k=nStation; k<ns; ++k ) {
      AddStation( k, mNames[k].name );
    }
    nStation = ns;
    geometry.SetNVertex( 4 * nStation );
    geometry.SetNSIndex( 6 * nStation );
    geometry.SetVertex( (void *)vertex );
    geometry.SetSIndex( (void *)index );
  }
}

StationTexture::~StationTexture()
{
  if ( texData ) delete[] texData;
  if ( vertex ) delete[] vertex;
  if ( index ) delete[] index;
}

void 
StationTexture::AddStation( int k, const char * str )
{
  // if ( k >= nStation ) return;
  int col = k % COLS;
  int row = k / COLS;
  int len = (length < strlen( str ))? length : strlen(str);

  float h = 1.0f / rows;
  float lw = len * (1.0f / (length * COLS));
  float cw = col * (1.0f / COLS);
  int k20 = k*20;
  int k4  = k*4;
  int k6  = k*6;
  vertex[ k20     + 3 ] = cw;           // (0) ---------- (1) k
  vertex[ k20     + 4 ] = (row+1) * h;  //  |              |
  vertex[ k20 + 5 + 3 ] = cw + lw;      // (2) ---------- (3) k+1
  vertex[ k20 + 5 + 4 ] = (row+1) * h;
  vertex[ k20 +10 + 3 ] = cw;
  vertex[ k20 +10 + 4 ] = (row+0) * h;
  vertex[ k20 +15 + 3 ] = cw + lw;
  vertex[ k20 +15 + 4 ] = (row+0) * h;
  // geom coords are not initialized

  index[k6 + 0] = k4 + 0;
  index[k6 + 1] = k4 + 2;
  index[k6 + 2] = k4 + 1;
  index[k6 + 3] = k4 + 1;
  index[k6 + 4] = k4 + 2;
  index[k6 + 5] = k4 + 3;

  CharMap * chars = CharMap::Instance();
  int xoff = name_stride  * col;
  int yoff = table_stride * row * HEIGHT;
  for ( int k1=0; k1<len; ++k1) {
    unsigned char * shape = chars->Shape( str[k1] );
    for (unsigned char m = 0x80; m > 0; m>>=1) {
      unsigned char * off = texData + yoff + xoff;
      for ( int j=0; j<HEIGHT; ++j ) {
        if ( (m & shape[ j ]) == m ) {
          memset( off, 0, bpp );
          off[0] = 0xff;
          off[bpp-1] = 0xff;
        }
        off += table_stride;
      }
      xoff += bpp;
    }
    xoff += 2 * bpp; // two empty pixel after each char
  }
}

void 
StationTexture::ComputeVertices( const Matrix4 * mat )
{
  // LOGI("StationTexture compute vertices ns %d", nStation);
  float * v = (float *)( modelGeometry->Vertex() );
  int k3 = 0;
  int off = 0;
  Vector4 vec;
  Vector4 out;
  for ( int k=0; k<nStation; ++k ) {
    k3 = 3 * mNames[k].idx;
    // LOGI("station %d idx %d name %s", k, mNames[k].idx, mNames[k].name );
    vec.m_x = v[k3+0];
    vec.m_y = v[k3+1];
    vec.m_z = v[k3+2];
    vec.m_w = 1.0f;
    mat->Multiply( vec, out );
    out.m_x /= out.m_w;
    out.m_y /= out.m_w;
    out.m_z /= out.m_w;
    // LOGI("StationTexture compute vertex %d: %.2f %.2f %.2f", k, out.m_x, out.m_y, out.m_z );
    vertex[ off + 0 ] = out.m_x;
    vertex[ off + 1 ] = out.m_y;
    vertex[ off + 2 ] = -1.0f; // out.m_z;
    off += 5;
    vertex[ off + 0 ] = out.m_x;
    vertex[ off + 1 ] = out.m_y+0.05;
    vertex[ off + 2 ] = -1.0f; // out.m_z;
    off += 5;
    vertex[ off + 0 ] = out.m_x+0.07;
    vertex[ off + 1 ] = out.m_y;
    vertex[ off + 2 ] = -1.0f; // out.m_z;
    off += 5;
    vertex[ off + 0 ] = out.m_x+0.07;
    vertex[ off + 1 ] = out.m_y+0.05;
    vertex[ off + 2 ] = -1.0f; // out.m_z;
    off += 5;
  }
  // geometry.SetVertex( (void *)vertex );
}

