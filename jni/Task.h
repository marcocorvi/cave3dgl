#ifndef TASK_H
#define TASK_H

#include <string>
#include "TaskPriority.h"

class Task
{
  private:
    unsigned int priority;
    std::string  name;
    bool         canKill;

  public:
    explicit Task( unsigned int p, const char * n )
      : priority( p )
      , name( n )
      , canKill( false )
    { }
    virtual ~Task() { }

    const char * Name() const { return name.c_str(); }

    virtual bool Start()     = 0;
    virtual void OnSuspend() = 0;
    virtual void OnResume()  = 0;
    virtual void Update()    = 0;
    virtual void Stop()      = 0;

    void SetCanKill( bool c ) { canKill = c; }
    bool CanKill() const      { return canKill; }
    unsigned int Priority() const { return priority; }

};

#endif
