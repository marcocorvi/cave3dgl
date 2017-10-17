#include "FileModel.h"

#include <assert.h>

#include "Functions.h"
#include "Log.h"

#include "TherionFile.h"
// #include "TherionSurface.h"
// #include "TherionBitmap.h"
#include "LoxFile.h"
// #include "LoxSurface.h"
// #include "LoxBitmap.h"

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

#define DEM_STRIDE 6 /* 3 only positions, 6 position+normal */

// vertices are Rotated-Z:
// x = - Y_orig
// y = + X_orig
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLES/gl.h>

FileModel::~FileModel()
{
  // LOGI("File Model::dstr()" );
  if ( dem ) delete[] dem;
}


FileModel::FileModel( )
  : Model( "FileModel" )
  , dem( NULL )
  , dem_cols( 0 )
  , dem_rows( 0 )
{
}

void
FileModel::Init( const char * filename )
{
  if ( filename == NULL ) {
    LOGW("ERROR null init file" );
    return;
  }

  size_t len = strlen( filename );
  if ( len > 3 && strcmp( filename + len - 3, "lox" ) == 0 ) {
    InitFromLox( filename );
  } else if ( len > 2 && strcmp( filename + len - 2, "th" ) == 0 ) {
    InitFromTh( filename );
  } else if ( len > 8 && strcmp( filename + len - 8, "thconfig" ) == 0 ) {
    InitFromTh( filename );
  } else if ( len >= 8 && strncmp( filename, "thconfig", 8 ) == 0 ) {
    InitFromTh( filename );
  } else {
    LOGW("ERROR unsupported init file %s", filename );
  }
}

