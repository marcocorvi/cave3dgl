#ifndef VECTOR3_H_
#define VECTOR3_H_

#include "Vector4.h"

class Vector3
{
  public:
    float m_x;
    float m_y;
    float m_z;
  
    Vector3()
      : m_x( 0 )
      , m_y( 0 )
      , m_z( 0 )
    { }

    Vector3(const float x, const float y, const float z)
      : m_x( x )
      , m_y( y )
      , m_z( z )
    { }

    Vector3( const Vector3 & v )
      : m_x( v.m_x )
      , m_y( v.m_y )
      , m_z( v.m_z )
    { }
      
    virtual ~Vector3() { }
  
    void    Set(const Vector3& in);
    void    Multiply(const float scalar);
    void    Divide(const float scalar);
    void    Add(const Vector3& in);
    void    Subtract(const Vector3& in);
    void    Negate();
    float   Length() const;
    float   LengthSquared() const;
    void    Normalize();
    void    GetNormal(Vector3& normal);
  
    Vector3& operator=(const Vector3& in);
    Vector3& operator=(const Vector4& in);
  
    float  Dot(const Vector3& in) const;
    Vector3  Cross(const Vector3& in) const;
};

#endif /* VECTOR3_H_ */
