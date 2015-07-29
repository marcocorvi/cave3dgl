#ifndef EVENT_MANAGER_FRIEND_H
#define EVENT_MANAGER_FRIEND_H

#include "Log.h"

bool
registerEvent( EventId id )
{
  // LOGI("registerEvent %d", id );
  return EventManager::Instance()->RegisterEvent( id );
}

void
sendEvent( EventId id )
{
  // LOGI("sendEvent %d", id );
  EventManager::Instance()->SendEvent( id );
}

void
sendEventToHandler( EventId id, EventHandler & h )
{
  // LOGI("sendEventToHandler %d", id );
  EventManager::Instance()->SendEventToHandler( id, h );
}

void
attachEvent( EventId id, EventHandler & h )
{
  // LOGI("attach Event %d", id );
  EventManager::Instance()->AttachEvent( id, h );
}

void
detachEvent( EventId id, EventHandler & h )
{
  // LOGI("detachEventHandler %d %s", id, h.HandlerName() );
  EventManager::Instance()->DetachEvent( id, h );
}

#endif