void
FileModel::InitFromTh( const char * filename )
{
  // LOGI("File Model::Init() %s", filename );
  TherionFile thf( filename );
  therion_ns = thf.GetStationNumber();
  therion_nl = thf.GetShotNumber();
  therion_nx = thf.GetSplayNumber();
  // LOGI("File Model::Init() N_stations %d N_splay %d N_legs %d", therion_ns, therion_nx, therion_nl );
  
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

  station_name = new Station[therion_ns];
  vertex = new float[ 3 * (therion_ns + therion_nx) ];
  lindex  = new unsigned short[ 2 * therion_nl ];
  sindex  = new unsigned short[ 2 * therion_nx ];
  float * svertex = vertex + (3 * therion_ns);        // N.B. offset by NR STATIONS

  std::vector< TherionStation * > & thst = thf.GetStations();
  std::vector< TherionShot * > & ths = thf.GetShots();
  std::vector< TherionShot * > & thx = thf.GetSplays();

  for ( int k=0; k<therion_ns; ++k ) {
    std::pair< std::map<std::string,int>::iterator, bool > ret =
      stations.insert( std::pair<std::string, int>( thst[k]->Name(), k ) );
    if ( ret.second ) { 
      std::string tmp = ret.first->first;
      size_t pos = tmp.find("@");
      if ( pos != std::string::npos ) {
        station_name[k].idx  = k;
        station_name[k].name = new char[pos+1];
        for ( size_t j=0; j<pos; ++j) station_name[k].name[j] = tmp[j];
        station_name[k].name[pos] = 0;
      }
    }
  }

  for ( int k=0; k<therion_ns; ++k ) {
    int kk = station_name[k].idx;
    vertex[3*kk+0] = SIGN_X( (thst[k]->Z() - x_offset) * scale );
    vertex[3*kk+1] = SIGN_Y( (thst[k]->E() - y_offset) * scale );
    vertex[3*kk+2] = SIGN_Z( (thst[k]->N() - z_offset) * scale ); // XXX + Z_OFFSET;
    LOGI("STATION %s %.2f %.2f %.2f", station_name[k].name, vertex[3*kk+0], vertex[3*kk+1], vertex[3*kk+2] );
  }
  for ( int k=0; k<therion_nx; ++k ) {
    // FIXME this assumes that splays have FROM station
    TherionStation * st = thx[k]->GetStationFromStation( thx[k]->FromStation() );
    svertex[3*k+0] = SIGN_X( ( st->Z() - x_offset ) * scale );
    svertex[3*k+1] = SIGN_Y( ( st->E() - y_offset ) * scale );
    svertex[3*k+2] = SIGN_Z( ( st->N() - z_offset ) * scale ); // XXX + Z_OFFSET;
    delete st;
  }
  for ( int k=0; k<therion_nl; ++k ) {
    int kf = 0;
    while ( kf < therion_ns && ths[k]->From() != thst[kf]->Name() ) ++kf;
    if ( kf >= therion_ns ) {
      LOGW("ERROR From station not found %s", ths[k]->From().c_str() );
      continue;
    }
    int kt = 0;
    while ( kt < therion_ns && ths[k]->To() != thst[kt]->Name() ) ++kt;
    if ( kt >= therion_ns ) {
      LOGW("ERROR To station not found %s", ths[k]->From().c_str() );
      continue;
    }
    lindex[2*k+0] = (unsigned short)kf;
    lindex[2*k+1] = (unsigned short)kt;
  }
  for ( int k=0; k<therion_nx; ++k ) {
    int kf = 0;
    while ( kf < therion_ns && thx[k]->From() != thst[kf]->Name() ) ++kf;
    if ( kf >= therion_ns ) {
      LOGW("ERROR From station not found %s", ths[k]->From().c_str() );
      continue;
    } 
    int kt = therion_ns + k; // offset by NR STATIONS
    sindex[2*k+0] = (unsigned short)kf;
    sindex[2*k+1] = (unsigned short)kt;
  }
  // LOGI("Station %d Shots %d Splays %d\n", therion_ns, therion_nl, therion_nx );

  // thf.DumpStations();

  SetVertex( vertex );                     //  vertex = float[ 3 * (therion_ns + therion_nx) ]
  SetNVertex( therion_ns + therion_nx );
  SetNStations( therion_ns );
  SetNLegs( therion_nl );
  SetLIndex( lindex );
  SetSIndex( sindex );
  SetNLIndex( 2 * therion_nl );  // number of indices to use to render
  SetNSIndex( 2 * therion_nx );  // number of indices to use to render
  SetName("therion-model");

  SetNPos(3);
  SetVertexStride( sizeof(float) * 3 );

  TherionSurface * s = thf.GetSurface();
  TherionBitmap * bitmap = thf.GetBitmap();
  // LOGI("Bitmap is %s", ( bitmap != NULL )? "not NULL" : "NULL" );

  if ( s != NULL ) {
    // LOGI("got therion surface");
    float de = s->DimEast();
    float dn = s->DimNorth();
    int n_cols = s->NrEast();
    dem_stride = DEM_STRIDE + 3; // +3 for the color
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
    dem = new float[ dem_stride * dem_cols * dem_rows ];
    float r=1.0, g=1.0, b=1.0;
    for ( int n =0; n < dem_rows; ++n ) {
      for ( int e = 0; e < dem_cols; ++e ) {
        int zoff = n * n_cols + e;
        int off = (n * dem_cols + e) * dem_stride; 
        #if DEM_STRIDE == 6
          float zval = ( z[zoff] + z[zoff+1] + z[zoff+n_cols] + z[zoff+n_cols+1] ) / 4;
          float normal_x = ( z[zoff] - z[zoff+1] + z[zoff+n_cols] - z[zoff+n_cols+1] ) / (2 * de);
          float normal_y = ( z[zoff] + z[zoff+1] - z[zoff+n_cols] - z[zoff+n_cols+1] ) / (2 * dn);
          float n2 = sqrt( normal_x * normal_x + normal_y * normal_y + 1);

          dem[ off + 0  ] = SIGN_X( (zval - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale );
          // FIXME why not to add Z_OFFSET ? because Z_OFFSET is  subtracted from vertexes in InitBBox
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); //  + Z_OFFSET;
          dem[ off + 3  ] = normal_x / n2;
          dem[ off + 4  ] = normal_y / n2;
          dem[ off + 5  ] = 1.0f / n2;
          if ( bitmap != NULL ) bitmap->GetRGB( e1 + e*de, n1 + n*dn, r, g, b );
          dem[ off + 6  ] = r;
          dem[ off + 7  ] = g;
          dem[ off + 8  ] = b;
        #else 
          dem[ off + 0  ] = SIGN_X( (z[ zoff ] - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale );
          // FIXME why not to add Z_OFFSET ? because Z_OFFSET is  subtracted from vertexes in InitBBox
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); //  + Z_OFFSET;
          if ( bitmap != NULL ) bitmap->GetRGB( e1 + e*de, n1 + n*dn, r, g, b );
          dem[ off + 3  ] = r;
          dem[ off + 4  ] = g;
          dem[ off + 5  ] = b;
        #endif
        // LOGI("DEM %d-%d %.2f %.2f %.2f", e,n, dem[off+0], dem[off+1], dem[off+2] );
      }
    }
    // LOGI("DEM %d x %d", dem_cols, dem_rows );
  }

  LOGI("scale %.6f", scale );
  InitBBox( true );
}


