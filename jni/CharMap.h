#ifndef CHAR_MAP_H
#define CHAR_MAP_H


#include "Singleton.h"

#define WIDTH   8
#define HEIGHT 13
#define BPP     4

class CharMap : public Singleton< CharMap >
{
  private:
    static unsigned char charMap[256];
    static unsigned char charShape[][ HEIGHT ]; // fonts

  public:
    CharMap() { InitCharMap(); }

    ~CharMap() { }

    // unsigned char CharIndex( unsigned char ch ) const
    // {
    //   return charMap[ ch ];
    // }

    unsigned char * Shape( unsigned char ch ) const
    {
      return charShape[ charMap[ ch ] ];
    }

  private:
    static void InitCharMap();

};

#endif

