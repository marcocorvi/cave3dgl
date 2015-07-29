#include "TherionFlip.h"

#include <ctype.h>

int ParseFlip( std::string flip )
{
  if ( flip == "horizontal" ) return FLIP_HORIZONTAL;
  if ( flip == "vertical" ) return FLIP_VERTICAL;
  return FLIP_NONE;
}

char * Trim( char * ch )
{
  while ( *ch != 0 && isspace( *ch ) ) ++ ch;
  if ( *ch == 0 ) return ch; // empty string

  char * ch2 = ch;
  while ( *ch2 != 0 && *ch2 != '\n' ) ++ ch2;
  while ( ch2 > ch && isspace( *ch2 ) ) -- ch2;
  ++ch2;
  *ch2 = 0; 
  return ch;
}

void RemoveComment( char * ch )
{
  char * ch2 = ch;
  while ( *ch2 != 0 ) {
    if ( *ch2 == '#' ) {
      while ( ch2 > ch && isspace( ch2[-1] ) ) -- ch2;
      *ch2 = 0;
      return;
    }
    ++ch2;
  }
}

size_t SplitOnSpaces( char * ch, std::vector<const char *> & vals )
{
  vals.clear();
  char * ch2 = ch;
  while ( *ch2 != 0 ) {
    while ( *ch2 != 0 && isspace( *ch2 ) ) ++ ch2;
    if ( *ch2 == 0 ) break;
    char * ch1 = ch2;
    while ( *ch2 != 0 && ! isspace( *ch2 ) ) ++ ch2;
    vals.push_back( ch1 );
    if ( *ch2 == 0 ) break;
    *ch2 = 0; // terminate c-string ch1
    ++ ch2;   // move forward
  }
  return vals.size();
}
