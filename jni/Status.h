#ifndef STATUS_H
#define STATUS_H

#include <stdlib.h>
#include "StatusTexture.h"
#include "TextureShader.h"
#include "Renderable.h"
#include "Vector4.h"
#include "Log.h"

class Status : public Renderable
{
  private:
    StatusTexture str;
    // Vector4 color;

    TextureShader shader;

  public:
    Status( int size )  // size = message length
      : str( size )
      // , color( 1.0f, 0.0f, 0.0f, 1.0f )
    { 
      // LOGI("Status cstr; length %d ", size );

      shader.SetTexture( & str );
      // Renderable fields:
      SetGeometry( str.GetGeometry() );
      SetShader( & shader );
      // SetTransform( NULL );
      // SetColor( color );
    }

    void SetStatusString( const char * msg )
    { 
      // LOGI("Set status string <%s>", msg );
      str.SetString( msg );
    }

    // Texture * GetStatusTexture() { return & str; }


};

#endif
