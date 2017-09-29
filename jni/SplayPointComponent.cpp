#include "SplayPointComponent.h"

#include <stdlib.h>
#include "Log.h"
#include "Events.h"
#include "Renderer.h"
#include "EventType.h"
#include "TransformComponent.h"
#include "Object.h"


void
SplayPointComponent::InitializeComponent()
{
  // LOGI("SplayPointComponent::Initialize()" );
}

void 
SplayPointComponent::HandleEvent( EventType * evt )
{
  // LOGI("SplayPoint Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  if ( splayPoints == NULL ) return;
  switch ( evt->GetId() ) {
    case RENDER_EVENT:
      TransformComponent * tc = component_cast<TransformComponent>( Owner() );
      if ( tc != NULL ) { 
        splayPoints->SetTransform( tc->GetComponentTransform() );
      }
      Renderer::Instance()->AddRenderable( splayPoints );
      break;
  }
}

