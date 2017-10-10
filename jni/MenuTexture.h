#ifndef MENU_TEXTURE_H
#define MENU_TEXTURE_H


#include "Texture.h"
#include "Geometry.h"
#include "CharMap.h"

#define IDX_STATIONS  1
#define IDX_SPLAYS   15
#define IDX_SURFACE  27

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

    // void ToggleStations() { ToggleIndex( IDX_STATIONS ); }
    // void ToggleSplays()   { ToggleIndex( IDX_SPLAYS ); }
    // void ToggleSurface()  { ToggleIndex( IDX_SURFACE ); }
    // void ToggleIndex( int j ) 
    // {
    //   menu_str[j] = ( menu_str[j] == STAR )? BLANK : STAR;
    //   UpdateString( j );
    // }

    // bool HasStations() const { return HasIndex( IDX_STATIONS ); }
    // bool HasSplays()   const { return HasIndex( IDX_SPLAYS ); }
    // bool HasSurface()  const { return HasIndex( IDX_SURFACE ); }
    // bool HasIndex( int j ) const { return ( menu_str[j] == STAR ); }

    Geometry * GetGeometry() { return & geometry; }

    void SetTheString( bool stations, bool splays, bool points, bool surface );
    void SetString();
    // void UpdateString( int j );
};

#endif
