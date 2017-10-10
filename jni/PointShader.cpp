#include "PointShader.h"

#include "Log.h"
#include "Geometry.h"
#include "Renderable.h"
#include "Renderer.h"

PointShader::PointShader( float s )
  : Shader( "point" )
  , scale( s )
{
  // LOGI( "PointShader::cstr()");
  // geometryShaderCode = 
  //   "layout(points) in; \n"
  //   "layout(line_strip, max_vertices=2) out; \n"
  //   "void main() { \n"
  //   "  gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); \n"
  //   "  EmitVertex(); \n";
  //   "  gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0); \n"
  //   "  EmitVertex(); \n"
  //   "  EndPrimitive(); \n"
  //   "} \n";

  vertexShaderCode = 
    "uniform mat4 u_mPoint; \n"
    "attribute vec4 a_vPosition; \n"
    "varying vec4 vColor; \n"
    "void main() { \n"
    "  gl_PointSize = 10.0; \n"
    "  gl_Position = u_mPoint * a_vPosition; \n"
    "  vColor = gl_Position; \n" // gl_Color
    "} \n";

  fragmentShaderCode = 
    "precision highp float; \n"
    "varying vec4 vColor; \n"
    "void main() { \n"
    "  gl_FragColor.r = \n"
    "  gl_FragColor.g = \n"
    "  gl_FragColor.b = vColor.b/20.0; \n"
    "  gl_FragColor.a = 1.0; \n"
    "} \n";
}

void
PointShader::LinkShader()
{
  if ( linked ) return;
  Shader::LinkShader();

  transformHandle = glGetUniformLocation( programId, "u_mPoint" );
  posHandle = glGetAttribLocation( programId, "a_vPosition" );
  colHandle = glGetAttribLocation( programId, "a_vColor" );

  proj = Renderer::Instance()->Projection();

  // LOGI( "PointShader::link %s scale %.2f handles %d %d %d", Name(), scale, transformHandle, posHandle, colHandle );
}

void
PointShader::SetupShader( Renderable & renderable )
{
  // LOGI( "Point Shader::Setup Shader()");
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

