#include "SurfaceComponent.h"

#include "Events.h"
#include "EventType.h"
#include "Renderer.h"
#include "TransformComponent.h"
#include "Object.h"

void 
SurfaceComponent::InitializeComponent()
{
}

void 
SurfaceComponent::HandleEvent( EventType * evt )
{
  // LOGI("Surface Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case RENDER_EVENT:
      if ( surface != NULL ) {
        // FIXME should i do this ? yes
        TransformComponent * tc = component_cast<TransformComponent>( Owner() );
        if ( tc != NULL ) { 
          surface->SetTransform( tc->GetComponentTransform() );
        }
        Renderer::Instance()->AddRenderable( surface );
      }
      break;
  }
}
