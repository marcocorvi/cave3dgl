#ifndef VECTOR4_H_
#define VECTOR4_H_

class Vector4
{
  public:
    float m_x;
    float m_y;
    float m_z;
    float m_w;
  
    Vector4()
      :  m_x(0.0f)
      ,  m_y(0.0f)
      ,  m_z(0.0f)
      ,  m_w(0.0f)
    { }
    Vector4(const float x, const float y, const float z, const float w)
      :  m_x( x )
      ,  m_y( y )
      ,  m_z( z )
      ,  m_w( w )
    { }
    virtual ~Vector4() { };
  
    void Set(Vector4& in);
    void Multiply(float scalar);
    void Divide(float scalar);
    void Add(Vector4& in);
    void Subtract(Vector4& in);
  
    Vector4& operator=(const Vector4& in);
};

#endif /* VECTOR4_H_ */
