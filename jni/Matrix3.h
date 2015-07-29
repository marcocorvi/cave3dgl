#ifndef MATRIX3_H
#define MATRIX3_H

class Vector3;
class Matrix4;

class Matrix3
{
  public:
    float m_m[9];
    
    Matrix3() { Identify(); }
    virtual ~Matrix3() { }
    
    void Identify();
    Vector3 Transform(const Vector3& in) const;
    void Transform(const Vector3& in, Vector3& out) const;
    Vector3 TransformTranspose(const Vector3& in) const;
    void RotateAroundX(const float radians);
    void RotateAroundY(const float radians);
    void RotateAroundZ(const float radians);
    // void RotateZ( bool ccw );
    
    Matrix3& operator=(const Matrix3& in);
    Matrix3& operator=(const Matrix4& in);
    
    Matrix3 Multiply(const Matrix3& input) const;
};

#endif /* MATRIX3_H_ */
