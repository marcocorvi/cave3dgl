#ifndef STATIONS_COMPONENT_H
#define STATIONS_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "Stations.h"
#include "Vector4.h"
#include "Transform.h"
#include "Log.h"

class StationsComponent : public Component
                      , public EventHandler
{
  private:
    static const unsigned int s_id = 3;

    Stations * stations; // external (not owned)

  public:
    static unsigned int GetId() { return s_id; }

    explicit StationsComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
      , stations( NULL )
    { 
      // LOGI("StationsComponent cstr");
    }

    virtual ~StationsComponent()
    {
      // LOGI("StationComponent dstr ");
    }

    virtual void InitializeComponent();

    Stations * GetStations() { return stations; }
    void SetStations( Stations * s ) { stations = s; }

    void HandleEvent( EventType * evt );

};

#endif
