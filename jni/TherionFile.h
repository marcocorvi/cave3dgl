#ifndef THERION_FILE_H
#define THERION_FILE_H

#include <string>
#include <vector>

#include "TherionFix.h"
#include "TherionStation.h"
#include "TherionShot.h"
#include "TherionSurvey.h"
#include "TherionSurface.h"
#include "TherionBitmap.h"

class TherionFile
{
  private:
    bool can_render;

    std::vector< TherionSurvey * > surveys;
    std::vector< TherionFix * > fixes;
    std::vector< TherionStation * > stations;
    std::vector< TherionShot * > shots;   // centerline shots
    std::vector< TherionShot * > splays;  // splay shots

    float emin, emax, nmin, nmax, zmin, zmax;
    float mCaveLength;
    TherionSurface * mSurface;
    TherionBitmap * mBitmap;

  public:
    float Emin() const { return emin; }
    float Emax() const { return emax; }
    float Nmin() const { return nmin; }
    float Nmax() const { return nmax; }
    float Zmin() const { return zmin; }
    float Zmax() const { return zmax; }

    int GetStationNumber() { return stations.size(); }
    int GetShotNumber()    { return shots.size(); }
    int GetSplayNumber()   { return splays.size(); }
    int GetSurveySize()    { return surveys.size(); }

    std::vector< TherionSurvey * > & GetSurveys() { return surveys; }
    std::vector< TherionShot * > & GetShots() { return shots; }
    std::vector< TherionShot * > & GetSplays() { return splays; }
    std::vector< TherionStation * > & GetStations() { return stations; }
    std::vector< TherionFix * > & GetFixes() { return fixes; }

    bool CanRender() const { return can_render; }

    TherionSurface * GetSurface() { return mSurface; }

    TherionBitmap * GetBitmap() { return mBitmap; }

    TherionSurvey * GetSurvey( const char * name );

    TherionSurvey * GetSurvey( const std::string & name ) { return GetSurvey( name.c_str() ); }

    // void DumpStations() const;
    // void DumpShots() const;

    float GetCaveDepth() { return zmax - zmin; }
    float GetCaveLength() { return mCaveLength; }

    /** fill an array of float with the values of the stations coordinates
     *  and set the "vertex" index of the stations
     */
    void GetStationVertices( float v[] );
 
    void SetShotSurveys();

    void SetSplaySurveys();

    void GetLegAt( TherionStation * station, std::vector< TherionShot * > & legs );

    void GetSplayAt( TherionStation * station, std::vector< TherionShot * > & ret, bool also_legs );

    void SetStationDepths( );

    ~TherionFile();

    TherionFile( const std::string & filename );

  private:
    void ProcessShots();

    /** read input file
     * @param usd
     * @param sd
     * @param ul units of length (as multiple of 1 meter)
     * @param ub units of bearing (as multiple of 1 degree)
     * @param uc units of clino
     */
    bool ReadFile( const std::string & filename,
                   const std::string & basepath,
                   bool usd, 
                   float sd, float ul, float ub, float uc );

};

#endif
