#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include "Geometry.h"
#include "Station.h"

class Model : public Geometry
{
  public:
    typedef std::map< std::string, int >           StationMap;
    typedef std::map< std::string, int >::iterator StationMapIterator;

  protected:
    Station * station_name;
    float * vertex;         // stations positions
    unsigned short * lindex; // leg index
    unsigned short * sindex; // splay index

  public:
    int therion_ns; // nr. of stations (0)
    int therion_nl; // nr. of legs (0)
    int therion_nx; // nr. of splays = number of indices

    float x_offset; // file-model offset ( center of the bbox )
    float y_offset;
    float z_offset;
    float scale;    // model scale = 20 / max_side
    
  protected:
    StationMap stations;

  public:
    Model( const char * name );

    ~Model();

    void addPoint( double e, double n, double z );

    int GetNStations() const { return therion_ns; }

    const Station * GetStations() const { return (const Station *) station_name; }
}; 

#endif
