#include "TransformShader.h"

#include "Log.h"
#include "Geometry.h"
#include "Renderable.h"
#include "Renderer.h"

TransformShader::TransformShader( float s )
  : Shader( "transform" )
  , scale( s )
{
  // LOGI( "TransformShader::cstr()");
  vertexShaderCode = 
    "uniform mat4 u_mModel; \n"
    "attribute vec4 a_vPosition; \n"
    "void main() { \n"
    "  gl_Position = u_mModel * a_vPosition; \n"
    "} \n";
  fragmentShaderCode = 
    "precision highp float; \n"
    "uniform vec4 a_vColor; \n"
    "void main() { \n"
    "  gl_FragColor = a_vColor; \n"
    "  gl_FragColor.a = 1.0; \n"
    "} \n";
}

void
TransformShader::LinkShader()
{
  if ( linked ) return;
  Shader::LinkShader();

  transformHandle = glGetUniformLocation( programId, "u_mModel" );
  posHandle = glGetAttribLocation( programId, "a_vPosition" );
  colHandle = glGetAttribLocation( programId, "a_vColor" );

  proj = Renderer::Instance()->Projection();

  LOGI( "TransformShader::link %s scale %.2f handles %d %d %d", Name(), scale, transformHandle, posHandle, colHandle );
}

void
TransformShader::SetupShader( Renderable & renderable )
{
  // LOGI( "TransformShader::Setup()");
  Geometry * g = renderable.GetGeometry();
  if ( g != NULL ) {
    Shader::SetupShader( renderable );
    // Renderer::Instance()->Camera().Multiply( renderable.GetTransformMatrix(), mvp1 );
    // proj.Multiply( mvp1, mvp );
    proj.Multiply( renderable.GetTransformMatrix(), mvp );
    glUniformMatrix4fv( transformHandle, 1, true, mvp.m_m ); // false: matrix is column-wise
    glVertexAttribPointer( posHandle, g->NPos(), GL_FLOAT, GL_FALSE, g->VertexStride(), g->Vertex() );
    glEnableVertexAttribArray( posHandle );

    // for ( int k=0; k<4; ++k ) {
    //   LOGI("M %5.2f %5.2f %5.2f %5.2f", mvp.m_m[4*k+0], mvp.m_m[4*k+1], mvp.m_m[4*k+2], mvp.m_m[4*k+3] );
    // }

    // float * v = (float *)g->Vertex();    
    // for ( int k=0; k<g->NVertex(); ++k ) {
    //   Vector4 vv( v[3*k+0], v[3*k+1], v[3*k+2], 1.0f );
    //   Vector4 ww = mvp.Multiply( vv );
    //   LOGI("V %d: %5.2f %5.2f %5.2f %5.2f", k, ww.m_x, ww.m_y, ww.m_z, ww.m_w );
    // }

    Vector4 & color = renderable.GetAmbientColor();
    glUniform4f( colHandle, color.m_x, color.m_y, color.m_z, color.m_w );
  }
}

