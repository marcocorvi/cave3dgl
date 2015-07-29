#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "Component.h"
#include "Transform.h"
#include "EventHandler.h"
#include "Status.h"

class TransformComponent : public Component
                         , public EventHandler
{
  private:
    static const unsigned int s_id = 0;

    Transform transform;
    Status * status;

  public:
    static unsigned int GetId() { return s_id; }

    explicit TransformComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
      , status( NULL )
    { }

    virtual ~TransformComponent() { }

    virtual void InitializeComponent();

    void SetStatus( Status * st ) { status = st; }

    Transform * GetComponentTransform() { return & transform; }

    void HandleEvent( EventType * evt );

};
#endif
