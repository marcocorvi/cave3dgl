#ifndef THERION_MODEL_H
#define THERION_MODEL_H

#include <string>
#include <map>
#include "Geometry.h"

class TherionModel : public Geometry
{
  public:
    typedef std::map< std::string, int >           StationMap;
    typedef std::map< std::string, int >::iterator StationMapIterator;

  private:
    StationMap stations;
    char ** station_name;
    float * vertex; // stations positions

    // each DEM point has 9 values: 3 pos. coords, 3 normal comps., 3 texture color comps
    float * dem; // dem points;
    int dem_cols;
    int dem_rows;
    int dem_stride; // DEM vertex stride

    unsigned short * index;
    int therion_ns; // nr. of stations
    int therion_nl; // nr. of legs
    int therion_nx; // nr. of splays

    float x_offset; // file-model offset
    float y_offset;
    float z_offset;
    float scale;    // model scale

  public:
    TherionModel( );

    ~TherionModel();

    void Init( const char * filename );

    const char ** GetStations();

    int GetNStations();

    const float * GetDEM() const { return dem; }
    int GetDEMCols() const { return dem_cols; }
    int GetDEMRows() const { return dem_rows; }
    int GetDEMStride() const { return dem_stride; }

  private:
    void InitFromTh( const char * filename );
    void InitFromLox( const char * filename );
}; 

#endif
