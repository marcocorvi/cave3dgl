#include "EventType.h"
#include "EventHandler.h"
#include "EventManager.h"

#include <stdlib.h>

#include "Log.h"
#include "EventManagerFriends.h"
// =======================================================

bool
EventManager::RegisterEvent( EventId id )
{
  // LOGI("EventManager::Register Event id %d size %d", id, events.size() );
  EventMapIterator it = events.find( id );
  if ( it == events.end() ) {
    EventType * evt = new EventType( id );
    if ( evt ) {
      std::pair<unsigned int, EventType*> pair( id, evt );
      std::pair< EventMapIterator, bool > ret = events.insert( pair );
      // LOGI("EventManager::Register Event size %d", events.size() );
      return ret.second;
    }
  } else {
    // LOGI("EventManager::Register Event id %d already present", id );
  }
  return false;
}

void 
EventManager::SendEvent( EventId id )
{
  // LOGI("EventManager::SendEvent %d", id );
  EventMapIterator it = events.find( id );
  if ( it != events.end() ) {
    it->second->Send();
  }
}

void
EventManager::SendEventToHandler( EventId id, EventHandler & h )
{
  // LOGI("EventManager::SendEventToHandler %d to %s", id, h.HandlerName() );
  EventMapIterator it = events.find( id );
  if ( it != events.end() ) {
    it->second->SendToHandler( h );
  }
}

void
EventManager::AttachEvent( EventId id, EventHandler & h )
{
  // LOGI("EventManager::AttachEvent %d to %s", id, h.HandlerName() );
  EventMapIterator it = events.find( id );
  if ( it != events.end() ) {
    it->second->AttachListener( h );
  }
}

void 
EventManager::DetachEvent( EventId id, EventHandler & h )
{
  // LOGI("EventManager::DetachEvent %d to %s", id, h.HandlerName() );
  EventMapIterator it = events.find( id );
  if ( it != events.end() ) {
    it->second->DetachListener( h );
  }
}

EventManager::~EventManager()
{
  LOGI("EventManager::dstr()");
  for ( EventMapIterator it = events.begin(); it != events.end(); ++it ) {
    if ( it->second ) {
      LOGI("delete Event type %d size %d\n", it->first, it->second->Size() );
      delete it->second;
      // it->second = NULL;
    }
  }
}
