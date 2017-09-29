#include "Transform.h"

// Transform::Transform()
// {
//   m_rotation.Identify();
//   m_translation = Vector3(0.0f, 0.0f, 0.0f);
//   m_scale = 1.0f;
//   m_matrix.Identify();
// }

#include "Log.h"


void
Transform::Clone(const Transform& transform)
{
  // LOGI("Transform::Clone() " );
  m_rotation    = transform.m_rotation;
  m_translation = transform.m_translation;
  m_scale       = transform.m_scale;
  angleT        = transform.angleT;
  angleP        = transform.angleP;
  UpdateMatrix();
}

// void
// Transform::ApplyForward(const Vector3& in, Vector3& out) const
// {
//   out.Set(in);                // copy in to out
//   out.Multiply(m_scale);      // apply scale
//   m_rotation.Transform(out);  // rotate
//   out.Add(m_translation);     // translate
// }

// void
// Transform::ApplyInverse(const Vector3& in, Vector3& out) const
// {
//   out.Set(in);
//   out.Subtract(m_translation);
//   m_rotation.TransformTranspose(out);
//   out.Divide(m_scale);
// }

/* MAT = SCALE * TRANSL * ROT */
void
Transform::UpdateMatrix()
{
  m_matrix.m_m[0]  = m_rotation.m_m[0] * m_scale;
  m_matrix.m_m[1]  = m_rotation.m_m[1] * m_scale;
  m_matrix.m_m[2]  = m_rotation.m_m[2] * m_scale;
  m_matrix.m_m[3]  = m_translation.m_x * m_scale;
  m_matrix.m_m[4]  = m_rotation.m_m[3] * m_scale;
  m_matrix.m_m[5]  = m_rotation.m_m[4] * m_scale;
  m_matrix.m_m[6]  = m_rotation.m_m[5] * m_scale;
  m_matrix.m_m[7]  = m_translation.m_y * m_scale;
  m_matrix.m_m[8]  = m_rotation.m_m[6] * m_scale;
  m_matrix.m_m[9]  = m_rotation.m_m[7] * m_scale;
  m_matrix.m_m[10] = m_rotation.m_m[8] * m_scale;
  m_matrix.m_m[11] = m_translation.m_z * m_scale;
  m_matrix.m_m[12] = 0.0f;
  m_matrix.m_m[13] = 0.0f;
  m_matrix.m_m[14] = 0.0f;
  m_matrix.m_m[15] = 1.0f;
}


// void
// Transform::GetInverseMatrix(Matrix4& out) const
// {
//   float invScale = 1.0f / m_scale;
//   out.m_m[0] = m_rotation.m_m[0] * invScale;
//   out.m_m[1] = m_rotation.m_m[3];
//   out.m_m[2] = m_rotation.m_m[6];
//   out.m_m[3] = 0.0f;
//   out.m_m[4] = m_rotation.m_m[1];
//   out.m_m[5] = m_rotation.m_m[4] * invScale;
//   out.m_m[6] = m_rotation.m_m[7];
//   out.m_m[7] = 0.0f;
//   out.m_m[8] = m_rotation.m_m[2];
//   out.m_m[9] = m_rotation.m_m[5];
//   out.m_m[10] = m_rotation.m_m[8] * invScale;
//   out.m_m[11] = 0.0f;
//   out.m_m[12] = -m_translation.m_x;
//   out.m_m[13] = -m_translation.m_y;
//   out.m_m[14] = -m_translation.m_z;
//   out.m_m[15] = 1.0f;
// }
// 
// void
// Transform::GetInverseTransposeMatrix(Matrix4& out) const
// {
//   float invScale = 1.0f / m_scale;
//   out.m_m[0] = m_rotation.m_m[0] * invScale;
//   out.m_m[1] = m_rotation.m_m[1];
//   out.m_m[2] = m_rotation.m_m[2];
//   out.m_m[3] = 0.0f;
//   out.m_m[4] = m_rotation.m_m[3];
//   out.m_m[5] = m_rotation.m_m[4] * invScale;
//   out.m_m[6] = m_rotation.m_m[5];
//   out.m_m[7] = 0.0f;
//   out.m_m[8] = m_rotation.m_m[6];
//   out.m_m[9] = m_rotation.m_m[7];
//   out.m_m[10] = m_rotation.m_m[8] * invScale;
//   out.m_m[11] = 0.0f;
//   out.m_m[12] = -m_translation.m_x;
//   out.m_m[13] = -m_translation.m_y;
//   out.m_m[14] = -m_translation.m_z;
//   out.m_m[15] = 1.0f;
// }
// 
// void
// Transform::GetInverseTransposeMatrix(Matrix3& out) const
// {
//   float invScale = 1.0f / m_scale;
//   out.m_m[0] = m_rotation.m_m[0] * invScale;
//   out.m_m[1] = m_rotation.m_m[1];
//   out.m_m[2] = m_rotation.m_m[2];
//   out.m_m[3] = m_rotation.m_m[3];
//   out.m_m[4] = m_rotation.m_m[4] * invScale;
//   out.m_m[5] = m_rotation.m_m[5];
//   out.m_m[6] = m_rotation.m_m[6];
//   out.m_m[7] = m_rotation.m_m[7];
//   out.m_m[8] = m_rotation.m_m[8] * invScale;
// }

