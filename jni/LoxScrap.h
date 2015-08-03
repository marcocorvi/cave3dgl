#ifndef LOX_SCRAP_H
#define LOX_SCRAP_H

#include <stdint.h>

class LoxScrap
{
  private:
    uint32_t id;
    uint32_t sid;
    uint32_t nPts;
    uint32_t nIdx;
    double * pts;
    uint32_t * idx;

  public:
    LoxScrap( uint32_t _id, uint32_t _sid, uint32_t np, uint32_t ni, double * p, uint32_t * i )
      : id( _id )
      , sid( _sid )
      , nPts( np )
      , nIdx( ni )
      , pts( p )
      , idx( i )
    { }

    uint32_t Id() const     { return id; }
    uint32_t Survey() const { return sid; }
    uint32_t NrPoint() const { return nPts; }
    uint32_t NrIndex() const { return nIdx; }

    double * Point()   { return pts; }
    uint32_t * Index() { return idx; }
    double * Point( int k ) { return pts+3*k; }
    uint32_t * Index( int k ) { return idx+3*k; }

};

#endif
