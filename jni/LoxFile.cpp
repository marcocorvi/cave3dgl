#include "LoxFile.h"

#include <stdio.h>
#include <assert.h>
// #include <endian.h>

#include <stdint.h>

#define SIZEDBL 8 // ( sizeof( double ) )
#define SIZE32  4 // ( sizeof( uint32_t ) )

#define SIZE_SURVEY  ( ( 6 * SIZE32 + 0 * SIZEDBL ) )
#define SIZE_STATION ( ( 7 * SIZE32 + 3 * SIZEDBL ) )
#define SIZE_SHOT    ( ( 5 * SIZE32 + 9 * SIZEDBL ) )
#define SIZE_SCRAP   ( ( 8 * SIZE32 + 0 * SIZEDBL ) )
#define SIZE_SURFACE ( ( 5 * SIZE32 + 6 * SIZEDBL ) )

#include "Log.h"

#ifdef BIG_ENDIAN
  uint32_t toLEndian( uint32_t val )
  {
    uint32_t ret;
    unsigned char * dst  = (unsigned char *)(&ret);
    unsigned char * head = (unsigned char *)(&val);
    unsigned char * tail = head + SIZE32;
    while ( head < tail ) {
      --tail;
      *dst = *tail;
      ++dst;
    }
    return ret;
  }
  
  double toLEndian( double val )
  {
    double ret;
    unsigned char * dst  = (unsigned char *)(&ret);
    unsigned char * head = (unsigned char *)(&val);
    unsigned char * tail = head + SIZE32;
    while ( head < tail ) {
      --tail;
      *dst = *tail;
      ++dst;
    }
    return ret;
  }
#else
  #define toLEndian( v ) ((v))
#endif

void
LoxFile::Clear()
{
  for ( std::vector< LoxSurvey * >::iterator it = mSurveys.begin(); it != mSurveys.end(); ++it ) {
    delete *it;
  }
  mSurveys.clear();
  for ( std::vector< LoxStation * >::iterator it = mStations.begin(); it != mStations.end(); ++it ) {
    delete *it;
  }
  mStations.clear();
  for ( std::vector< LoxShot * >::iterator it = mShots.begin(); it != mShots.end(); ++it ) {
    delete *it;
  }
  mShots.clear();
  for ( std::vector< LoxScrap * >::iterator it = mScraps.begin(); it != mScraps.end(); ++it ) {
    delete *it;
  }
  mScraps.clear();
  if ( mSurface ) delete mSurface; 
  mSurface = NULL;
  if ( mBitmap ) delete mBitmap;
  mBitmap = NULL;
}

uint32_t 
LoxFile::ReadChunk( FILE * fp ) 
{
  uint32_t type;
  if ( fread( &(type), SIZE32, 1, fp ) != 1 ) return (uint32_t)(-1);
  Chunk_t * c = NULL;
  switch ( type ) {
    case 1: // SURVEY
      c = & mSurveyChunk;
      break;
    case 2: // STATIONS
      c = & mStationChunk;
      break;
    case 3: // SHOTS
      c = & mShotChunk;
      break;
    case 4: // SCRAPS
      c = & mScrapChunk;
      break;
    case 5: // SURFACE
      c = & mSurfaceChunk;
      break;
    case 6: // SURFACE_BITMAP
      c = & mBitmapChunk;
      break;
    default:
      LOGW("Unexpected chunk type %d\n", type );
      return type;
  }
  c->type = type;
  if ( fread( &(c->rec_size),  SIZE32, 1, fp ) != 1 ) return EOF;
  if ( fread( &(c->rec_cnt),   SIZE32, 1, fp ) != 1 ) return EOF;
  if ( fread( &(c->data_size), SIZE32, 1, fp ) != 1 ) return EOF;
  // LOGI("Type %d RecSize %d RecCnt %d DataSize %d", c->type, c->rec_size, c->rec_cnt, c->data_size );
  if ( c->rec_size > 0 ) {
    c->records = (void *)malloc( c->rec_size );
    fread( c->records, 1, c->rec_size, fp );
  }
  if ( c->data_size > 0 ) {
    c->data = (void *)malloc( c->data_size );
    fread( c->data, 1, c->data_size, fp );
  }
  // LOGI("Read: %d bytes", 4 * SIZE32 + c->rec_size + c->data_size );
  return type;
}

