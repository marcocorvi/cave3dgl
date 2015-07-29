#include "StatusComponent.h"

#include "Events.h"
#include "EventType.h"
#include "Renderer.h"
#include "Log.h"

void 
StatusComponent::InitializeComponent()
{
  LOGI("Status Component::Initialize()" );
}

void 
StatusComponent::HandleEvent( EventType * evt )
{
  // LOGI("Status Component::HandleEvent() %d ", evt->GetId() );
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case PRERENDER_EVENT:
      // geom "string" shader "texture"
      // LOGI("Status Component::HandleEvent() PRERENDER %s %s", status.GetGeometry()->Name(), status.GetShader()->Name() );
      Renderer::Instance()->AddRenderable( &status );
      break;
  }
}

