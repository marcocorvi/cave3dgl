#ifndef THERION_SHOT_H
#define THERION_SHOT_H

#include <stdlib.h>
#include <string>

#include "TherionStation.h"
#include "Vector3.h"

class TherionSurvey;

class TherionShot
{
  private:
    std::string from;
    std::string to;
    TherionStation * from_station;
    TherionStation * to_station;  // null for splay shots
    float len, ber, cln;      // radians
    TherionSurvey * survey;
    int surveyNr;
    bool used;

  public:
    TherionShot( const char * f, const char * t, float l, float b, float c );

    TherionStation * GetStationFromStation( TherionStation * st ) const;

    float Depth()
    {
      if ( to_station == NULL ) return 0.0;
      return (from_station->Depth() + to_station->Depth())/2;
    }

    Vector3 ToVector3();

    const std::string & From() const { return from; }
    const std::string & To() const { return to; }
    TherionStation * FromStation() { return from_station; }
    TherionStation * ToStation() { return to_station; }
    void SetFromStation( TherionStation * st ) { from_station = st; }
    void SetToStation( TherionStation * st ) { to_station = st; }
    // float len, ber, cln;      // radians

    float Length() const { return len; }
    float Bering() const { return ber; }
    float Clino()  const { return cln; }

    TherionSurvey * Survey() { return survey; }
    void SetSurvey( TherionSurvey * s );

    int SurveyNr() const { return surveyNr; }

    bool IsUsed() const { return used; }
    void SetUsed( bool u ) { used = u; }

};

#endif