void 
LoxFile::HandleSurvey( )
{
  int i;
  int n0 = mSurveyChunk.rec_cnt;
  // LOGI("Handle Survey: Nr. %d ", n0 );
  uint32_t * recs = (uint32_t *)(mSurveyChunk.records);
  char * data = (char *)(mSurveyChunk.data);
  char name[128];
  char title[128];
  for ( i=0; i<n0; ++i ) {
    uint32_t id = toLEndian( recs[6*i + 0] );
    uint32_t np = toLEndian( recs[6*i + 1] );
    uint32_t ns = toLEndian( recs[6*i + 2] );
    uint32_t pnt= toLEndian( recs[6*i + 3] );
    uint32_t tp = toLEndian( recs[6*i + 4] );
    uint32_t ts = toLEndian( recs[6*i + 5] );
    if ( ns > 0 ) memcpy( name, data+np, ns );
    name[ns] = 0;
    if ( ts > 0 ) memcpy( title, data+tp, ts );
    title[ts] = 0;
    mSurveys.push_back( new LoxSurvey( id, pnt, name, title ) );
    // LOGI("%d / %d: Survey %d (parent %d) Name %d \"%s\" Title %d \"%s\"", i, n0, id, pnt, ns, name, ts, title );
  }
  // LOGI("Handle Survey done");
}


void 
LoxFile::HandleStations( )
{
  int i;
  int n0 = mStationChunk.rec_cnt;
  unsigned char * recs = (unsigned char *)mStationChunk.records;
  char * data = (char *)mStationChunk.data;
  char name[128];
  char comment[128];
  for ( i=0; i<n0; ++i ) {
    size_t off = i * SIZE_STATION;
    uint32_t id = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t sid= toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t np = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t ns = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t tp = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t ts = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t fl = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    double c0   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double c1   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double c2   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    memcpy( name, data+np, ns );  name[ns] = 0;
    memcpy( comment, data+tp, ts ); comment[ts] = 0;
    // LOGI("Station %d (survey %d) Name \"%s\" Title \"%s\" Flags %d %.2f %.2f %.2f",
    //   id, sid, name, comment, fl, c0, c1, c2 );
    mStations.push_back( new LoxStation( id, sid, name, comment, fl, c0, c1, c2 ) );
  }
}


void 
LoxFile::HandleShots( )
{
  int i;
  int n0 = mShotChunk.rec_cnt;
  unsigned char * recs = (unsigned char *)mShotChunk.records;
  // void * data = (char *)mShotChunk.data;
  for ( i=0; i<n0; ++i ) {
    size_t off = i * SIZE_SHOT;
    uint32_t fr = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t to = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    double f0   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double f1   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double f2   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double f3   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double t0   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double t1   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double t2   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
    double t3   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;

    // flag: SURFACE DUPLICATE NOT_VISIBLE NOT_LRUD SPLAY
    uint32_t fl = toLEndian( *((uint32_t*)(recs+off))) ; off += SIZE32;
    // type: NONE OVAL SQUARE DIAMOND TUNNEL
    uint32_t ty = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;

    uint32_t sid= toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    double tr   = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL; // vthreshold

    // LOGI("Shot %d %d (%d) Flag %d Type %d thr %.2f", fr, to, sid, fl, ty, tr );
    // LOGI("  From-LRUD %.2f %.2f %.2f %.2f", f0, f1, f2, f3 );
    // LOGI("  To-LRUD %.2f %.2f %.2f %.2f", t0, t1, t2, t3 );

    mShots.push_back( new LoxShot( fr, to, sid, fl, ty, tr, f0, f1, f2, f3, t0, t1, t2, t3 ) );
  }
}


void 
LoxFile::HandleScraps( )
{
  int i;
  int n0 = mScrapChunk.rec_cnt;
  unsigned char * recs = (unsigned char *)mScrapChunk.records;
  unsigned char * data = (unsigned char *)mScrapChunk.data;
  for ( i=0; i<n0; ++i ) {
    size_t off = i * SIZE_SCRAP;
    uint32_t id = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t sid= toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t np = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t pp = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t ps = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t na = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t ap = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    uint32_t as = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
    // LOGI("Scrap %d (Survey %d) N.pts %d %d %d N.ang %d %d %d Size %d",
    //   id, sid, np, pp, ps, na, ap, as, mScrapChunk.data_size );
    assert( pp + np * 3 * sizeof(double) == ap );
    assert( np * 3 * sizeof(double) == ps );
    assert( na * 3 * SIZE32 == as );
    double * ptr = (double *)( data + pp );
    for ( i=0; i<np; ++i) {
      double x = ptr[3*i+0] = toLEndian( ptr[3*i + 0] );
      double y = ptr[3*i+1] = toLEndian( ptr[3*i + 1] );
      double z = ptr[3*i+2] = toLEndian( ptr[3*i + 2] );
      // LOGI("  Point %d: %.2f %.2f %.2f", i, x, y, z );
    }
    uint32_t * itr = (uint32_t *)( data + ap );
    for ( i=0; i<na; ++i ) {
      uint32_t x = itr[3*i+0] = toLEndian( itr[3*i + 0] );
      uint32_t y = itr[3*i+1] = toLEndian( itr[3*i + 1] );
      uint32_t z = itr[3*i+2] = toLEndian( itr[3*i + 2] );
      // LOGI("  3Angle %d: %d %d %d", i, x, y, z );
    }

    mScraps.push_back( new LoxScrap( id, sid, np, na, ptr, itr ) );
  }
}


