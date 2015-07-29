#include "Model.h"

/** model file format
 *
 * nr_stations nr_legs nr_splays
 * station_0 x0 y0 z0
 * station_1 x1 y1 z1
 * ...
 * leg0_from leg0_to
 * leg1_from leg1_to
 * ...
 * splay0_from x y z
 * ...
 */

#include <stdlib.h>
#include <stdio.h>
#include <GLES/gl.h>
#include "Log.h"

Model::Model( const char * filename )
  : Geometry( GL_LINES, filename, FLAG_LINE )
{
  LOGI("Model::cstr() file %s", filename );
  FILE * fp = fopen( filename, "r" );
  if ( fp != NULL ) {
    int ns;
    int nl;
    int nx;
    fscanf(fp, "%d %d %d", &ns, &nl, &nx );  // Nr.Stations Nr.Legs Nr.splays
    my_vertex = new float[ 3 * ( ns + nx ) ];
    my_index  = new unsigned short[ 2 * (nl + nx) ];

    char name[32];
    char name2[32];
    for ( int k=0; k<ns; ++k ) {
      float x, y, z;
      if ( fscanf( fp, "%s %f %f %f", name, &x, &y, &z ) != 4 ) break;
      my_vertex[ 3*k + 0 ] = x;
      my_vertex[ 3*k + 1 ] = y;
      my_vertex[ 3*k + 2 ] = z;
      stations.insert( std::pair<std::string, int>( name, k ) );
    }
    for ( int k=0; k<nl; ++k ) {
      int a, b;
      fscanf(fp, "%s %s", name, name2 );
      StationMapIterator it1 = stations.find( name );
      StationMapIterator it2 = stations.find( name2 );
      if ( it1 != stations.end() && it2 != stations.end() ) {
        my_index[ 2*k + 0 ] = it1->second;
        my_index[ 2*k + 1 ] = it2->second;
      } else {
        break;
      }
    }
    int cx = 0;
    for ( int k=0; k<nx; ++k ) {
      int a, b;
      float x, y, z;
      fscanf(fp, "%s %f %f %f", name, &x, &y, &z );
      StationMapIterator it1 = stations.find( name );
      if ( it1 == stations.end() ) break;
      sprintf(name2, "%s-%d", name, cx );
      ++ cx;
      my_vertex[ 3*ns + 3*k + 0 ] = x;
      my_vertex[ 3*ns + 3*k + 1 ] = y;
      my_vertex[ 3*ns + 3*k + 2 ] = z;
      // station name not inserted in the stations map for the TO of the splay 
      my_index[ 2 * nl + 2*k + 0 ] = it1->second;
      my_index[ 2 * nl + 2*k + 1 ] = ns + k;
    }  
    SetVertex( my_vertex );
    SetNVertex( ns + nx );
    SetIndex( my_index );
    SetNIndex( 2 * (nl+nx) );
    
    InitBBox();
  }
}

Model::~Model()
{
  LOGI("Model::dstr()" );
  if ( my_vertex ) delete my_vertex;
  if ( my_index )  delete my_index;
}

void
Model::Init()
{
  LOGI("Model::Init()" );
}
