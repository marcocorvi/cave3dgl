


#include <stdlib.h>
#include "Log.h"

#include "Object.h"
#include "Events.h"
#include "Renderer.h"
#include "EventType.h"
#include "TransformComponent.h"
#include "PointsComponent.h"


void
PointsComponent::InitializeComponent()
{
  // LOGI("Points Component::Initialize()" );
}

void 
PointsComponent::HandleEvent( EventType * evt )
{
  // LOGI("Points Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  if ( points == NULL ) return;
  switch ( evt->GetId() ) {
    case RENDER_EVENT:
      TransformComponent * tc = component_cast<TransformComponent>( Owner() );
      if ( tc != NULL ) { 
        points->SetTransform( tc->GetComponentTransform() );
      }
      Renderer::Instance()->AddRenderable( points );
      break;
  }
}

