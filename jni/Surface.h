#ifndef SURFACE_H
#define SURFACE_H

#include "Geometry.h"
#include "Renderable.h"
#include "Shader.h"
// #include "SurfaceShader.h"
#include "Log.h"

class Surface : public Renderable
              , public Geometry
{
  private:
    // Shader * shader;
    float * vertex_data;
    unsigned short * index_data;
    int stride; // vertex stride (3 only surface, 6 also normal)
    Vector4 ambientColor;
    Vector4 diffuseColor;

  public:
    Surface( const float * coords, int ne, int nn, int ns, Shader * s );

    ~Surface()
    {
      // LOGI("Surface dstr");
      if ( vertex_data ) delete[] vertex_data;
      if ( index_data ) delete[] index_data;
    }

    Vector4 & GetAmbientColor() { return ambientColor; }
    Vector4 & GetDiffuseColor() { return diffuseColor; }


};

#endif
