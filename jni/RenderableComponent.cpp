#include "RenderableComponent.h"

#include <stdlib.h>
#include "Log.h"
#include "Events.h"
#include "Renderer.h"
#include "EventType.h"
#include "TransformComponent.h"
#include "Object.h"

void
RenderableComponent::HandleEvent( EventType * evt )
{
  // LOGI("RenderableComponent::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case RENDER_EVENT:
      {
        // FIXME this could be done once for all by mainTask
        TransformComponent * tc = component_cast<TransformComponent>( Owner() );
        if ( tc != NULL ) { 
          renderable.SetTransform( tc->GetComponentTransform() );
        }

        Renderer::Instance()->AddRenderable( &renderable );
      }
      break;
  }
}

void
RenderableComponent::InitializeComponent()
{
  // LOGI("RenderableComponent::Initialize()" );
}

