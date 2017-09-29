#include "Shader.h"

#include "Log.h"

void
Shader::LoadShader( GLuint id, std::string & code )
{
  // LOGI("Shader::LoadShader() %s id %d ", Name(), id );
  const static uint32_t NUM_SHADERS = 1;
  const GLchar * c = code.c_str();
  GLint len        = code.length();
  GLint compiled;

  glShaderSource( id, NUM_SHADERS, &c, &len );
  glCompileShader( id );
  glGetShaderiv( id, GL_COMPILE_STATUS, &compiled);

  glAttachShader( programId, id );
}

void
Shader::LinkShader()
{
  if ( linked ) return;
  programId = glCreateProgram();

  // if ( ! geometryShaderCode.empty() ) {
  //   geometryShaderId = glCreateShader( GL_GEOMETRY_SHADER );
  //   LoadShader( geometryShaderId, geometryShaderCode );
  // }

  vertexShaderId = glCreateShader( GL_VERTEX_SHADER );
  LoadShader( vertexShaderId, vertexShaderCode );

  fragmentShaderId = glCreateShader( GL_FRAGMENT_SHADER );
  LoadShader( fragmentShaderId, fragmentShaderCode );

  glLinkProgram( programId );

  GLint lk;
  glGetProgramiv(programId, GL_LINK_STATUS, &lk);
  linked = (lk != 0);
  if ( !linked ) {
    GLint infoLen = 0;
    glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &infoLen );
    
    if (infoLen > 1) {
      char* infoLog = new char[infoLen];
      glGetProgramInfoLog( programId, infoLen, NULL, infoLog );
      LOGW("Error linking program %s: %s\n", Name(), infoLog);
      delete[] infoLog;
    } else {
      LOGW("Error linking program %s\n", Name() );
    }
    glDeleteProgram(programId);
  }
  // LOGI("Shader::link() %s program %d shaders %d %d: %d", Name(), programId, vertexShaderId, fragmentShaderId, lk );
}

void 
Shader::SetupShader( Renderable & r )
{ 
  LinkShader();
  // LOGI("Shader::setup() %s program %d ", Name(), programId );
  glUseProgram( programId );
}