/**
     s 0   1 t   R 0    s 0   R t    sR st
     0 1   0 1   0 1  = 0 1   0 1  =  0  1
 *
     sR st   zP zd     szRP szRd+st
      0  1    0  1  =    0     1
 */
// Transform
// Transform::Multiply(const Transform& input) const
// {
//   Transform result;
//   result.m_rotation = m_rotation.Multiply(input.GetRotation());
//   result.m_translation.Set( m_translation );
//   result.m_translation.Multiply( 1.0f /  input.GetScale() );
//   result.m_translation.Add( result.m_rotation.Transform(input.GetTranslation()) );
//   result.m_scale = m_scale * input.GetScale();
//   result.UpdateMatrix();
//   return result;
// }

// void 
// Transform::RotateZ( bool ccw ) 
// {
//   m_rotation.RotateZ( ccw );
//   UpdateMatrix();
// }
  

void
Transform::TranslateCenter( float dx, float dy, float dz )
{
  m_translation.m_x += dy;
  m_translation.m_y += dx;
  m_translation.m_z += dz;

  // LOGI("Center %.3f %.3f %.3f", m_translation.m_x, m_translation.m_y, m_translation.m_z );
  // UpdateMatrix();
}

void
Transform::Zoom( float dz )
{
  m_translation.m_z *= dz;
  // LOGI("Zoom scale %.3f * %.3f", m_scale, dz );
  // m_scale *= dz;
  // UpdateMatrix();
}

void 
Transform::Log() const
{
  LOGI("T Center %.3f %.3f %.3f R %.2f %.2f",
    m_translation.m_x, m_translation.m_y, m_translation.m_z, angleT, angleP );
}

void 
Transform::Rotate( float dt, float dp )
{
  angleT -= dt/10;
  if ( angleT > 90 ) angleT = 90;
  if ( angleT < -90 ) angleT = -90;
  angleP -= dp/10; 
  while ( angleP >= 360 ) angleP -= 360;
  while ( angleP < 0 )    angleP += 360;
  // LOGI("Camera R %.2f %.2f ", angleT, angleP );

  float ct = (float)cos( angleT * 3.1415926536f / 180.0f );
  float st = (float)sin( angleT * 3.1415926536f / 180.0f );
  float cp = -(float)cos( angleP * 3.1415926536f / 180.0f );
  float sp = -(float)sin( angleP * 3.1415926536f / 180.0f );

  m_rotation.m_m[0] = ct;
  m_rotation.m_m[1] = - st * sp;
  m_rotation.m_m[2] = - st * cp;

  m_rotation.m_m[3] = 0.0f;
  m_rotation.m_m[4] =        cp;
  m_rotation.m_m[5] = -      sp;

  m_rotation.m_m[6] = st;
  m_rotation.m_m[7] =   ct * sp;
  m_rotation.m_m[8] =   ct * cp;

  // UpdateMatrix();
}

