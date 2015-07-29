#include "TherionShot.h"

#include <math.h>
#include "TherionSurvey.h"
#include "Vector3.h"

#define SIN(x) ((float)sin(x))
#define COS(x) ((float)cos(x))

#define DEG2RAD ((float)M_PI/180)

TherionShot::TherionShot( const char * f, const char * t, float l, float b, float c )
  : from( f )
  , to( t )
  , len( l )
  , ber( b * DEG2RAD )
  , cln( c * DEG2RAD )
  , from_station( NULL )
  , to_station( NULL )
  , survey( NULL )
  , surveyNr( -1 )
  , used( false )
{ }

TherionStation * 
TherionShot::GetStationFromStation( TherionStation * st ) const
{
  if ( st->Name() == from ) {
    float dz = len * SIN( cln );
    float dh = len * COS( cln );
    return new TherionStation( to,
                        st->E() + dh * SIN(ber),
                        st->N() + dh * COS(ber),
                        st->Z() + dz );
  } 
  if ( st->Name() == to ) {
    float dz = len * SIN( cln );
    float dh = len * COS( cln );
    return new TherionStation( from,
                        st->E() - dh * SIN(ber),
                        st->N() - dh * COS(ber),
                        st->Z() - dz );
  }
  return NULL;
}

Vector3 
TherionShot::ToVector3() 
{
  float h = len * COS(cln);
  return Vector3( h * SIN(ber), h * COS(ber), len * SIN(cln) );
}

void 
TherionShot::SetSurvey( TherionSurvey * s ) 
{ 
  survey = s;
  surveyNr = ( survey != NULL )? survey->Number() : -1; 
}
