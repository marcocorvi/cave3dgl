#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <map>

#include "Singleton.h"
#include "EventManagerFcts.h"
#include "EventType.h"

class EventManager : public Singleton<EventManager>
{
  friend bool registerEvent( EventId id );
  friend void sendEvent( EventId id );
  friend void sendEventToHandler( EventId id, EventHandler & h );
  friend void attachEvent( EventId id, EventHandler & h );
  friend void detachEvent( EventId id, EventHandler & h );

  private:
    typedef std::map< EventId, EventType * >           EventMap;
    typedef std::map< EventId, EventType * >::iterator EventMapIterator;

    EventMap events;

    bool RegisterEvent( EventId id );
    void SendEvent( EventId id );
    void SendEventToHandler( EventId id, EventHandler & h );
    void AttachEvent( EventId id, EventHandler & h );
    void DetachEvent( EventId id, EventHandler & h );

  public: 
    EventManager()
    {
      // LOGI("EventManager::cstr()");
    }

  public:
    ~EventManager();

};

#endif
