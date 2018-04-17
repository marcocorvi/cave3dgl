#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <GLES/gl.h>

#include "DistoXModel.h"
#include "Log.h"
#include "Functions.h"

#include "mainTask.h" // definition included here

#define ZOFF ((-10))

#define STATION_0 "0"
// #define STATION_X "X"
// #define STATION_Y "Y"
// #define STATION_Z "Z"

#define IBUFFER_SIZE 4  // splays
#define LBUFFER_SIZE 4  // legs
#define VBUFFER_SIZE 4  // point array size  increment
#define SBUFFER_SIZE 4  // station array size-increment

#ifdef __cplusplus
extern "C" {
#endif

JavaVM *  globalJVM = NULL;
// jobject   gContext;
jobject   gClassLoader;
jmethodID gFindClassMethod;
jclass    gDistoXCommClass; 
JNIEnv *  gEnv;


JNIEnv * getEnv( JavaVM * vm )
{
  JNIEnv * env;
  int status = vm->GetEnv( (void**)&env, JNI_VERSION_1_6 );
  if ( status < 0 ) {
    status = vm->AttachCurrentThread( &env, NULL );
    if ( status < 0 ) return NULL;
  }
  return env;
}

void checkException( JNIEnv * env )
{
  if ( env->ExceptionOccurred() ) {
    env->ExceptionDescribe();
    env->ExceptionClear();
  }
}

jclass findClass( JavaVM * vm, const char * name )
{
  JNIEnv * env = getEnv( vm );
  // LOGI("JNI find class method <%s> env %p vm %p", name, (void*)env, (void*)vm );
  jobject class_obj = env->CallObjectMethod( gClassLoader, gFindClassMethod, env->NewStringUTF( name ) );
  // LOGI("JNI class object %p", (void *)class_obj );
  return static_cast<jclass>( class_obj );
}

// JNIExport jobject JNICALL
// getContext( )
// {
//   return gContext;
// }

JNIEXPORT jint JNICALL
JNI_OnLoad( JavaVM * vm, void * reserved ) 
{
  // LOGI("JNI on load");
  globalJVM = vm;
  JNIEnv * env = getEnv( vm );
  gEnv = env;
  jclass tmp_class = env->FindClass( "com/Cave3Dgl/DistoXComm" );
  checkException( env );

  if ( tmp_class == NULL ) {
    LOGW("JNI on load could not find class String");
  } else {
    jclass obj_class = env->GetObjectClass( tmp_class );
    checkException( env );
    jmethodID midGetClassLoader = env->GetMethodID( obj_class, "getClassLoader", "()Ljava/lang/ClassLoader;" );
    checkException( env );

    gClassLoader = env->NewGlobalRef( env->CallObjectMethod( tmp_class, midGetClassLoader ) );
    checkException( env );

    jclass ldr_class = env->FindClass( "java/lang/ClassLoader" );
    checkException( env );
    gFindClassMethod = env->GetMethodID( ldr_class, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;" );
    checkException( env );
    // LOGI("JNI on load gClassLoader %p gFindClass %p", (void*)gClassLoader, (void*)gFindClassMethod );

    gDistoXCommClass = findClass( vm, "com/Cave3Dgl/DistoXComm" );
    checkException( env );
  }
  return JNI_VERSION_1_6;
}

  

JNIEXPORT void JNICALL
Java_com_Cave3Dgl_DistoXComm_addPoint( JNIEnv * env, jobject obj, jdouble e, jdouble n, jdouble z )
{
  if ( DistoXModel::instance() == NULL ) return;
  DistoXModel::instance()->addPoint( e, n, z );
}

#ifdef __cplusplus
}
#endif

// ================================================================

DistoXModel * 
DistoXModel::theModel = NULL;

DistoXModel::DistoXModel( android_app * state, JNIEnv * e, mainTask * task )
  : Model( "DistoXModel" )
  , mState( state )
  , mEnv( e )
  , mTask( task )
  , e0( 0 )  // origin station coords
  , n0( 0 )
  , z0( 0 )
  , e1( 0 )  // reference data
  , n1( 0 )
  , z1( 0 )
  , cnt( 0 )
  , mStationNumber( 0 )
  , k0( 0 )  // origin station index
{
  // JNIEnv * lenv;
  // int ret = state->activity->vm->GetEnv( (void**)&lenv, JNI_VERSION_1_6 );
  // if ( ret == JNI_EDETACHED ) {
  //   LOGI("get env DETACHED" );
  // } else if ( ret == JNI_EVERSION ) {
  //   LOGI("get env VERSION" );
  // } else if ( ret == JNI_OK ) {
  //   LOGI("get env OK" );
  // }
  // LOGI("DistoX Model cstr env %p %p", (void*)mEnv, (void *)lenv );
  isize = IBUFFER_SIZE; // number of splays
  lsize = LBUFFER_SIZE; // number of splays
  vsize = VBUFFER_SIZE;
  ssize = SBUFFER_SIZE;
  scale = 1.0f;

  therion_ns = 1; // one station
  therion_nl = 0;
  therion_nx = 0;
  vertex = (float *)malloc( 3 * vsize * sizeof(float) );
  lindex = (unsigned short *)malloc( 2 * lsize * sizeof(unsigned short) );
  sindex = (unsigned short *)malloc( 2 * isize * sizeof(unsigned short) );
  float fz = SIGN_X( ( float )z0 * scale );
  float fe = SIGN_Y( ( float )e0 * scale );
  float fn = SIGN_Z( ( float )n0 * scale ) + ZOFF;
  vertex[3*k0+0]  = fz;
  vertex[3*k0+1]  = fe;
  vertex[3*k0+2]  = fn; // origin

  station_name = (Station *)malloc( ssize * sizeof(Station) );
  station_name[k0].name = new char[2];
  strcpy( station_name[k0].name, STATION_0 );

  SetNLegs( therion_nl );
  SetNVertex( therion_ns );
  SetNLIndex( 2 * therion_nl );
  SetNSIndex( 2 * therion_nx );

  JNIEnv * env = getEnv( mState->activity->vm );
  // LOGI("mEnv %p env %p", (void*)mEnv, (void*)env );
  state->activity->vm->AttachCurrentThread( &env, NULL );

  // gContext = state->activity;

  // * This call does not find the class
  // * it searches it in DexPathlib[ "."] and native libs: /vemdor/lib, /system/lib
  // jclass jcl0 = mEnv->FindClass( "com/Cave3Dgl/DistoXComm" );
  // checkException( mEnv );
  // LOGI("class0 %p", (void*)jcl0 );

  jclass jcl = findClass( mState->activity->vm, "com/Cave3Dgl/DistoXComm" );
  // LOGI("class local %p global %p", (void*)jcl, (void*)gDistoXCommClass );

  // jclass jcl = gDistoXCommClass;

  if ( jcl == NULL ) {
    LOGW("ERROR DistoX Model could not find class DistoXComm");
  } else {
    jmethodID cstr = env->GetMethodID( jcl, "<init>", "(Landroid/content/Context;)V" );
    checkException( env );
    // LOGI("env %p vm %p cstr-mid %p", (void*)env, (void*)(state->activity->vm), (void*)cstr );
    if ( cstr == NULL ) {
      LOGW("ERROR DistoX Model could not find DistoXComm cstr");
    } else {
      // state->activity is ANativeActivity which is an Activity, thus a Context, the object is clazz
      mDistoXComm = env->NewObject( jcl, cstr, state->activity->clazz );
      checkException( env );
    }
  }
  theModel = this;

  // LOGI("DistoX Model cstr done" );
}

DistoXModel::~DistoXModel()
{
  free( vertex );
  free( lindex );
  free( sindex );
}

bool
DistoXModel::connect( const char * address )
{
  if ( mDistoXComm == NULL ) {
    LOGI("DistoX Model connect NULL DistoXComm" );
  } else {
    LOGI("DistoX Model connect" );
    JNIEnv * env = getEnv( mState->activity->vm );
    jstring jstr( env->NewStringUTF(address) );
    jclass jcl = env->GetObjectClass( mDistoXComm );

    jmethodID mid = env->GetMethodID( jcl, "connect", "(Ljava/lang/String;)Z" );
    checkException( env );
    if ( mid != NULL ) {
      jboolean ret = env->CallBooleanMethod( mDistoXComm, mid, jstr );
      checkException( env );
      return ret;
    }
  }
  return false;
}

bool
DistoXModel::disconnect( )
{
  if ( mDistoXComm == NULL ) {
    LOGI("DistoX Model disconnect NULL DistoXComm" );
  } else {
    LOGI("DistoX Model disconnect" );
    JNIEnv * env = getEnv( mState->activity->vm );
    jclass jcl = env->GetObjectClass( mDistoXComm );

    jmethodID mid = env->GetMethodID( jcl, "disconnect", "()Z" );
    checkException( env );
    if ( mid != NULL ) {
      jboolean ret = env->CallBooleanMethod( mDistoXComm, mid );
      checkException( env );
      return ret;
    }
  }
  return false;
}

#define EPS 0.2

void
DistoXModel::addPoint( double e, double n, double z )
{
  // LOGI("delta %.2f %.2f %.2f cnt %d", e-e1, n-n1, z-z1, cnt );
  if ( fabs(e-e1) < EPS && fabs(n-n1) < EPS && fabs(z-z1) < EPS ) {
    if ( ++cnt >= 3 ) {
      if ( cnt == 3 ) {
        char name[8];
        ++ mStationNumber;
        int n = mStationNumber;
        int div = 1000;
        int c = 0;
        while ( div > 0 ) {
          c = n / div;
          n %= div;
          if ( c > 0 ) break;
          div /= 10;
        }
        int k = 0;
        for ( ; ; ) {
          name[k] = '0' + c;
          ++ k;
          div /= 10;
          if ( div == 0 ) break;
          c = n / div;
          n %= div;
        } 
        name[k] = 0;
        addStation( name, e1, n1, z1 );
      }
      return;
    }
  } else {
    cnt = 0;
    e1 = e;
    n1 = n;
    z1 = z;
  }

  bool update = false;
  nVertex = therion_ns + therion_nx;
  nSIndex = 2 * therion_nx;
  if ( nVertex >= vsize ) {
    vsize += VBUFFER_SIZE;
    vertex = (float *)realloc( vertex, sizeof(float)*3*vsize );
    SetVertex( vertex );       
    update = true;
  }
  if ( therion_nx >= isize ) {
    isize += IBUFFER_SIZE;
    sindex = (unsigned short *)realloc( sindex, 2 * isize * sizeof(unsigned short) );
    SetSIndex( sindex );
    update = true;
  }
  SetNVertex( nVertex );
  SetNSIndex( nSIndex );

  SetNPos(3);
  SetVertexStride( sizeof(float) * 3 );

  // x_offset = (0,0,0), scale also accounted for
  float fz = SIGN_X( ( float )(z0+z) * scale );
  float fe = SIGN_Y( ( float )(e0+e) * scale );
  float fn = SIGN_Z( ( float )(n0+n) * scale ) + ZOFF;
  unsigned short k = (unsigned short)(therion_nx + therion_ns);
  vertex[ 3*k + 0 ] = fz; 
  vertex[ 3*k + 1 ] = fe;
  sindex[ 2*therion_nx + 0 ] = k0;  // therion_nl = 0
  sindex[ 2*therion_nx + 1 ] =  k;  // therion_ns = 1
  vertex[ 3*k + 2 ] = fn;
  // LOGI("DistoX Model add point %d: v[%d] %.2f %.2f %.2f", therion_nx, k, fz, fe, fn );

  // update geometry bounds (bbox)
  // if ( fz < xmin ) { xmin = fz; } else if ( fz > xmax ) { xmax = fz; }
  // if ( fe < ymin ) { ymin = fe; } else if ( fe > ymax ) { ymax = fe; }
  // if ( fn < xmin ) { zmin = fn; } else if ( fn > zmax ) { zmax = fn; }
  ++ therion_nx;

  if ( update ) {
    InitBBox( false );
    NotifyRenderer( false );
  }
}

void
DistoXModel::addStation( const char * name, double e, double n, double z )
{
  bool update = false;
  nVertex = therion_ns + therion_nx;
  nLIndex = 2 * therion_nl;
  if ( therion_ns >= ssize ) {
    ssize += SBUFFER_SIZE;
    station_name = ( Station *) realloc( station_name, ssize * sizeof(Station) );
    // SetStations( station_name );
    update = true;
  }
  if ( nVertex >= vsize ) {
    vsize += VBUFFER_SIZE;
    vertex = (float *)realloc( vertex, sizeof(float)*3*vsize );
    SetVertex( vertex );       
    update = true;
  }
  if ( therion_nl >= lsize ) {
    lsize += LBUFFER_SIZE;
    lindex = (unsigned short *)realloc( lindex, 2 * lsize * sizeof(unsigned short) );
    SetLIndex( lindex );
    update = true;
  }
  SetNVertex( nVertex );
  SetNSIndex( nSIndex );

  SetNPos(3);
  SetVertexStride( sizeof(float) * 3 );

  station_name[therion_ns].idx  = nVertex;
  station_name[therion_ns].name = new char[ strlen(name) + 1];
  strcpy( station_name[therion_ns].name, name );

  z0 += z;
  e0 += e;
  n0 += n;
  // x_offset = (0,0,0), scale also accounted for
  float fz = SIGN_X( ( float )z0 * scale );
  float fe = SIGN_Y( ( float )e0 * scale );
  float fn = SIGN_Z( ( float )n0 * scale ) + ZOFF;
  unsigned short k = (unsigned short)(therion_nx + therion_ns);
  vertex[ 3*k + 0 ] = fz; 
  vertex[ 3*k + 1 ] = fe;
  vertex[ 3*k + 2 ] = fn;
  lindex[ 2*therion_nl + 0 ] = k0;  
  lindex[ 2*therion_nl + 1 ] =  k; 
  k0 = k; // index of last station
  LOGI("DistoX Model add station point %d: v[%d] %.2f %.2f %.2f", therion_nx, k, fz, fe, fn );

  // update geometry bounds (bbox)
  // if ( fz < xmin ) { xmin = fz; } else if ( fz > xmax ) { xmax = fz; }
  // if ( fe < ymin ) { ymin = fe; } else if ( fe > ymax ) { ymax = fe; }
  // if ( fn < xmin ) { zmin = fn; } else if ( fn > zmax ) { zmax = fn; }
  ++ therion_ns;
  ++ therion_nl;

  if ( update ) {
    InitBBox( false );
    NotifyRenderer( true );
  }
}



void 
DistoXModel::NotifyRenderer( bool with_stations ) 
{
  LOGI("DistoX Model notify st %d nx %d", therion_ns, therion_nx );
  mTask->NotifiedModel( with_stations );
}

