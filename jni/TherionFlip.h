#ifndef THERION_FLIP_H
#define THERION_FLIP_H

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#define FLIP_NONE       0
#define FLIP_HORIZONTAL 1
#define FLIP_VERTICAL   2
#define EQUALS(a,b) (( strcmp(a,b) == 0))
#define TO_FLOAT(a) ((float)atof(a))
#define TO_INT(a) ((int)atoi(a))

int ParseFlip( std::string flip );
char * Trim( char * ch );
void RemoveComment( char * ch );
size_t SplitOnSpaces( char * ch, std::vector<const char *> & vals );

#endif
