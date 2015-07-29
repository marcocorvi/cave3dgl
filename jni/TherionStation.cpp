#include "TherionStation.h"


TherionStation::TherionStation( std::string nm, float e0, float n0, float z0 )
  : Vector3(e0, n0, z0)
  , vertex( -1 )
  , name( nm )
  , depth( 0.0f )
{
  MakeShortName();
}

TherionStation::TherionStation( const char * nm, float e0, float n0, float z0 )
  : Vector3(e0, n0, z0)
  , vertex( -1 )
  , name( nm )
  , depth( 0.0f )
{
  MakeShortName();
}

void 
TherionStation::MakeShortName()
{
  size_t index = name.find("@");
  if ( index != std::string::npos ) {
    short_name = name.substr( 0, index );
  } else {
    short_name = name;
  }
}

