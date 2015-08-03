#ifndef IMAGE_JPG_H
#define IMAGE_JPG_H

#include "Image.h"

class Image_JPG : public Image
{
  private:

  public:
    /** JPG file based constructor 
     * @param filename input file name
     */
    Image_JPG() {}

    bool open( const char * filename );

    bool open( FILE * fp );
    
    /** Saving the image data to a JPG file 
     * @param filename output file name
     */
    // bool save_image(const char * filename);
};

#endif // IMAGE_JPG_H





