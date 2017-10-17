
#ifndef POINTS_H
#define POINTS_H

#include <string>
#include <map>
#include "Geometry.h"
#include "Renderable.h"
#include "Shader.h"
#include "Model.h"

class Points : public Renderable
             , public Geometry
{
  private:
    float * vertex; // splay points positions [not owned]
    unsigned short * index; // splay indices [owned]
    int therion_nx;

    float x_offset; // file-model offset
    float y_offset;
    float z_offset;
    float scale;    // model scale

  public:
    Points( Model * model, Shader * shader );

    ~Points();

    int GetNPoints() { return therion_nx; }

    void UpdateDataBuffers( Model * model );
}; 

#endif
