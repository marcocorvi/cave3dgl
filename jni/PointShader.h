#ifndef POINT_SHADER_H
#define POINT_SHADER_H

#include "Shader.h"
#include "Matrix4.h"

class PointShader : public Shader
{
  private:
    Matrix4 proj;
    // Matrix4 mvp1;  // = CameraMatrix * TransformMatrix
    Matrix4 mvp;   // = ProjectionMatrix * CameraMatrix * TransformMatrix
    GLint   transformHandle;
    GLint   posHandle;
    GLint   colHandle;
    float   scale;

  public:
    PointShader( float s );
    virtual ~PointShader() { }

    virtual void LinkShader();
    virtual void SetupShader( Renderable & r );

    const Matrix4 * GetMVP() { return & mvp; }
};
#endif
