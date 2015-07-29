#ifndef THERION_SURVEY_H
#define THERION_SURVEY_H

#include <string>

#include "TherionShot.h"

class TherionSurvey
{
  private:
    static int count;
    int number;

    std::string name;
    int mNrShots;
    int mNrSplays;
    float mLenShots;
    float mLenSplays;

  public:
    TherionSurvey( const char * n );

    void AddShotInfo( TherionShot * sh )
    {
      mNrShots ++;
      mLenShots += sh->Length();
    }

    void AddSplayInfo( TherionShot * sh )
    {
      mNrSplays ++;
      mLenSplays += sh->Length();
    }

    int Number() const { return number; }
    const std::string & Name() const { return name; }

    int NrShots() const { return mNrShots; }
    int NrSplays() const { return mNrSplays; }
    float LengthShots() const { return mLenShots; }
    float LengthSplays() const { return mLenSplays; }

};

#endif
