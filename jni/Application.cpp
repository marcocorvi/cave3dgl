#include "Application.h"

#include "Log.h"
#include "Events.h"
#include "EventManagerFcts.h"
#include "Renderer.h"
#include "TaskPriority.h"

bool 
Application::Initialize()
{
  if ( initialized ) {
    // LOGI("Application::Initialize() alredy initialized");
    return true;
  }
 
  registerEvent( UPDATE_EVENT );
  registerEvent( POSTUPDATE_EVENT );
  registerEvent( RENDER_EVENT );
  registerEvent( PRERENDER_EVENT );
  registerEvent( MOTION_EVENT );
  registerEvent( POSTMOTION_EVENT );
  registerEvent( RESUME_EVENT );
  registerEvent( PAUSE_EVENT );

  bool ret = true;
  ret = kernel.AddTask( &androidTask );
  // TODO ret &= m_kernel.AddTask(Timer::Instance());
  renderer = Renderer::Instance();
  if ( renderer ) {
    // renderer->InitRenderer( &androidTask, androidTask.GetState() ); // initialize later on INIT_WINDOW
    ret &= kernel.AddTask( renderer );
  }
  initialized = ret;
  return ret;
}

void 
Application::Run() 
{
  // LOGI("Application::Run()");
  kernel.Execute();
}

