#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdlib.h>
#include <GLES2/gl2.h>

class Texture
{
  private:
    GLuint id;
    GLint packBits;
    GLint intFormat;
    GLint format;

  protected:
    unsigned int width;
    unsigned int height;
    unsigned int bpp;  // bytes per pixel
    unsigned int size; // data size
    void * data; // image data (not owned)
    bool inited;

  public:
    Texture()
      : packBits( 4 )
      , intFormat( GL_RGBA )
      , format( GL_RGBA )
      , width( 0 )
      , height( 0 )
      , bpp( 4 )
      , size( 0 )
      , data( NULL )
      , inited( false )
    { }

    ~Texture() {}

    GLint PackBits() const { return packBits; }
    GLint IntFormat() const { return intFormat; }
    GLint Format() const { return format; }

    void SetTexture( unsigned int w, unsigned int h, unsigned int b, void * d )
    {
      width = w;
      height = h;
      bpp  = b;
      size = w * b * h;
      data = d;
    }

    GLuint Id() const { return id; }

    void InitTexture();

};
#endif
