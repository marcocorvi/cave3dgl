#ifndef TRANSFORM_SHADER_H
#define TRANSFORM_SHADER_H

#include "Shader.h"
#include "Matrix4.h"

class TransformShader : public Shader
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
    TransformShader( float s );
    virtual ~TransformShader() { }

    virtual void LinkShader();
    virtual void SetupShader( Renderable & r );

    const Matrix4 * GetMVP() { return & mvp; }
};
#endif
