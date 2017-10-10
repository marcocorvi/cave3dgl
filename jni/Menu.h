#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include "MenuTexture.h"
#include "TextureShader.h"
#include "Renderable.h"
#include "Log.h"

class Menu : public Renderable
{
  private:
    MenuTexture  menu;
    TextureShader shader;

  public:
    Menu( )  // size = message length
    { 
      // LOGI("Menu cstr; length %d ", size );
      shader.SetTexture( & menu );
      SetGeometry( menu.GetGeometry() );
      SetShader( & shader );
    }

    void SetMenuString( bool stations, bool splays, bool points, bool surface )
    {
      menu.SetTheString( stations, splays, points, surface );
      // shader.SetTexture( & menu );
    }

};

#endif
