#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <math.h>
#include <stdio.h>

#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector3.h"


class Transform
{
  private:
    Matrix3  m_rotation;     // rotation about the center of mass
    Vector3  m_translation;  // center of mass
    Matrix4  m_matrix;

    float    m_scale;        // scale about the center of mass
    float    angleT, angleP;
    char statusString[1024];
  
  public:
    Transform()
      : m_translation( 0.0f, 0.0f, 0.0f )
      , m_scale( 1.0f )
      , angleT( 0.0f )
      , angleP( 0.0f )
    { }
    virtual ~Transform() { }
  
    void Clone(const Transform& transform);
  
    const char * ToString() 
    {
      // sprintf( statusString, "C %.0f %.0f %.0f Theta %.0f Phi %.0f",
      //   m_translation.m_y, -m_translation.m_x, m_translation.m_z,
      //   -angleT, angleP ); // was: -angleP, -angleT
      sprintf( statusString, "Theta %.0f Phi %.0f", -angleT, angleP ); // was: -angleP, -angleT
      return statusString;
    }


    // void  SetRotation(const Matrix3& rotation) 
    // { 
    //   m_rotation = rotation;
    //   UpdateMatrix();
    // }
    // const Matrix3& GetRotation() const { return m_rotation; }
  
    // void  SetTranslation(const Vector3& translation)
    // {
    //   m_translation = translation; 
    //   UpdateMatrix();
    // }
    const Vector3 & GetTranslation() const { return m_translation; }
  
    // void  SetScale(const float scale)
    // {
    //   m_scale = scale;
    //   UpdateMatrix();
    // }
    // const float GetScale() const { return m_scale; }

    // void  ApplyForward(const Vector3& in, Vector3& out) const;
    // void  ApplyInverse(const Vector3& in, Vector3& out) const;
  
    const Matrix4 & GetMatrix() const { return m_matrix; }
    // void  GetInverseMatrix(Matrix4& out) const;
    // void  GetInverseTransposeMatrix(Matrix4& out) const;
    // void  GetInverseTransposeMatrix(Matrix3& out) const;
  
    // Transform Multiply(const Transform& input) const;

    // void RotateZ( bool ccw );

    void TranslateCenter( float dx, float dy, float dz );
    void Zoom( float dz );
    void Rotate( float dt, float dp );

    void Log() const;

    void  UpdateMatrix();
  
};

#endif /* TRANSFORM_H_ */
