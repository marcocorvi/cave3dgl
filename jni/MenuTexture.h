#ifndef MENU_TEXTURE_H
#define MENU_TEXTURE_H


#include "Texture.h"
#include "Geometry.h"
#include "CharMap.h"

#define INDEX_STATIONS  1
#define INDEX_SPLAYS   15
#define INDEX_SURFACE  27

#define BLANK ' '
#define STAR  '+'


class MenuTexture : public Texture
{
  private:
    unsigned int length;
    unsigned int stride;
    unsigned char * texData; // [owned]
    char menu_str[40];
    Geometry geometry;

    static float vertex[4*5];
    static unsigned short index[6];

  public:
    MenuTexture( ); 

    ~MenuTexture()
    {
      if ( texData ) delete[] texData;
    }

    void ToggleStations() { ToggleIndex( INDEX_STATIONS ); }
    void ToggleSplays()   { ToggleIndex( INDEX_SPLAYS ); }
    void ToggleSurface()  { ToggleIndex( INDEX_SURFACE ); }
    void ToggleIndex( int j ) 
    {
      LOGI("Menu Texture toggle %d ", j);
      menu_str[j] = ( menu_str[j] == STAR )? BLANK : STAR;
      UpdateString( j );
    }

    bool HasStations() const { return HasIndex( INDEX_STATIONS ); }
    bool HasSplays()   const { return HasIndex( INDEX_SPLAYS ); }
    bool HasSurface()  const { return HasIndex( INDEX_SURFACE ); }
    bool HasIndex( int j ) const { return ( menu_str[j] == STAR ); }

    Geometry * GetGeometry() { return & geometry; }

    void SetString();
    void UpdateString( int j );
};

#endif
