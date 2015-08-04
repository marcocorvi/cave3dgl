#ifndef MEMORY_SOURCE_H
#define MEMORY_SOURCE_H

#include <string.h>

#include "Log.h"

class MemorySource
{
  private:
    size_t pos;
    size_t len;
    unsigned char * mem; // not owned

  public:
    MemorySource( unsigned char * m, size_t l )
      : pos( 0 )
      , len( l )
      , mem( m )
    { }

    size_t read( unsigned char * outbuffer, size_t cnt )
    {
      // LOGI("memory read: pos %d len %d cnt %d", pos, len, cnt );
      if ( pos + cnt > len ) {
        LOGW("memory overrun: pos %d len %d cnt %d", pos, len, cnt );
        cnt = len - pos;
      }
      memcpy( outbuffer, mem+pos, cnt );
      pos += cnt;
      return cnt;
    }

};


#endif
