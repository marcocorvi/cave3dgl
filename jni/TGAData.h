#ifndef TGA_DATA_H
#define TGA_DATA_H

class TGAData
{
  public:
    struct TGAHeader
    {
      unsigned char idSize;
      unsigned char colorMapType;
      unsigned char imageType;

      unsigned short paletteStart;
      unsigned short paletteLength;
      unsigned char  paletteBits;

      unsigned short xOrigin;
      unsigned short yOrigin;
      unsigned short width;
      unsigned short height;
      unsigned char  bpp;
      unsigned char  descriptor;
    } __attribute__ ((packed));

  private:
    TGAHeader * header;
    void *      data;

  public:
    TGAData( void * h, void * d )
      : header( (TGAHeader *)h )
      , data( d )
    { }
    virtual ~TGAData() { }

    unsigned short Width() const  { return header? header->width : 0; }
    unsigned short Height() const { return header? header->height : 0; }
    
    void * Data() { return data; }

};

#endif
