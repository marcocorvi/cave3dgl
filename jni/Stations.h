#ifndef STATIONS_H
#define STATIONS_H

#include <assert.h>
#include "StationTexture.h"
#include "TextureShader.h"
#include "Renderable.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Geometry.h"
#include "Log.h"

class Stations : public Renderable
{
  private:
    StationTexture stations;

    TextureShader shader;
    const Matrix4 * mvp;

  public:
    Stations( int ns, Geometry * geom, const char ** names, const Matrix4 * mat )
      : stations( ns, geom, names )
      , mvp( mat )
    { 
      assert( mvp );
      shader.SetTexture( & stations );

      // Renderable fields:
      SetGeometry( stations.GetMyGeometry() );
      SetShader( & shader );
      // SetTransform( NULL );
      // SetColor( color );
    }

    Texture * GetTexture() { return & stations; }

    void UpdateStations() 
    {
      if ( mvp == NULL ) {
        // LOGI("Stations: update station texture MVP is null");
        return;
      }
      stations.ComputeVertices( mvp );
    }

};

#endif
