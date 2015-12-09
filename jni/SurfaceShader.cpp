#include "SurfaceShader.h"

#include "Geometry.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Log.h"

SurfaceShader::SurfaceShader( )
  : Shader("surface")
{
  vertexShaderCode = 
    "uniform mat4 u_mModel; \n"
    "uniform mat3 u_mModelIT; \n"
    "attribute vec4 a_vPosition; \n"
    "attribute vec3 a_vNormal; \n"
    "attribute vec4 a_tColor; \n"
    "varying vec3 v_vNormal; \n"
    "varying vec4 v_tColor; \n"   // texture color
    "void main() { \n"
    "  gl_Position = u_mModel * a_vPosition; \n"
    "  v_vNormal  = normalize( u_mModelIT * a_vNormal ); \n"
    "  v_tColor   = a_tColor; \n"
    // "  v_tColor   = vec4( 1.0, 1.0, 1.0, 1.0 ); \n"
    "} \n";
  fragmentShaderCode = 
    "precision mediump float; \n"
    "varying vec3 v_vNormal; \n"
    "varying vec4 v_tColor; \n"   
    "uniform vec4 a_vColor; \n"   // ambient color
    "uniform vec4 d_vColor; \n"   // diffuse color
    "uniform vec3 u_vLight; \n"   // light direction
    "const float c_zero = 0.0; \n"
    // "const float c_one  = 1.0; \n"
    // "const vec4 v_zero = vec4(0,0,0,0); \n"
    "void main() { \n"
    "  float d = dot( v_vNormal, u_vLight ); \n"
    "  d = max( d, c_zero ); \n"
    "  gl_FragColor = (a_vColor + d *  d_vColor) * v_tColor; \n"
    // gl_FragColor = max( gl_FragColor, v_zero ); \n"
    "  gl_FragColor.a = 0.75; \n"
    // "  gl_FragColor = vec4( 1.0, 0, 0, 1.0 ); \n"
    "} \n";
}

void
SurfaceShader::LinkShader()
{
  if ( linked ) return;
  Shader::LinkShader();

  transformHandle   = glGetUniformLocation( programId, "u_mModel" );
  transformITHandle = glGetUniformLocation( programId, "u_mModelIT" );

  posHandle    = glGetAttribLocation( programId, "a_vPosition" );
  normHandle   = glGetAttribLocation( programId, "a_vNormal" );
  texHandle    = glGetAttribLocation( programId, "a_tColor" );

  ambColHandle = glGetUniformLocation( programId, "a_vColor" );
  difColHandle = glGetUniformLocation( programId, "d_vColor" );
  lightHandle  = glGetUniformLocation( programId, "u_vLight" );

  proj = Renderer::Instance()->Projection();

  // LOGI("Surface Shader: link %s program %d", Name(), programId );
}

void 
SurfaceShader::SetupShader( Renderable & renderable )
{
  Geometry * g = renderable.GetGeometry();
  if ( g != NULL ) {
    Shader::SetupShader( renderable );
    proj.Multiply( renderable.GetTransformMatrix(), mvp );
    glUniformMatrix4fv( transformHandle, 1, true, mvp.m_m ); // false: matrix is column-wise
    Matrix3 mat;
    mvp.GetRotation( mat );
    glUniformMatrix3fv( transformITHandle, 1, true, mat.m_m ); // false: matrix is column-wise

    glVertexAttribPointer( posHandle, g->NPos(), GL_FLOAT, GL_FALSE, g->VertexStride(), g->Vertex() );
    glEnableVertexAttribArray( posHandle );

    float * normal = ((float *)g->Vertex()) + g->NPos();
    glVertexAttribPointer( normHandle, g->NPos(), GL_FLOAT, GL_FALSE, g->VertexStride(), normal );
    glEnableVertexAttribArray( normHandle );

    glVertexAttribPointer( texHandle, g->NPos(), GL_FLOAT, GL_FALSE, g->VertexStride(), normal+3 );
    glEnableVertexAttribArray( texHandle );

    Vector4 & colA = renderable.GetAmbientColor();
    glUniform4f( ambColHandle, colA.m_x, colA.m_y, colA.m_z, colA.m_w );

    Vector4 & colD = renderable.GetDiffuseColor();
    glUniform4f( difColHandle, colD.m_x, colD.m_y, colD.m_z, colD.m_w );

    Vector3 light = Renderer::Instance()->GetLight();
    // light.Normalize();
    glUniform3f( lightHandle, light.m_x, light.m_y, light.m_z );
  }
}

