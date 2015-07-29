#ifndef STATION_TEXTURE_H
#define STATION_TEXTURE_H


#include "Texture.h"
#include "Geometry.h"
#include "Matrix4.h"

class StationTexture : public Texture
{
  private:
    unsigned int length;
    unsigned int rows;  // number of rows
    unsigned int name_width;
    unsigned int name_stride;
    unsigned int table_stride;

    unsigned char * texData;   // [owned]
    unsigned int nStation;
    Geometry * modelGeometry;  // model geometry [not owned]
    float * vertex;            // [owned]
    unsigned short * index;    // [owned]
    Geometry geometry;         // my geometry

  public:
    // ns   number of stations
    // len  max nr chars in station names
    StationTexture( int ns, Geometry * geom, const char ** names, unsigned int len = 2 ); 

    ~StationTexture();

    Geometry * GetMyGeometry() { return & geometry; }

    void ComputeVertices( const Matrix4 * mat );

  private:
    // k    index
    // str  name
    void AddStation( int k, const char * str );

};

#endif