void 
LoxFile::HandleSurface( )
{
  int i;
  int n0 = mSurfaceChunk.rec_cnt;
  unsigned char * recs = (unsigned char *)mSurfaceChunk.records;
  unsigned char * data = (unsigned char *)mSurfaceChunk.data;
  size_t off = 0;
  uint32_t id = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  uint32_t ww = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  uint32_t hh = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  uint32_t dp = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  uint32_t ds = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  double c[6];
  c[0]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[1]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[2]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[3]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[4]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[5]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  LOGI("Surface %d %dx%d Calib %.2f %.2f %.2f %.2f %.2f %.2f",
    id, ww, hh, c[0], c[1], c[2], c[3], c[4], c[5] );
  int npts = ww * hh;
  assert( ds == npts * sizeof(double) );
  double * ptr = (double *)( data + dp );
  for ( i=0; i< npts; ++i ) {
    double z = ptr[i] = toLEndian( ptr[i] );
    // LOGI("  Z: %.2f", z );
  }
  if ( mSurface ) delete mSurface;
  mSurface = new LoxSurface( id, ww, hh, c, ptr );
}


void 
LoxFile::HandleBitmap( )
{
  int n0 = mBitmapChunk.rec_cnt;
  unsigned char * recs = (unsigned char *)mBitmapChunk.records;
  unsigned char * data = (unsigned char *)mBitmapChunk.data;
  size_t off = 0;
  uint32_t id = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32; // surface id
  uint32_t tp = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32; // type: JPEG PNG
  uint32_t dp = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  uint32_t ds = toLEndian( *((uint32_t*)(recs+off)) ); off += SIZE32;
  double c[6];
  c[0]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[1]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[2]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[3]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[4]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  c[5]  = toLEndian( *((double*)(recs+off)) );   off += SIZEDBL;
  LOGI("Bitmap %d Type %d Calib %.2f %.2f %.2f %.2f %.2f %.2f File off %d size %d",
    id, tp, c[0], c[1], c[2], c[3], c[4], c[5], dp, ds );
  // image file binary data
  unsigned char * img = data + dp;
  LOGI("File %x %x %x %x %x %x %x %x", 
    img[0], img[1], img[2], img[3], img[4], img[5], img[6], img[7] );
  if ( mBitmap ) delete mBitmap;
  mBitmap = new LoxBitmap( id, tp, ds, c, img );
}

void 
LoxFile::ReadFile( const char * filename )
{
  FILE * fp = fopen( filename, "rb" );
  if ( fp == NULL ) {
    LOGW("Cannot open file \"%s\"\n", filename );
  } else {
    // LOGI("Lox read file \"%s\"", filename );
    bool repeat = true;
    do {
      uint32_t type = ReadChunk( fp );
      // LOGI("Read File: chunk type %d ", type );
      switch ( type ) {
        case 1: // SURVEY
          HandleSurvey( );
          break;
        case 2: // STATIONS
          HandleStations( );
          break;
        case 3: // SHOTS
          HandleShots( );
          break;
        case 4: // SCRAPS
          HandleScraps( );
          break;
        case 5: // SURFACE
          HandleSurface( );
          break;
        case 6: // SURFACE_BITMAP
          HandleBitmap( );
          break;
        default:
          repeat = false;
      }
    } while ( repeat );
    fclose( fp );
  }
}

// int main( int argc, char ** argv )
// {
//   if ( argc < 2 ) {
//     fprintf( stderr, "Usage: %s <lox-file>\n", argv[0] );
//     return 0;
//   }
//   parse_lox( argv[1] );
//   return 0;
// }
