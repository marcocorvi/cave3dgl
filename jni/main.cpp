#include <jni.h>
#include <android_native_app_glue.h>
#include "mainApp.h"
#include <unistd.h>
#include "Log.h"

void android_main(struct android_app* state)
{
  // sleep(5);
  
  app_dummy();
  
  mainApp app(state);
  if (app.Initialize())
  {
    app.Run();
  }
  // std::terminate();
  // LOGI("main exit ...");
  Renderer::ReleaseInstance();
  ANativeActivity_finish( state->activity );
}
