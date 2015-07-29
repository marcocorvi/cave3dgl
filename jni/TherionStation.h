#ifndef THERION_STATION_H
#define THERION_STATION_H

#include <string>
#include "Vector3.h"

class TherionStation : public Vector3
{
  private:
    int vertex;     // index of vertex (coords) in the array of vertices 
                    // to get the coords use 3*vertex+0, 3*vertex+1, 3*vertex+2
    std::string short_name;
    std::string name;
    // float e, n, z; // north east, vertical (upwards)
    float depth;   // depth from Zmax: positive and scaled in [0,1]
                    // 1.0 deepest

  public:
    TherionStation( std::string nm, float e0, float n0, float z0 );

    TherionStation( const char * nm, float e0, float n0, float z0 );

    void AddToName( int loop_cnt ) { name += "-" + loop_cnt; }

    const std::string & Name() const { return name; }
    const std::string & ShortName() const { return short_name; }

    float Depth() const { return depth; }
    void SetDepth( float d ) { depth = d; }

    float E() const { return m_x; }
    float N() const { return m_y; }
    float Z() const { return m_z; }

    int Vertex() const { return vertex; }
    void SetVertex( int v ) { vertex = v; }

  private:
    void MakeShortName();

};

#endif
