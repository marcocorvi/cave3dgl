#ifndef STATIONS_H
#define STATIONS_H

#include <assert.h>
#include "StationTexture.h"
#include "TextureShader.h"
#include "Renderable.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Geometry.h"
#include "Station.h"
#include "Model.h"
#include "Log.h"

class Stations : public Renderable
{
  private:
    StationTexture stationTexture;

    TextureShader shader;
    const Matrix4 * mvp;

  public:
    Stations( int ns, Geometry * geom, const Station * names, const Matrix4 * mat )
      : stationTexture( ns, geom, names )
      , mvp( mat )
    { 
      assert( mvp );
      shader.SetTexture( & stationTexture );

      // Renderable fields:
      SetGeometry( stationTexture.GetMyGeometry() );
      SetShader( & shader );
      // SetTransform( NULL );
      // SetColor( color );
    }

    Texture * GetTexture() { return & stationTexture; }

    void UpdateStations() 
    {
      if ( mvp == NULL ) {
        // LOGI("Stations: update station texture MVP is null");
        return;
      }
      stationTexture.ComputeVertices( mvp );
    }


    void UpdateStations( Geometry * geom )
    {
      Model * model = dynamic_cast< Model * >( geom );
      int ns = model->GetNStations();
      const Station * name = model->GetStations();
      if ( name == NULL ) {
        LOGI("update stations model has null stations");
        return;
      }

      LOGI("update stations model has %d stations", ns );
      stationTexture.SetStationNames( ns, name );
      stationTexture.ComputeVertices( mvp );
    }
};

#endif
