#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

class EventType;

#include <string>

class EventHandler
{
  private:
    std::string name;

  public:
    EventHandler( const char * n )
      : name( n )
    { } 

    virtual ~EventHandler() {}

    virtual void HandleEvent( EventType * event ) = 0;

    const char * HandlerName() const { return name.c_str(); }

};

#endif
