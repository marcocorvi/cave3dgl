#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GLES2/gl2.h>

class Renderable;

class Shader
{
  private:
    void LoadShader( GLuint id, std::string & code );

  protected:
    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    // GLuint geometryShaderId;
    GLuint programId; 

    std::string vertexShaderCode;
    std::string fragmentShaderCode;
    // std::string geometryShaderCode;

    bool linked;
    std::string name;

  public:
    Shader( const char * n )
      : vertexShaderId(GL_INVALID_VALUE)
      , fragmentShaderId(GL_INVALID_VALUE)
      // , geometryShaderId(GL_INVALID_VALUE)
      , programId(GL_INVALID_VALUE)
      , linked( false )
      , name( n )
    { }
    virtual ~Shader() { }

    const char * Name() const { return name.c_str(); }

    virtual void LinkShader();
    virtual void SetupShader( Renderable &r );

    bool IsLinked() const { return linked; }

};
#endif
