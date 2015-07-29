#ifndef COMPONENT_H
#define COMPONENT_H

class Object;

#include <string>
#include "Log.h"

class Component
{
  private:
    Object * owner; // [not owned]
    std::string name;

  public:
    Component( Object * o, const char * n )
      : owner( o )
      , name( n )
    {
      // LOGI("Component cstr %s", name.c_str() );
    }
    virtual ~Component()
    {
      // LOGI("Component dstr %s", name.c_str() );
    }
    
    const char * Name() const { return name.c_str(); }

    virtual void InitializeComponent() = 0;

    Object * Owner() { return owner; }

};

#endif
