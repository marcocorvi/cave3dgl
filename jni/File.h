#ifndef FILE_H
#define FILE_H

#include <string>

class File
{
  protected:
    std::string path;

  public:
    File( std::string & p ) 
      : path( p )
    { }
    File( const char * p )
      : path( p )
    { }
    virtual ~File() { }

    virtual bool Open() = 0;
    virtual void Read( void * buf, unsigned int bytes_to_read, size_t & bytes_read ) = 0;
    virtual void Close() = 0;

    virtual unsigned int Length() const { return 0; }
};

#endif
