#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#define EXTERN extern "C"

#include "Image.h"
#include "../../libpng-android/jni/png.h"

class Image_PNG : public Image
{
  private:

  public:
    Image_PNG() { }

    bool open( const char * filename );

    bool open( FILE * fp );

    bool open( unsigned char * data, size_t size );

    // bool save_image(const char * filename);

  private:
    bool readPNGdata( png_structp png_ptr, png_infop info_ptr, int mode );

};

#endif // Image_PNG





