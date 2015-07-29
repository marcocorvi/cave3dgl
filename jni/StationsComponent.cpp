#include "StationsComponent.h"

#include "Events.h"
#include "EventType.h"
#include "Renderer.h"
#include "Log.h"

void 
StationsComponent::InitializeComponent()
{
  LOGI("Stations Component::Initialize()" );
}

void 
StationsComponent::HandleEvent( EventType * evt )
{
  // LOGI("Stations Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  if ( stations == NULL ) return;
  switch ( evt->GetId() ) {
    case PRERENDER_EVENT:
      Renderer::Instance()->AddRenderable( stations );
      break;
    case POSTMOTION_EVENT:
      // LOGI("Stations Component: handle event POSTMOTION");
      stations->UpdateStations();
      break;
  }
}

