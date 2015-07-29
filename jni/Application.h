#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdlib.h>

#include "Kernel.h"
#include "Android.h"
#include "Log.h"
#include "TaskPriority.h"
#include "Renderer.h"
// #include "StatusTexture.h"

class Application
{
  protected:
    Kernel  kernel;
    Android androidTask;
    Renderer * renderer; // singleton
    bool initialized;

  public:
    Application( android_app * state )
      : androidTask( state, PLATFORM_PRIORITY )
      , renderer( NULL )
      , initialized( false )
    {
      // LOGI("Application::cstr");
      // CreateSingletons();
    }

    virtual ~Application()
    {
      // LOGI("Application::dstr");
  
      // DestroySingletons();
    }

    bool Initialize();
    void Run();

  private:
    // void CreateSingletons();
    // void DestroySingletons();

};

#endif

