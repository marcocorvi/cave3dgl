#ifndef LOX_FILE_H
#define LOX_FILE_H

#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include <vector>

#include "LoxSurvey.h"
#include "LoxStation.h"
#include "LoxShot.h"
#include "LoxScrap.h"
#include "LoxSurface.h"
#include "LoxBitmap.h"


class LoxFile
{
  private:
    struct Chunk_t
    {
      uint32_t type;
      uint32_t rec_size;
      uint32_t rec_cnt;
      uint32_t data_size;
      void * records;
      void * data;

      size_t size() const { return rec_cnt; }

      Chunk_t()
        : type( 0 )
        , rec_size( 0 )
        , rec_cnt( 0 )
        , data_size( 0 )
        , records( NULL )
        , data( NULL )
      { }

      ~Chunk_t()
      {
        if ( records ) free( records );
        if ( data ) free( data );
      }
    };

  private:
    Chunk_t mSurveyChunk;
    Chunk_t mStationChunk;
    Chunk_t mShotChunk;
    Chunk_t mScrapChunk;
    Chunk_t mSurfaceChunk;
    Chunk_t mBitmapChunk;

    std::vector< LoxSurvey * >  mSurveys;
    std::vector< LoxStation * > mStations;
    std::vector< LoxShot * >    mShots;
    std::vector< LoxScrap * >   mScraps;
    LoxSurface *               mSurface;
    LoxBitmap *                mBitmap;

  public:
    LoxFile()
      : mSurface( NULL )
      , mBitmap( NULL )
    { }

    ~LoxFile() { Clear(); }

    size_t NrSurveys() const  { return mSurveyChunk.size(); }
    size_t NrStations() const { return mStationChunk.size(); }
    size_t NrShots() const    { return mShotChunk.size(); }
    size_t NrScraps() const   { return mScrapChunk.size(); }
    size_t NrSurfaces() const { return mSurfaceChunk.size(); }
    size_t NrBitmaps() const  { return mBitmapChunk.size(); }

    std::vector< LoxSurvey * > &  GetSurveys()  { return mSurveys; }
    std::vector< LoxStation * > & GetStations() { return mStations; }
    std::vector< LoxShot * > &    GetShots()    { return mShots; }
    std::vector< LoxScrap * > &   GetScraps()   { return mScraps; }
    LoxSurface *                  GetSurface()  { return mSurface; }
    LoxBitmap *                   GetBitmap()   { return mBitmap; }

    void ReadFile( const char * filename );

  private:
    void Clear();

    uint32_t ReadChunk( FILE * fp ); // return type of chunk
    void HandleSurvey( );
    void HandleStations( );
    void HandleShots( );
    void HandleScraps( );
    void HandleSurface( );
    void HandleBitmap( );

};

#endif
