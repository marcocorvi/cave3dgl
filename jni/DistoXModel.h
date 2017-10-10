#ifndef DISTOX_MODEL_H
#define DISTOX_MODEL_H

#include <android_native_app_glue.h>
#include <string>
#include <map>
#include <jni.h>

#include "Model.h"

class mainTask;


class DistoXModel : public Model
{
  private: 
    static DistoXModel * theModel;
    mainTask * mTask;
    double e0, n0, z0; // last station coordinates


  public:
    static DistoXModel * instance() { return theModel; }
      
  private:
    android_app * mState;
    JNIEnv * mEnv;
    jobject  mDistoXComm;

    int isize;
    int lsize;
    int vsize;
    int ssize;
    unsigned short k0;

  public:
    DistoXModel( android_app * state, JNIEnv * e, mainTask * task );

    ~DistoXModel();

    bool connect( const char * address );

    bool disconnect();

    /** add splay point
     */
    void addPoint( double e, double n, double z );

    /** add leg and station point
     */
    void addStation( const char * name, double e, double n, double z );

  private:
    // @param with_stations  rebuild model stations 
    void NotifyRenderer( bool with_stations );
};

#endif
