#ifndef FILE_MODEL_H
#define FILE_MODEL_H

#include <string>
#include <map>
#include "Model.h"
#include "Geometry.h"

class FileModel : public Model
{
  private:

    // each DEM point has 9 values: 3 pos. coords, 3 normal comps., 3 texture color comps
    float * dem; // dem points;
    int dem_cols;
    int dem_rows;
    int dem_stride; // DEM vertex stride

  public:
    FileModel( );

    ~FileModel();

    void Init( const char * filename );

    const float * GetDEM() const { return dem; }
    int GetDEMCols() const { return dem_cols; }
    int GetDEMRows() const { return dem_rows; }
    int GetDEMStride() const { return dem_stride; }

    void addPoint( double e, double n, double z ) { } // TODO

  private:
    void InitFromTh( const char * filename );
    void InitFromLox( const char * filename );
}; 

#endif
