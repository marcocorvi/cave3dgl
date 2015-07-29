#include "UserFile.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

UserFile::UserFile( std::string & name )
  : File( name )
  , fp( NULL )
{ }

UserFile::UserFile( const char * name )
  : File( name )
  , fp( NULL )
{ }

bool
UserFile::Open()
{
  fp = fopen( path.c_str(), "r" );
  return (fp != NULL);
}

void
UserFile::Read( void * buf, unsigned int tr, size_t & rd )
{
  rd = ( fp != NULL )? fread(buf, 1, tr, fp ) : 0;
}

void
UserFile::Close()
{
  if ( fp != NULL ) {
    fclose( fp );
    fp = NULL;
  }
}

unsigned int 
UserFile::Length() const 
{
  if ( fp != NULL ) {
    struct stat st;
    if ( stat( path.c_str(), &st ) == 0 ) {
      return st.st_size;
    }
  }
  return 0;
}

