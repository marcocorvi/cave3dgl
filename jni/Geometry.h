#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdlib.h>
#include <string>
// #include <GLES/gl.h>
#include "Vector3.h"
#include "DrawFlags.h"
#include "Log.h"

class Geometry
{
  protected:
    int  type; // geometry GL type
    std::string name;
    unsigned int flag; // drawing flag : geom objects type
    int  nVertex;
    int  nIndex;
    void * vertex;
    void * index;
 
    int  nPos;
    int  nCol;
    int  nTex;
    int  nStations;
    int  nLegs;

    int  vertexStride;

    float xmin, xmax, ymin, ymax, zmin, zmax;
    float xc, yc, zc; // midpoint

  public:
    float XMin() const { return xmin; }
    float XMax() const { return xmax; }
    float YMin() const { return ymin; }
    float YMax() const { return ymax; }
    float ZMin() const { return zmin; }
    float ZMax() const { return zmax; }

    float XCenter() const { return xc; }
    float YCenter() const { return yc; }
    float ZCenter() const { return zc; }

  public:
    Geometry( int t /* = GL_TRIANGLES */, const char * n, unsigned int f )
      : type( t )
      , name( n )
      , flag( f )
      , nVertex( 0 )
      , nIndex( 0 )
      , vertex( NULL )
      , index( NULL )
      , nPos( 0 )
      , nCol( 0 )
      , nTex( 0 )
      , nStations( 0 )
      , nLegs( 0 )
      , vertexStride( 0 )
    { }
    virtual ~Geometry() 
    {
      // LOGI("Geometry dstr %s", name.c_str() );
    }

    bool TestFlag( unsigned int f ) const { return (flag & f) == flag; }

    unsigned int GetFlag() const { return flag; }

    void SetType( int t ) { type = t; }
    int GetType() const { return type; }

    void SetName( const char * n ) { name = n; }
    const char * Name() const { return name.c_str(); }

    void SetNVertex( int n ) { nVertex = n; }
    void SetNIndex( int n )  { nIndex = n; }
    int  NVertex() const { return nVertex; }
    int  NIndex() const { return nIndex; }

    void SetVertex( void * buf ) { vertex = buf; }
    void SetIndex( void * buf )  { index = buf; }
    void * Vertex() { return vertex; }
    void * Index()  { return index; }

    void SetNPos( int n ) { nPos = n; }
    void SetNCol( int n ) { nCol = n; }
    void SetNTex( int n ) { nTex = n; }
    void SetNStations( int n ) { nStations = n; }
    void SetNLegs( int n ) { nLegs = n; }
    int  NPos() const { return nPos; }
    int  NCol() const { return nCol; }
    int  NTex() const { return nTex; }
    int  NStations() const { return nStations; }
    int  NLegs() const { return nLegs; }

    void SetVertexStride( int n ) { vertexStride = n; }
    int  VertexStride() const { return vertexStride; }

    Vector3 GetMin() const { return Vector3( xmin, ymin, zmin ); }
    Vector3 GetMax() const { return Vector3( xmax, ymax, zmax ); }

    Vector3 GetCenter() const { return Vector3( xc, yc, zc ); }

  protected:
    void InitBBox();
};
#endif
