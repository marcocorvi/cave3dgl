#ifndef EVENT_MANAGER_FCTS_H
#define EVENT_MANAGER_FCTS_H

#include "EventType.h"
bool registerEvent( EventId id );
void sendEvent( EventId id );
void sendEventToHandler( EventId id, EventHandler & h );
void attachEvent( EventId id, EventHandler & h );
void detachEvent( EventId id, EventHandler & h );

#endif

