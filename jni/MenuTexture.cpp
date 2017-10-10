#include "MenuTexture.h"

#include <string.h>
#include <GLES2/gl2.h>
#include "Log.h"
#include "CharMap.h"

float 
MenuTexture::vertex[4*5] = {
     0.88f, -0.90f, 0.0f, 0, 1, 0.88f, 0.90f, 0.0f, 1, 1,
     0.95f, -0.90f, 0.0f, 0, 0, 0.95f, 0.90f, 0.0f, 1, 0 
};

unsigned short 
MenuTexture::index[6] = { 0, 2, 1, 1, 2, 3 };

#define IDX_STATIONS  1
#define IDX_SPLAYS   15
#define IDX_SURFACE  27

MenuTexture::MenuTexture( )
  : geometry( GL_TRIANGLES, "menu", FLAG_MENU )
{ 
  //                 0        1    1    2      2  3
  //                0123456789012345678901234567890123456
  strcpy(menu_str, "[ ] Stations  [ ] Splays  [ ] Surface" );

  bpp = BPP;
  length = 40;
  width  = length * (WIDTH+2);
  stride = width * bpp;
  texData = new unsigned char[ stride * HEIGHT ];
  SetTexture( width, HEIGHT, bpp, texData );
  // width  = len * WIDTH; each char is 8 byte wide
  // height = HEIGHT;
  geometry.SetNVertex( 4 );
  geometry.SetNSIndex( 6 );
  geometry.SetVertex( (void *)vertex );
  geometry.SetSIndex( (void *)index );
  geometry.SetNPos( 3 );  // nr. vertex position elements
  geometry.SetNCol( 1 );
  geometry.SetNTex( 2 );  // nr. texture coord elements
  geometry.SetVertexStride( sizeof(float) * 5 );
  SetString( );
}


void 
MenuTexture::SetString( )
{
  // LOGI("Menu Texture set string: <%s> %dx%d stride %d bpp %d", menu_str, width, height, stride, bpp );
  // LOGI("Menu Texture set string: <%s> len %d", menu_str, len );

  CharMap * chars = CharMap::Instance();

  int len = strlen( menu_str );

  memset( texData, 0xff, stride * height );
  int xoff = 0;
  for ( int k=0; k<len && xoff+WIDTH*bpp<=stride; ++k) {
    unsigned char * shape = chars->Shape( menu_str[k] );
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

void
MenuTexture::SetTheString( bool stations, bool splays, bool points, bool surface )
{
  menu_str[IDX_STATIONS] = stations ? ' ' : 'x';
  menu_str[IDX_SPLAYS]   = splays ? (points? ' ' : '.') : (points? 'x' : '+');
  menu_str[IDX_SURFACE]  = surface ? ' ' : 'x';
  SetString();
}

// void
// MenuTexture::UpdateString( int j )
// {
//   CharMap * chars = CharMap::Instance();
// 
//   int xoff = j * 10 * bpp;
//   unsigned char * shape = chars->Shape( menu_str[j] );
//   for (unsigned char m = 0x80; m > 0; m>>=1) {
//     unsigned char * off = texData + xoff;
//     for ( int j=0; j<HEIGHT; ++j ) {
//       if ( (m & shape[ j ]) == m ) {
//         memset( off, 0, bpp );
//         off[bpp-1] = 0xff;
//       }
//       off += stride;
//     }
//     xoff += bpp;
//   }
// }
