#ifndef LOX_SURVEY_H
#define LOX_SURVEY_H

#include <stdint.h>

#include <string>

#include "Log.h"

class LoxSurvey
{
  private:
    uint32_t id;
    uint32_t pid; // parent id
    std::string name;
    std::string title;

  public:
    LoxSurvey( uint32_t _id, uint32_t _pid, const char * n, const char * t )
      : id( _id )
      , pid( _pid )
      , name( n )
      , title( t )
    { 
      // LOGI("Survey cstr %d", id );
    }

    uint32_t Id() const { return id; }
    uint32_t Parent() const { return pid; }

    const char * Name() const { return name.c_str(); }
    const char * Title() const { return title.c_str(); }

};

#endif
