#include "mainApp.h"

#include <string>

#include "Log.h"
#include "TaskPriority.h"
#include "Events.h"
#include "EventManagerFcts.h"

mainApp::mainApp(android_app* pState)
  : Application( pState )
  , state( pState )
  , theTask( NULL )
{
  // LOGI("mainApp cstr");
}

mainApp::~mainApp()
{
  // LOGI("mainApp dstr");
  if ( theTask ) {
    detachEvent( PAUSE_EVENT, *theTask );
    detachEvent( RESUME_EVENT, *theTask );
    delete theTask;
  }
  state->activity->vm->DetachCurrentThread( );
}

bool mainApp::Initialize()
{
  // std::string filename = "file:///storage/emulated/0/TopoDroid-draghi/th/draghi.th";
  std::string filename = "";

  state->activity->vm->AttachCurrentThread( &env, 0 );
  jobject me = state->activity->clazz;     // NativeActivity object handle
  jclass acl = env->GetObjectClass( me );  // class NativeActivity

  jmethodID jmid = env->GetMethodID( acl, "getApplicationContext", "()Landroid/content/Context;" );
  jobject context = env->CallObjectMethod( me, jmid );
  if ( context != NULL ) {
    LOGI("main app got context");
  }

  jmethodID giid = env->GetMethodID( acl, "getIntent", "()Landroid/content/Intent;" );
  jobject intent = env->CallObjectMethod( me, giid );
  if ( intent != NULL ) {
    jclass icl = env->GetObjectClass( intent );

    // jmethodID gseid = env->GetMethodID(icl, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    // jstring jsParamName = env->NewStringUTF("FILENAME");
    // jstring jsParam = (jstring)env->CallObjectMethod( intent, gseid, jsParamName );
    // const char * param = env->GetStringUTFChars( jsParam, 0 );
    // // when done
    // env->ReleaseStringUTFChars( jsParam, param ); 

    jmethodID gdid = env->GetMethodID( icl, "getData", "()Landroid/net/Uri;" );
    jobject uri = env->CallObjectMethod( intent, gdid );
    if ( uri != NULL ) {
      jclass ucl = env->GetObjectClass( uri );
      jmethodID guid = env->GetMethodID( ucl, "toString", "()Ljava/lang/String;" );
      jstring str = (jstring) env->CallObjectMethod( uri, guid );
      const char * path = env->GetStringUTFChars( str, 0 );
      // LOGI("mainApp::initialize() URI path %s", path );
      filename = path;
      env->ReleaseStringUTFChars( str, path );
    }
  }
  // LOGI("mainApp::initialize() %s", filename.c_str() );

  bool success = Application::Initialize(); // add tasks to kernel: Android, Timer, Renderer
  
  if (success) {
    // LOGI("application successfully initialized");
    theTask = new mainTask( MAIN_PRIORITY, filename.c_str() );
    attachEvent( PAUSE_EVENT, *theTask );
    attachEvent( RESUME_EVENT, *theTask );
    success &= kernel.AddTask( theTask );  // add my task to kernel
  } else {
    LOGW("WARNING application initialization failed");
  }

  return success;
}
