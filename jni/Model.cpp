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
#include <GLES/gl.h>

#include "Log.h"

Model::Model( const char * name )
  : Geometry( GL_LINES, name, FLAG_LINE )
  , station_name( NULL )
  , vertex( NULL )
  , lindex( NULL )
  , sindex( NULL )
  , therion_ns( 0 )
  , therion_nl( 0 )
  , therion_nx( 0 )
  , x_offset( 0 )
  , y_offset( 0 )
  , z_offset( 0 )
  , scale( 1 )
{
  // LOGI("Model::cstr() file %s", filename );
}

Model::~Model()
{
  if ( station_name ) {
    for ( int k = 0; k < therion_ns; ++k ) if ( station_name[k] ) delete[] station_name[k];
    delete[] station_name;
  }
}
