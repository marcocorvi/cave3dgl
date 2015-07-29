#include "Vector4.h"


void Vector4::Set(Vector4& in)
{
  m_x = in.m_x;
  m_y = in.m_y;
  m_z = in.m_z;
  m_w = in.m_w;
}

void Vector4::Multiply(float scalar)
{
  m_x *= scalar;
  m_y *= scalar;
  m_z *= scalar;
  m_w *= scalar;
}

void Vector4::Divide(float scalar)
{
  float divisor = 1.0f / scalar;
  m_x *= divisor;
  m_y *= divisor;
  m_z *= divisor;
  m_w *= divisor;
}

void Vector4::Add(Vector4& in)
{
  m_x += in.m_x;
  m_y += in.m_y;
  m_z += in.m_z;
  m_w += in.m_w;
}

void Vector4::Subtract(Vector4& in)
{
  m_x -= in.m_x;
  m_y -= in.m_y;
  m_z -= in.m_z;
  m_w -= in.m_w;
}

Vector4& Vector4::operator=(const Vector4& in)
{
  m_x = in.m_x;
  m_y = in.m_y;
  m_z = in.m_z;
  m_w = in.m_w;
  return *this;
}

