#ifndef SURFACE_SHADER_H
#define SURFACE_SHADER_H

#include "Shader.h"
#include "Matrix4.h"

class SurfaceShader : public Shader
{
  private:
    Matrix4 proj;
    // Matrix4 mvp1;  // = CameraMatrix * TransformMatrix
    Matrix4 mvp;   // = ProjectionMatrix * CameraMatrix * TransformMatrix
    GLint   transformHandle;
    GLint   transformITHandle;
    GLint   posHandle;
    GLint   normHandle;
    GLint   texHandle;
    GLint   ambColHandle;
    GLint   difColHandle;
    GLint   lightHandle;
    
  public:
    SurfaceShader( );
    virtual ~SurfaceShader( ) { }

    virtual void LinkShader();
    virtual void SetupShader( Renderable & r );

};

#endif