void
FileModel::InitFromLox( const char * filename )
{
  LoxFile lox;
  lox.ReadFile( filename );

  int ns  = lox.NrStations();
  int nlx = lox.NrShots(); // legs + splays
  vertex = new float[ 3 * ns ];
  // LOGI("Lox: stations %d shots %d ", ns, nlx );

  therion_ns = 0;
  therion_nx = 0;

  std::vector< LoxStation * > & sts = lox.GetStations();
  if ( sts.size() < 2 ) {
    LOGW("Init From Lox: too few stations");
    return;
  }

  float zmin, zmax, xmin, xmax, ymin, ymax;
  xmin = xmax = sts[0]->x; // east
  ymin = ymax = sts[0]->y; // north
  zmin = zmax = sts[0]->z; // vert
  for ( std::vector< LoxStation * >::iterator it = sts.begin(); it != sts.end(); ++it ) {
    LoxStation * st = *it;
    if ( st->NameStr() == "-" ) {
      therion_nx ++;
    } else {
      therion_ns ++;
    }
    float x = (float)st->x; // east
    float y = (float)st->y; // north
    float z = (float)st->z; // vert
    if ( x < xmin ) xmin = x; else if ( x > xmax ) xmax = x;
    if ( y < ymin ) ymin = y; else if ( y > ymax ) ymax = y;
    if ( z < zmin ) zmin = z; else if ( z > zmax ) zmax = z;
  }
  
  x_offset = (zmin + zmax)/2; // vert
  y_offset = (xmin + xmax)/2; // east
  z_offset = (ymin + ymax)/2; // north

  float x_scale = zmax - zmin;
  float y_scale = xmax - xmin;
  float z_scale = ymax - ymin;
  scale = x_scale;
  if ( y_scale > scale ) scale = y_scale;
  if ( z_scale > scale ) scale = z_scale;
  scale = 20.0f / scale;
  // LOGI("Offset (Center) V %.2f E %.2f N %.2f Scale %.2f", x_offset, y_offset, z_offset, scale );

  int idst[ ns ]; // stations id

  char st_name[128];
  int k = 0;
  for ( std::vector< LoxStation * >::iterator it = sts.begin(); it != sts.end(); ++it ) {
    LoxStation * st = *it;
    if ( st->NameStr() == "-" ) continue; // skip splay stations
    // assert( strlen( st->Name() ) > 0 );
    sprintf(st_name, "%s@%d", st->Name(), st->Survey() );
    stations.insert( std::pair<std::string, int>( st_name, k ) );
    vertex[3*k+0] = SIGN_X( (st->z - x_offset) * scale );
    vertex[3*k+1] = SIGN_Y( (st->x - y_offset) * scale );
    vertex[3*k+2] = SIGN_Z( (st->y - z_offset) * scale ); // XXX + Z_OFFSET;
    // LOGI("St %d %s: %.2f %.2f %.2f => %.2f %.2f %.2f",
    //   k, st_name, st->x, st->y, st->z, vertex[3*k+0], vertex[3*k+1], vertex[3*k+2] );
    idst[ k ] = (int)(st->Id());
    ++ k;
  }
  // LOGI("Named stations %d map size %d", therion_ns, stations.size() );
  // assert( therion_ns == k ); // number of "named" stations

  for ( std::vector< LoxStation * >::iterator it = sts.begin(); it != sts.end(); ++it ) {
    LoxStation * st = *it;
    if ( st->NameStr() != "-" ) continue; // only splay stations
    // stations.insert( std::pair<std::string, int>( st->Name(), k ) );
    vertex[3*k+0] = SIGN_X( (st->z - x_offset) * scale );
    vertex[3*k+1] = SIGN_Y( (st->x - y_offset) * scale );
    vertex[3*k+2] = SIGN_Z( (st->y - z_offset) * scale ); // XXX + Z_OFFSET;
    idst[ k ] = (int)(st->Id());
    ++ k;
  }

  // LOGI("Named stations %d map size %d", therion_ns, stations.size() );
  station_name = new Station[therion_ns];
  for ( std::map< std::string, int >::iterator it = stations.begin(); it != stations.end(); ++it ) {
    const std::string & tmp = it->first;
    k = it->second;
    size_t pos = tmp.find("@");
    if ( pos == std::string::npos ) pos = tmp.length();
    // assert( pos > 0 );
    station_name[k].idx = k;
    station_name[k].name = new char[pos+1];
    for ( size_t j=0; j<pos; ++j) station_name[k].name[j] = tmp[j];
    station_name[k].name[pos] = 0;
  }

  therion_nl = 0;
  std::vector< LoxShot * > & shs = lox.GetShots();
  lindex  = new unsigned short[ 2 * nlx ];
  k = 0;
  for ( std::vector< LoxShot * >::iterator it = shs.begin(); it != shs.end(); ++it ) {
    LoxShot * sh = *it;
    int fr = sh->From();
    int to = sh->To();
    int kfr = -1;
    int kto = -1;
    for ( int kk = 0; kk<ns; ++kk ) {
      if ( idst[kk] == fr ) {
        kfr = kk;
        if ( kto >= 0 ) break;
      } else if ( idst[kk] == to ) {
        kto = kk;
        if  ( kfr >= 0 ) break;
      }
    }
    if ( kfr < 0 || kto < 0 ) {
      LOGW("Shot without stations");
      continue;
    }
    if ( kfr < therion_ns && kto < therion_ns ) {
      lindex[2*k+0] = (unsigned short)kfr;
      lindex[2*k+1] = (unsigned short)kto;
      // LOGI("Leg %d: %d-%d", therion_nl, lindex[2*k+0], lindex[2*k+1] );
      ++ therion_nl;
      ++ k;
    }
  }
  
  sindex  = new unsigned short[ 2 * nlx ];
  int nx = 0;
  for ( std::vector< LoxShot * >::iterator it = shs.begin(); it != shs.end(); ++it ) {
    LoxShot * sh = *it;
    int fr = sh->From();
    int to = sh->To();
    int kfr = -1;
    int kto = -1;
    for ( int kk = 0; kk<ns; ++kk ) {
      if ( idst[kk] == fr ) {
        kfr = kk;
        if ( kto >= 0 ) break;
      } else if ( idst[kk] == to ) {
        kto = kk;
        if  ( kfr >= 0 ) break;
      }
    }
    if ( kfr < 0 || kto < 0 ) {
      // LOGW("Shot without stations");
      continue;
    }
    if ( kfr >= therion_ns || kto >= therion_ns ) {
      sindex[2*k+0] = (unsigned short)kfr;
      sindex[2*k+1] = (unsigned short)kto;
      ++ nx;
      ++ k;
    }
  }
  therion_nx = nx;
  // assert( nx == therion_nx );
  // assert( k == nlx );
  // assert( nlx == therion_nl + therion_nx );

  // LOGI("Station %d Shots %d Splays %d\n", therion_ns, therion_nl, therion_nx );

  // lox.DumpStations();

  SetVertex( vertex );
  SetNVertex( therion_ns + therion_nx );
  SetNStations( therion_ns );
  SetNLegs( therion_nl );
  SetLIndex( lindex );
  SetNLIndex( 2 * therion_nl );  // number of indices to use to render
  SetSIndex( sindex );
  SetNSIndex( 2 * therion_nx );  // number of indices to use to render
  SetName("therion-model");

  SetNPos(3);
  SetVertexStride( sizeof(float) * 3 );

  LoxSurface * s = lox.GetSurface();
  LoxBitmap * bitmap = lox.GetBitmap();
  // LOGI("Bitmap is %s", ( bitmap != NULL )? "not NULL" : "NULL" );

  if ( s != NULL ) {
    // LOGI("got lox surface");
    float de = s->DimEast();
    float dn = s->DimNorth();
    int n_cols = s->NrEast();
    dem_stride = DEM_STRIDE + 3; // +3 for the color
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
    // LOGI("Surface %.2f %.2f (delta %.2f %.2f dim %d %d)", e1, n1, de, dn, dem_cols, dem_rows );
    const double * z0 = s->Grid();
    #if DEM_STRIDE == 6
      const double * z1 = z0 + 1;
      const double * z2 = z0 + n_cols;
      const double * z3 = z0 + n_cols + 1;
    #endif
    dem = new float[ dem_stride * dem_cols * dem_rows ];
    float r=1.0, g=1.0, b=1.0;
    for ( int n =0; n < dem_rows; ++n ) {
      for ( int e = 0; e < dem_cols; ++e ) {
        int zoff = n * n_cols + e;
        int off = (n * dem_cols + e) * dem_stride; 
        #if DEM_STRIDE == 6
          float zval     = (float)( z0[zoff] + z1[zoff] + z2[zoff] + z3[zoff] ) / 4;
          float normal_x = (float)( z0[zoff] - z1[zoff] + z2[zoff] - z3[zoff] ) / (2 * de);
          float normal_y = (float)( z0[zoff] + z1[zoff] - z2[zoff] - z3[zoff] ) / (2 * dn);
          float n2 = sqrt( normal_x * normal_x + normal_y * normal_y + 1);

          dem[ off + 0  ] = SIGN_X( (zval - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale ); 
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); // + Z_OFFSET;
          // LOGI("DEM %.2f %.2f %.2f => %.2f %.2f %.2f", e1 + e * de, n1 + n * dn, zval,
          //   dem[ off + 0  ], dem[ off + 1  ], dem[ off + 2  ] );
          // FIXME why not to add Z_OFFSET ? because Z_OFFSET is subtracted by InitBBox 

          dem[ off + 3  ] = normal_x / n2;
          dem[ off + 4  ] = normal_y / n2;
          dem[ off + 5  ] = 1.0f / n2;

          if ( bitmap != NULL ) bitmap->GetRGB( e1 + e*de, n1 + n*dn, r, g, b );
          dem[ off + 6  ] = r;
          dem[ off + 7  ] = g;
          dem[ off + 8  ] = b;
        #else 
          dem[ off + 0  ] = SIGN_X( (float)(z0[ zoff ] - x_offset) * scale );
          dem[ off + 1  ] = SIGN_Y( (e1 + e * de - y_offset) * scale );
          dem[ off + 2  ] = SIGN_Z( (n1 + n * dn - z_offset) * scale ); // + Z_OFFSET;
          // FIXME why not to add Z_OFFSET ? because it is subtracted from vertices by InitBBox

          if ( bitmap != NULL ) bitmap->GetRGB( e1 + e*de, n1 + n*dn, r, g, b );
          dem[ off + 3  ] = r;
          dem[ off + 4  ] = g;
          dem[ off + 5  ] = b;
          // LOGI("DEM %.2f %.2f %.2f => %.2f %.2f %.2f", e1 + e * de, n1 + n * dn, z0[zoff],
          //   dem[ off + 0  ], dem[ off + 1  ], dem[ off + 2  ] );
        #endif
        // LOGI("DEM %d-%d %.2f %.2f %.2f", e,n, dem[off+0], dem[off+1], dem[off+2] );
      }
    }
    // LOGI("DEM %d x %d", dem_cols, dem_rows );
  }

  InitBBox( true );
}



