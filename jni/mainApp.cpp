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
  std::string filename = "";

  state->activity->vm->AttachCurrentThread( &env, 0 );
  jclass android_content_Context = env->GetObjectClass( state->activity->clazz );

  // jmethodID jmid = env->GetMethodID( android_content_Context, "getApplicationContext", "()Landroid/content/Context;" );
  // jobject context = env->CallObjectMethod( state->activity->clazzme, jmid );
  // if ( context != NULL ) {
  //   LOGI("main app: got context");
  // }

  // com.Cave3Dgl.DistoXComm is not visible here
  jmethodID midGetPackageName = env->GetMethodID( android_content_Context, "getPackageName", "()Ljava/lang/String;" );
  jstring packageName = (jstring)env->CallObjectMethod( state->activity->clazz, midGetPackageName );
  const char * nativePackageName = env->GetStringUTFChars( packageName, 0 );
  // LOGI("main app: Package <%s>", nativePackageName );


  jmethodID midGetIntent = env->GetMethodID( android_content_Context, "getIntent", "()Landroid/content/Intent;" );
  jobject intent = env->CallObjectMethod( state->activity->clazz, midGetIntent );
  if ( intent != NULL ) {
    // LOGI("main app: got intent");
    jclass icl = env->GetObjectClass( intent );

    // jmethodID gseid = env->GetMethodID(icl, "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;");
    // jstring jsParamName = env->NewStringUTF("FILENAME");
    // jstring jsParam = (jstring)env->CallObjectMethod( intent, gseid, jsParamName );
    // const char * param = env->GetStringUTFChars( jsParam, 0 );
    // // when done
    // env->ReleaseStringUTFChars( jsParam, param ); 

    jmethodID gdid = env->GetMethodID( icl, "getData", "()Landroid/net/Uri;" );
    jobject data = env->CallObjectMethod( intent, gdid );
    if ( data != NULL ) {
      jclass data_class = env->GetObjectClass( data );
      jmethodID midToString = env->GetMethodID( data_class, "toString", "()Ljava/lang/String;" );
      jstring str = (jstring) env->CallObjectMethod( data, midToString );
      const char * path = env->GetStringUTFChars( str, 0 );
      LOGI("main app::initialize() URI path <%s>", path );
      filename = path;
      env->ReleaseStringUTFChars( str, path );
    } else {
      LOGI("main app: no uri");
    }
  }
  // LOGI("main app::initialize() %s", filename.c_str() );

  bool success = Application::Initialize(); // add tasks to kernel: Android, Timer, Renderer
  
  if (success) {
    // LOGI("main app: successfully initialized");
    theTask = new mainTask( MAIN_PRIORITY, state, env, filename.c_str() );
    attachEvent( PAUSE_EVENT,  *theTask );
    attachEvent( RESUME_EVENT, *theTask );
    success &= kernel.AddTask( theTask );  // add my task to kernel
    // LOGI("main app: got main task");
  } else {
    LOGW("ERROR main app: initialization failed");
  }

  return success;
}
