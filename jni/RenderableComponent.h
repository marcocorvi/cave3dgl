#ifndef RENDERABLE_COMPONENT_H
#define RENDERABLE_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "Renderable.h"

class RenderableComponent : public Component
                          , public EventHandler
{
  private:
    static const unsigned int s_id = 1;

    Renderable renderable;

  public:
    static unsigned int GetId() { return s_id; }

    explicit RenderableComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
    { }
    virtual ~RenderableComponent() { }

    virtual void InitializeComponent();

    Renderable & GetRenderable() { return renderable; }

    virtual void HandleEvent( EventType * evt );


};
#endif
