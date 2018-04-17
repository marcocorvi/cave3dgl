#ifndef STATION_H
#define STATION_H

#include <stdlib.h>

struct Station
{
  char * name;  // station name
  int  idx;     // vertex index

  Station()
    : name( NULL )
    , idx( -1 )
  { }

};



#endif
