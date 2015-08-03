#ifndef LOX_STATION_H
#define LOX_STATION_H

#include <stdint.h>

#include <string>

class LoxStation
{
  private:
    uint32_t id;
    uint32_t sid; // survey
    std::string name;
    std::string comment;
    uint32_t flag;

  public:
    double x, y, z;

  public:
    LoxStation( uint32_t _id, uint32_t _sid, const char * n, const char * c, uint32_t f,
                double _x, double _y, double _z )
      : id( _id )
      , sid( _sid )
      , name( n )
      , comment( c )
      , flag( f )
      , x( _x )
      , y( _y )
      , z( _z )
    { }

    uint32_t Id() const { return id; }
    uint32_t Survey() const { return sid; }
    uint32_t Flag() const { return flag; }

    const std::string & NameStr() const { return name; }
    const char * Name() const { return name.c_str(); }
    const char * Comment() const { return comment.c_str(); }

};
#endif
