#include "TherionSurvey.h"

int 
TherionSurvey::count = 0;


TherionSurvey::TherionSurvey( const char * n )
  : number( count++ )
  , name( n )
  , mNrShots( 0 )
  , mNrSplays( 0 )
  , mLenShots( 0.0f )
  , mLenSplays( 0.0f )
{ }
