#include "TherionModel.h"

#include "Log.h"
/*
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

const char ** 
TherionModel::GetStations()
{
  return (const char **) station_name;
}

int 
TherionModel::GetNStations()
{
  return therion_ns;
}

TherionModel::TherionModel( )
  : Geometry( GL_LINES, "TherionModel", FLAG_LINE )
  , station_name( NULL )
  , vertex( NULL )
  , dem( NULL )
  , dem_cols( 0 )
  , dem_rows( 0 )
  , dem_color( NULL )
  , index( NULL )
  , therion_ns( 0 )
  , therion_nl( 0 )
  , therion_nx( 0 )
  , x_offset( 0.0f )
  , y_offset( 0.0f )
  , z_offset( 0.0f )
  , scale( 1.0f )
{ }

void
TherionModel::Init( const char * filename )
{
  // LOGI("TherionModel::Init() %s", filename );
  TherionFile thf( filename );
  therion_ns = thf.GetStationNumber();
  therion_nl = thf.GetShotNumber();
  therion_nx = thf.GetSplayNumber();
  // LOGI("TherionModel::Init() N_stations %d N_splay %d N_legs %d", therion_ns, therion_nx, therion_nl );
  
  x_offset = (thf.Zmin() + thf.Zmax())/2;
  y_offset = (thf.Emin() + thf.Emax())/2;
  z_offset = (thf.Nmin() + thf.Nmax())/2;
  float x_scale = thf.Zmax() - thf.Zmin();
  float y_scale = thf.Emax() - thf.Emin();
  float z_scale = thf.Nmax() - thf.Nmin();
  scale = x_scale;
  if ( y_scale > scale ) scale = y_scale;
  if ( z_scale > scale ) scale = z_scale;
  scale = 20.0f / scale;

  station_name = new char * [therion_ns];
  vertex = new float[ 3 * (therion_ns + therion_nx) ];
  index  = new unsigned short[ 2 * (therion_nl + therion_nx) ];

  std::vector< TherionStation * > & thst = thf.GetStations();
  std::vector< TherionShot * > & ths = thf.GetShots();
  std::vector< TherionShot * > & thx = thf.GetSplays();

  for ( int k=0; k<therion_ns; ++k ) {
    station_name[k] = NULL;
    std::pair< std::map<std::string,int>::iterator, bool > ret =
      stations.insert( std::pair<std::string, int>( thst[k]->Name(), k ) );
    if ( ret.second ) { 
      std::string tmp = ret.first->first;
      size_t pos = tmp.find("@");
      if ( pos != std::string::npos ) {
        station_name[k] = new char[pos+1];
        for ( size_t j=0; j<pos; ++j) station_name[k][j] = tmp[j];
        station_name[k][pos] = 0;
      }
    }
  }

  for ( int k=0; k<therion_ns; ++k ) {
    vertex[3*k+0] = SIGN_X( (thst[k]->Z() - x_offset) * scale );
    vertex[3*k+1] = SIGN_Y( (thst[k]->E() - y_offset) * scale );
    vertex[3*k+2] = SIGN_Z( (thst[k]->N() - z_offset) * scale ) + 100.0f;
    // LOGI("STATION %s %.2f %.2f %.2f", station_name[k], vertex[3*k+0], vertex[3*k+1], vertex[3*k+2] );
  }
  for ( int k=0; k<therion_nx; ++k ) {
    // FIXME this assumes that splays have FROM station
    TherionStation * st = thx[k]->GetStationFromStation( thx[k]->FromStation() );
    vertex[3*(therion_ns+k)+0] = SIGN_X( ( st->Z() - x_offset ) * scale );
    vertex[3*(therion_ns+k)+1] = SIGN_Y( ( st->E() - y_offset ) * scale );
    vertex[3*(therion_ns+k)+2] = SIGN_Z( ( st->N() - z_offset ) * scale ) + 100.0f;
    delete st;
  }
  for ( int k=0; k<therion_nl; ++k ) {
    int kf = 0;
    while ( kf < therion_ns && ths[k]->From() != thst[kf]->Name() ) ++kf;
    if ( kf >= therion_ns ) {
      LOGI("ERROR From station not found %s", ths[k]->From().c_str() );
      continue;
    }
    int kt = 0;
    while ( kt < therion_ns && ths[k]->To() != thst[kt]->Name() ) ++kt;
    if ( kt >= therion_ns ) {
      LOGI("ERROR To station not found %s", ths[k]->From().c_str() );
      continue;
    }
    index[2*k+0] = (unsigned short)kf;
    index[2*k+1] = (unsigned short)kt;
  }
  for ( int k=0; k<therion_nx; ++k ) {
    int kf = 0;
    while ( kf < therion_ns && thx[k]->From() != thst[kf]->Name() ) ++kf;
    if ( kf >= therion_ns ) {
      LOGI("ERROR From station not found %s", ths[k]->From().c_str() );
      continue;
    } 
    int kt = therion_ns + k;
    index[2*(therion_nl+k)+0] = (unsigned short)kf;
    index[2*(therion_nl+k)+1] = (unsigned short)kt;
  }
  // LOGI("Station %d Shots %d Splays %d\n", therion_ns, therion_nl, therion_nx );

  // thf.DumpStations();

  SetVertex( vertex );
  SetNVertex( therion_ns + therion_nx );
  SetNStations( therion_ns );
  SetNLegs( therion_nl );
  SetIndex( index );
  SetNIndex( 2 * (therion_nl+therion_nx) );  // number of indices to use to render
  SetName("therion-model");

  SetNPos(3);
  
#define DEM_STRIDE 6
  SetVertexStride( sizeof(float) * 3 );

  TherionSurface * s = thf.GetSurface();
  if ( s != NULL ) {
    // LOGI("got therion surface");
    float de = s->DimEast();
    float dn = s->DimNorth();
    int n_cols = s->NrEast();
    dem_stride = DEM_STRIDE;
    #if DEM_STRIDE == 6
      dem_cols = s->NrEast()  - 1;
      dem_rows = s->NrNorth() - 1;
      float e1 = s->East1()  + de / 2;
      float n1 = s->North1() + dn / 2;
    #else
      dem_cols = s->NrEast();
      dem_rows = s->NrNorth();
      float e1 = s->East1();
      float n1 = s->North1();
    #endif
    const float * z = s->GetZ();
    dem = new float[ DEM_STRIDE * dem_cols * dem_rows ];
    dem_color = new unsigned char[ 3 * dem_cols * dem_rows ];
    for ( int n =0; n < dem_rows; ++n ) {
      for ( int e = 0; e < dem_cols; ++e ) {
        int zoff = n * n_cols + e;
        int off = (n * dem_cols + e) * DEM_STRIDE; 
        #if DEM_STRIDE == 6
          float zval = ( z[zoff] + z[zoff+1] + z[zoff+n_cols] + z[zoff+n_cols+1] ) / 4;
          float normal_x = ( z[zoff] - z[zoff+1] + z[zoff+n_cols] - z[zoff+n_cols+1] ) / (2 * de);
          float normal_y = ( z[zoff] + z[zoff+1] - z[zoff+n_cols] - z[zoff+n_cols+1] ) / (2 * dn);
          float n2 = sqrt( normal_x * normal_x + normal_y * normal_y + 1);

          dem[ off + 0  ] = SIGN_X( (zval - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale );
          // FIXME why not to add 100.0f ????
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); //  + 100.0f;
          dem[ off + 3  ] = normal_x / n2;
          dem[ off + 4  ] = normal_y / n2;
          dem[ off + 5  ] = 1.0f / n2;
        #else 
          dem[ off + 0  ] = SIGN_X( (z[ zoff ] - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale );
          // FIXME why not to add 100.0f ????
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); //  + 100.0f;
        #endif
        // LOGI("DEM %d-%d %.2f %.2f %.2f", e,n, dem[off+0], dem[off+1], dem[off+2] );
      }
    }
    // TODO if TherionFile has a bitmap
    for ( int n =0; n < dem_rows; ++n ) {
      for ( int e = 0; e < dem_cols; ++e ) {
        int zoff = n * n_cols + e;
        int off = (n * dem_cols + e) * 3;
        dem_color[ off + 0 ] = 128;
        dem_color[ off + 1 ] = 128;
        dem_color[ off + 2 ] = 255;
      }
    }
    // LOGI("DEM %d x %d", dem_cols, dem_rows );
  }

  InitBBox();
}

TherionModel::~TherionModel()
{
  LOGI("TherionModel::dstr()" );
  if ( vertex ) delete[] vertex;
  if ( index ) delete[] index;
  if ( station_name ) {
    for ( int k = 0; k < therion_ns; ++k ) if ( station_name[k] ) delete[] station_name[k];
    delete[] station_name;
  }
  if ( dem ) delete[] dem;
  if ( dem_color ) delete[] dem_color;
}



