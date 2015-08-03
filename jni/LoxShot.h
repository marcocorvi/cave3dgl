#ifndef LOX_SHOT_H
#define LOX_SHOT_H

#include <stdint.h>

class LoxShot
{
  private:
    uint32_t from;
    uint32_t to;
    uint32_t sid;
    uint32_t flag;
    uint32_t type;
    double fLRUD[4];
    double tLRUD[4];
    double vthr;

  public:
    LoxShot( uint32_t f, uint32_t t, uint32_t _sid, uint32_t fl, uint32_t tp, double thr,
             double f0, double f1, double f2, double f3,
             double t0, double t1, double t2, double t3 )
      : from( f )
      , to( t )
      , sid( _sid )
      , flag( fl )
      , type( tp )
      , vthr( thr )
    {
      fLRUD[0] = f0;
      fLRUD[1] = f1;
      fLRUD[2] = f2;
      fLRUD[3] = f3;
      tLRUD[0] = t0;
      tLRUD[1] = t1;
      tLRUD[2] = t2;
      tLRUD[3] = t3;
    }

    uint32_t From() const { return from; }
    uint32_t To() const   { return to; }
    uint32_t Survey() const { return sid; }
    uint32_t Flag() const { return flag; }
    uint32_t Type() const { return type; }

    double FLeft() const  { return fLRUD[0]; }
    double FRight() const { return fLRUD[0]; }
    double FUp() const    { return fLRUD[0]; }
    double FDown() const  { return fLRUD[0]; }

    double TLeft() const  { return tLRUD[0]; }
    double TRight() const { return tLRUD[0]; }
    double TUp() const    { return tLRUD[0]; }
    double TDown() const  { return tLRUD[0]; }

    double VThreshold() const { return vthr; }
};

#endif
