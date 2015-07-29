
#include "StatusTexture.h"

#include <string.h>
#include <GLES2/gl2.h>
#include "Log.h"
#include "CharMap.h"

float 
StatusTexture::vertex[4*5] = {
     -0.95f, -0.90f, 0.0f, 0, 1, -0.95f, 0.90f, 0.0f, 1, 1,
     -0.88f, -0.90f, 0.0f, 0, 0, -0.88f, 0.90f, 0.0f, 1, 0 
};

unsigned short 
StatusTexture::index[6] = { 0, 2, 1, 1, 2, 3 };


StatusTexture::StatusTexture( unsigned int len )
  : length( len )
  , geometry( GL_TRIANGLES, "status", FLAG_STATUS )
{ 
  // LOGI("Status Texture cstr length %d", len );
  bpp = BPP;
  width = len * (WIDTH+2);
  stride = width * bpp;
  texData = new unsigned char[ stride * HEIGHT ];
  SetTexture( width, HEIGHT, bpp, texData );
  // width  = len * WIDTH; each char is 8 byte wide
  // height = HEIGHT;
  geometry.SetNVertex( 4 );
  geometry.SetNIndex( 6 );
  geometry.SetVertex( (void *)vertex );
  geometry.SetIndex( (void *)index );
  geometry.SetNPos( 3 );  // nr. vertex position elements
  geometry.SetNCol( 1 );
  geometry.SetNTex( 2 );  // nr. texture coord elements
  geometry.SetVertexStride( sizeof(float) * 5 );

  SetString("Cave 3D");
}


void 
StatusTexture::SetString( const char * str )
{
  int len = (length < strlen( str ))? length : strlen(str);
  // LOGI("Status Texture set string: <%s> %dx%d stride %d bpp %d", str, width, height, stride, bpp );
  // LOGI("Status Texture set string: <%s> len %d", str, len );

  CharMap * chars = CharMap::Instance();

  memset( texData, 0xff, stride * height );
  int xoff = 0;
  for ( int k=0; k<len && xoff+WIDTH*bpp<=stride; ++k) {
    unsigned char * shape = chars->Shape( str[k] );
    for (unsigned char m = 0x80; m > 0; m>>=1) {
      unsigned char * off = texData + xoff;
      for ( int j=0; j<HEIGHT; ++j ) {
        if ( (m & shape[ j ]) == m ) {
          memset( off, 0, bpp );
          off[bpp-1] = 0xff;
        }
        off += stride;
      }
      xoff += bpp;
    }
    xoff += 2 * bpp;
  }
  // InitTexture(); // do not initailize here: makes a black rectangle
}

