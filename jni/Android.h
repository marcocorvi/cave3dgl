#ifndef ANDROID_H
#define ANDROID_H

#include <android_native_app_glue.h>

#include "Task.h"
#include "Log.h"

static void android_handle_cmd( struct android_app * s, int32_t cmd );
static int32_t android_handle_input_event( struct android_app * s, int32_t evt );

class Android : public Task
{
  private:
    static bool closing;
    static bool paused; // is paused ?
    static bool focus;  // has focus ?
    static bool render; // has surface ?

    android_app * state;

  public:
    Android( android_app * s, unsigned int p );

    virtual ~Android()
    { }

    android_app * GetState() { return state; }

    static void ClearClosing()      { closing = false; }
    static bool IsClosing()         { return closing; }
    static void SetPaused( bool p ) { paused = p; }
    static bool IsPaused()          { return paused; }
    static void ResetFlags();

    static void SetFocus( bool f )  { focus = f; }
    static void SetRender( bool r ) { render = r; }
    static bool CanRender() { return (! paused) && focus && render; }

    virtual bool Start() 
    { 
      // LOGI("Android START");
      ResetFlags();
      return true;
    }
    virtual void OnSuspend()
    {
      // LOGI("Android SUSPEND");
    }
    virtual void OnResume() 
    { 
      // LOGI("Android RESUME");
      paused = false;
    }
    virtual void Stop() 
    {
      // LOGI("Android STOP");
      paused = true;
      render = false;
    }

    virtual void Update() 
    {
      // LOGI("Android::Update()");
      int evt;
      struct android_poll_source * src;
      int i = ALooper_pollAll( 0, 0, &evt, (void**)&src );
      if ( i >= 0 ) {
        if ( src ) src->process( state, src );
        if ( state->destroyRequested ) closing = true;
      }
    }
};

#endif
