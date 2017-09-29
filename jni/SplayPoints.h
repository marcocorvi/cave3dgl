#ifndef SPLAY_POINTS_H
#define SPLAY_POINTS_H

#include <string>
#include <map>
#include "Geometry.h"
#include "Renderable.h"
#include "Shader.h"
#include "TherionModel.h"

class SplayPoints : public Renderable
                  , public Geometry
{
  private:
    float * vertex; // splay points positions [not owned]
    unsigned short * index; // splay indices [not owned]
    int therion_nx; // nr. of splays

    float x_offset; // file-model offset
    float y_offset;
    float z_offset;
    float scale;    // model scale

  public:
    SplayPoints( TherionModel * model, Shader * shader );

    ~SplayPoints();

    int GetNPoints() { return therion_nx; }

}; 

#endif
