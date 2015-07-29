#include "Texture.h"

#include "Log.h"

void
Texture::InitTexture()
{
  packBits    = 4;
  intFormat   = GL_RGBA;
  format      = GL_RGBA;

  switch ( bpp ) {
    case 1:
      packBits  = 1;
      intFormat = GL_ALPHA;
      format    = GL_ALPHA;
      break;
  }

  if ( ! inited ) {
    glGenTextures( 1, &id );  // nr, array-pointer
    // LOGI("Init Texture %dx%d id %d ", width, height, id );
  }
  glBindTexture( GL_TEXTURE_2D, id );
  glPixelStorei( GL_UNPACK_ALIGNMENT, packBits );
  glTexImage2D( GL_TEXTURE_2D, 0, intFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data );

  inited = true;
}
