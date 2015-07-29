#ifndef MATRIX4_H_
#define MATRIX4_H_

class Vector3;
class Vector4;
class Matrix3;

class Matrix4
{
  public:
    enum Rows
    {
      X,
      Y,
      Z,
      W,
      NUM_ROWS
    };
  
    float m_m[16];
  
    Matrix4() { Identify(); }
    virtual ~Matrix4() {}
  
    void Identify();

    void GetRotation( Matrix3 & mat );
    Vector3 Transform(const Vector3& in) const;
    void Transform(const Vector3& in, Vector3 & out) const;
    Vector3 TransformTranspose(const Vector3& in) const;
    Vector4 Multiply(const Vector4& in) const;
    void Multiply(const Vector4& in, Vector4 & out) const;
    void RotateAroundX(float radians);
    void RotateAroundY(float radians);
    void RotateAroundZ(float radians);
    void Multiply(const Matrix4& in, Matrix4& out) const;
  
    Matrix4 Transpose() const;
  
    Matrix4& operator=(const Matrix3& in);
    Matrix4& operator=(const Matrix4& in);
  
    Vector4  GetRow(const Rows row) const;
};

#endif /* MATRIX4_H_ */
