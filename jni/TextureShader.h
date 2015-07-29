#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "Shader.h"
#include "Texture.h"

class TextureShader : public Shader
{
  private:
    Texture * texture;
    GLint     posHandle;
    GLint     texHandle;
    GLint     samplerHandle;

  public:
    TextureShader( );
    virtual ~TextureShader() { }

    virtual void LinkShader();
    virtual void SetupShader( Renderable & r );

    void SetTexture( Texture * t ) { texture = t; }
    // Texture * GetTexture() { return texture; }

};
#endif
