#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>

class Image
{
  protected:               // let subclasses see them
    unsigned char * _img;  //!< Image pixel data
    unsigned int _width;   //!< Image width
    unsigned int _height;  //!< Image height
    unsigned int _stride;
    int _bpp; 
    bool _owner;           //!< This object owns the data memory

  public:
    Image()
      : _img( NULL )
      , _width( 0 )
      , _height( 0 )
      , _stride( 0 )
      , _bpp( 3 )
      , _owner( false )
    { }

    /** Memory-based constructor 
     * @param img pixel data
     * @param w   image width
     * @param h   image height
     */
    Image(unsigned char * img, unsigned int w, unsigned int h)
      : _img( img )
      , _width( w )
      , _height( h )
      , _owner( false )
    { }


    virtual ~Image()
    {
      if ( _owner && _img != NULL ) free( _img );
    }
  
    unsigned int width()   { return _width; }

    unsigned int height()  { return _height; }

    unsigned int stride() const { return _stride; }

    int BPP() const { return _bpp; }

    unsigned char * image() { return _img; }

    virtual bool open( const char * filename ) = 0;
  
    virtual bool open( FILE * fp ) = 0;

    virtual bool open( unsigned char * data, size_t size ) = 0;

};

#endif // IMAGE_H





