#include "TransformComponent.h"

#include "Log.h"
#include "InputState.h"
#include "EventType.h"
#include "Events.h"
#include "StatusComponent.h"

void
TransformComponent::InitializeComponent()
{
  // LOGI("TransformComponent initialize");
}


/** 
 * the MOTION events change the Trasnform stored inside the TransformComponent
 * this is then copied to that in the Renderable (on the RENDER event)
 */
void
TransformComponent::HandleEvent( EventType * evt )
{
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case MOTION_EVENT:
      {
        InputState * state = InputState::Instance();
        if ( state->Mode() == STATE_MOVE ) {
          // LOGI("Transl. %.3f %.3f zoom %.3f ", state->DeltaX(), state->DeltaY(), state->Zoom() );
          transform.TranslateCenter( state->DeltaX(), state->DeltaY(), 0.0f );
          transform.Zoom( state->Zoom() );
          transform.UpdateMatrix();
          if ( status ) {
            status->SetStatusString( transform.ToString() );
          }
        } else if ( state->Mode() == STATE_ROTATE ) {
          // LOGI("Rotat. %.3f %.3f", state->DeltaT(), state->DeltaP() );
          transform.Rotate( state->DeltaT(), state->DeltaP() );
          transform.UpdateMatrix();
          if ( status ) {
            status->SetStatusString( transform.ToString() );
          }
        }
        // transform.Log();
      }
      break;
  }
}
