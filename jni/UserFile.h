#ifndef USER_FILE_H
#define USER_FILE_H

#include "File.h"

#include <stdio.h>

class UserFile : public File
{
  private:
    FILE * fp;

  public:
    UserFile( std::string & filename );
    UserFile( const char * filename );
    ~UserFile() { Close(); }

    bool Open();
    void Read( void * buf, unsigned int tr, size_t & rd );
    void Close();

    unsigned int Length() const;
};

#endif
