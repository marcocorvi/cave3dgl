#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#define EXTERN extern "C"

#include "Image.h"

class Image_PNG : public Image
{
  private:
    int _bpp; 
    int _stride;

  public:
    Image_PNG() { }

    bool open( const char * filename );

    // bool save_image(const char * filename);

    int stride() const { return _stride; }

    int BPP() const { return _bpp; }
};

#endif // Image_PNG





