#ifndef THERION_FIX_H
#define THERION_FIX_H

#include <string>
#include "Vector3.h"

class TherionFix : public Vector3
{
  private:
    /** fix station:
     * fix stations are supposed to be referred to the same coord system
     */
    // private CS cs;
    std::string name;
    // double e, n, z; // north east, vertical (upwards)

  public:
    TherionFix( std::string nm, float e0, float n0, float z0 )
      : Vector3( e0, n0, z0 )
      , name( nm )
    { }

    TherionFix( const char * nm, float e0, float n0, float z0 )
      : Vector3( e0, n0, z0 )
      , name( nm )
    { }

    const std::string & Name() const { return name; }

    float E() const { return m_x; }
    float N() const { return m_y; }
    float Z() const { return m_z; }

};


#endif
