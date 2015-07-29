#ifndef SURFACE_COMPONENT_H
#define SURFACE_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "Surface.h"
// #include "Vector4.h"
// #include "Transform.h"
#include "Log.h"

class SurfaceComponent : public Component
                       , public EventHandler
{
  private:
    static const unsigned int s_id = 4;

    Surface * surface;  // [not owned]

  public:
    static unsigned int GetId() { return s_id; }

    explicit SurfaceComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
      , surface( NULL )
    {
    }

    virtual ~SurfaceComponent() { }

    virtual void InitializeComponent();

    Surface * GetSurface() { return surface; }
    void SetSurface( Surface * s ) { surface = s; }

    void HandleEvent( EventType * evt );

};

#endif
