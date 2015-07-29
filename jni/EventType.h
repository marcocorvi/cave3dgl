#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

#include <vector>

#include "Log.h"
#include "EventHandler.h"
class EventHandler;

typedef unsigned int EventId;

class EventType
{
  private:
    typedef std::vector<EventHandler *>           HandlerVector;
    typedef std::vector<EventHandler *>::iterator HandlerVectorIterator;

    HandlerVector handlers;
    EventId       id;

  public:
    explicit EventType( EventId i ) 
      : id( i )
    { 
      // LOGI("EventType::cstr id %d", id );
    }

    ~EventType() 
    {
      // LOGI("EventType::dstr id %d", id );
    }

    int Size() const { return handlers.size(); }

    void Send();

    void SendToHandler( EventHandler & h );
    
    void AttachListener( EventHandler & h );

    void DetachListener( EventHandler & h );

    unsigned int GetId() const { return id; }

};
#endif
