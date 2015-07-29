#ifndef STRING_TEXTURE_H
#define STRING_TEXTURE_H


#include "Texture.h"
#include "Geometry.h"
#include "CharMap.h"

class StatusTexture : public Texture
{
  private:
    unsigned int length;
    unsigned int stride;
    unsigned char * texData; // [owned]
    Geometry geometry;

    static float vertex[4*5];
    static unsigned short index[6];

  public:
    StatusTexture( unsigned int len = 80 ); // 80-chars

    ~StatusTexture()
    {
      if ( texData ) delete[] texData;
    }

    void SetString( const char * str );

    Geometry * GetGeometry() { return & geometry; }

};

#endif
