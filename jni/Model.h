#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include "Geometry.h"

class Model : public Geometry
{
  public:
    typedef std::map< std::string, int >           StationMap;
    typedef std::map< std::string, int >::iterator StationMapIterator;

  private:
    float * my_vertex; // float: station coordinates
    unsigned short * my_index;  // unsigned short: indices of lines

    // void * index2;
    // int nIndex2;
    
    StationMap stations;

  public:
    Model( const char * filename );

    ~Model();

    void Init();

}; 

#endif
