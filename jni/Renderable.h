#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <stdlib.h>

#include "Transform.h"
#include "Vector4.h"
#include "Log.h"

class Geometry;
class Shader;

class Renderable
{
  private:
    Geometry  * geometry;
    Shader    * shader;
    Transform * transform;
    Vector4     ambientColor;     // ambient color
    Vector4     diffuseColor;

  public:
    Renderable() 
      : geometry( NULL )
      , shader( NULL )
      , ambientColor( 1.0f, 0.0f, 0.0f, 1.0f )
      , diffuseColor( 0.0f, 0.0f, 1.0f, 1.0f )
      , transform( NULL )
    {
      // LOGI("Renderable::cstr()");
    }
    ~Renderable() 
    {
      // LOGI("Renderable::cstr()");
    }

    void SetGeometry( Geometry * g ) { geometry = g; }
    Geometry * GetGeometry() { return geometry; }

    void SetShader( Shader * s ) { shader = s; }
    Shader * GetShader() { return shader; }

    void SetAmbientColor( Vector4 & c ) { ambientColor = c; }
    Vector4 & GetAmbientColor() { return ambientColor; }
    void SetDiffuseColor( Vector4 & c ) { diffuseColor = c; }
    Vector4 & GetDiffuseColor() { return diffuseColor; }

    void SetTransform( Transform * t ) { transform = t; }
    bool HasTransform() const { return transform != NULL; }
    const Matrix4 & GetTransformMatrix() const { return transform->GetMatrix(); }
    const Vector3 & GetTransformTranslation() const { return transform->GetTranslation(); }

    // virtual void Initialize() 
    // {
    //   // LOGI("Renderable::Initialize()");
    // }

    Vector3 GetBoundMin() const;
    Vector3 GetBoundMax() const;

};

#endif
