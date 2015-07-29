#include "TextureShader.h"

#include "Geometry.h"
#include "Renderable.h"
#include "Log.h"

/* 
 * vertex format
 *   X Y Z u v
 * where (X,Y,Z) 3D point in world space
 *       (u,v) texture coords (origin at bottom-left)
 * Therefore the basic geometry is
 *   nr. vertex     4
 *   nr. indices    6 (two triengles)
 *   position size  3
 *   texture size   2
 *   vertex stride  5
 * More generally
 *   nr. vertex     N * M
 *   nr. indice     3 * 2 * (N-1) * (M-1)
 * If vertices are listed row-wise
 *   x0 y0 z0 u0 v0 x1 y1 z1 u1 v1 ... xM-1 ...
 *   xM yM zM uM vM ...
 * indices:
 *   0 M 1 M M+1 1 1 M+1 2 M+1 M+2 2 ...
 *   M 2M M+1 ...
 *
 *   +---+---+--
 *   | .'| .'|
 *   +'--+'--+--
 *   |  '|  '|
 */

TextureShader::TextureShader( )
  : Shader( "texture" )
  , texture( NULL )
{
  vertexShaderCode   = 
    "attribute vec4 a_vPosition; \n"
    "attribute vec2 a_texCoord; \n"
    "varying   vec2 v_texCoord; \n"
    "void main() { \n"
    "  gl_Position = a_vPosition; \n"
    "  v_texCoord  = a_texCoord; \n"
    "} \n";
  fragmentShaderCode =
    "precision highp float; \n"
    "varying vec2 v_texCoord; \n"
    "uniform sampler2D s_texture; \n"
    "void main() { \n"
    "  gl_FragColor = texture2D( s_texture, v_texCoord ); \n"
    "} \n";
}

void
TextureShader::LinkShader()
{
  if ( linked ) return;
  Shader::LinkShader();

  posHandle     = glGetAttribLocation( programId, "a_vPosition" );
  texHandle     = glGetAttribLocation( programId, "a_texCoord" );
  samplerHandle = glGetUniformLocation( programId, "s_texture" );

  // LOGI("Texture Shader: link %s program %d", Name(), programId );
}

void 
TextureShader::SetupShader( Renderable & r )
{
  Shader::SetupShader( r );
  Geometry * g = r.GetGeometry();
  if ( texture ) texture->InitTexture();

  // LOGI("Texture Shader: setup geom %s id %d", g->Name(), texture->Id() );
  // glEnable( GL_BLEND );
  // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texture->Id() );
  glUniform1i( samplerHandle, 0 );
  
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  // glPixelStorei( GL_UNPACK_ALIGNMENT, texture->PackBits() );
  // glTexImage2D( GL_TEXTURE_2D, 0, intFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data );

  glVertexAttribPointer( posHandle, g->NPos(), GL_FLOAT, GL_FALSE, g->VertexStride(), g->Vertex() );
  glEnableVertexAttribArray( posHandle );

  glVertexAttribPointer( texHandle, g->NTex(), GL_FLOAT, GL_FALSE, g->VertexStride(),
                         &(static_cast<GLfloat*>( g->Vertex()) [g->NPos()] ) );
  glEnableVertexAttribArray( texHandle );

}

