#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#define EXTERN extern "C"

#include "Image.h"

class Image_PNG : public Image
{
  private:

  public:
    Image_PNG() { }

    bool open( const char * filename );

    bool open( FILE * fp );

    // bool save_image(const char * filename);

};

#endif // Image_PNG





