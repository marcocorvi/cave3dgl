#include "MenuComponent.h"

#include "Events.h"
#include "Renderer.h"

void 
MenuComponent::HandleEvent( EventType * evt )
{
  // LOGI("Menu Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case PRERENDER_EVENT:
      // LOGI("Menu Component::HandleEvent() PRERENDER ");
      Renderer::Instance()->AddRenderable( &menu );
      break;
  }
}
