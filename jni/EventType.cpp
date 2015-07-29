#include "EventType.h"
#include "EventHandler.h"
#include "Log.h"

void 
EventType::Send()
{
  // LOGI("EventType::Send() type %d handlers %d", id, handlers.size() );
  for (HandlerVectorIterator it = handlers.begin(); it != handlers.end(); ++it ) {
    if ( *it != NULL ) (*it)->HandleEvent( this );
  }
}

void 
EventType::SendToHandler( EventHandler & h )
{
  LOGI("EventType::SendToHandler() type %d: %s", id, h.HandlerName() );
  for (HandlerVectorIterator it = handlers.begin(); it != handlers.end(); ++it ) {
    if ( *it == &h ) {
      h.HandleEvent( this );
      break;
    }
  }
}

void 
EventType::AttachListener( EventHandler & h )
{
  // LOGI("EventType::AttachListener() type %d: %s", id, h.HandlerName() );
  for (HandlerVectorIterator it = handlers.begin(); it != handlers.end(); ++it ) {
    if ( *it == &h ) return;
  }
  handlers.push_back( &h );
}

void 
EventType::DetachListener( EventHandler & h )
{
  // LOGI("EventType::DetachListener() type %d: %s", id, h.HandlerName() );
  for (HandlerVectorIterator it = handlers.begin(); it != handlers.end(); ++it ) {
    if ( *it == &h ) {
      handlers.erase( it );
      break;
    }
  }
}
