#include "Kernel.h"

#include <unistd.h>

#include "Log.h"

void 
Kernel::Execute()
{
  // LOGI( "Kernel::Execute() tasks %d paused %d", tasks.size(), paused.size() );
  while ( tasks.size() > 0 ) {
    if ( Android::IsClosing() ) {
      // LOGI("Kernel exec: Android is closing");
      KillAllTasks();
    }
    TaskListIterator it;
    for ( it = tasks.begin(); it != tasks.end(); ) {
      Task * t = * it;
      if ( t->CanKill() ) {
        t->Stop();
        // LOGI("Kernel drop task %s", t->Name() );
        it = tasks.erase( it );
        // t = 0;
      } else {
        t->Update();
        ++it;
      }
    }
    // sleep( 1 ); // slow down execution
  }
  Android::ClearClosing();
}


// ------------------------------------------------------------
// maintenance

bool 
Kernel::AddTask( Task * t )
{ 
  bool s = t->Start();
  if ( s ) PriorityAdd( t );
  // LOGI( "Kernel::AddTask %s %s ", t->Name(), (s ? "ok" : "fail" ) );
  return s;
}

void 
Kernel::RemoveTask( Task * t )
{
  // LOGI( "Kernel::Remove Task %s ", t->Name() );
  if ( std::find( tasks.begin(), tasks.end(), t ) != tasks.end() ) {
    t->SetCanKill( true );
  }
}

void 
Kernel::SuspendTask( Task * t )
{
  // LOGI( "#### Kernel::Suspend Task %s ", t->Name() );
  if ( std::find( tasks.begin(), tasks.end(), t ) != tasks.end() ) {
    t->OnSuspend();
    tasks.remove( t );
    paused.push_back( t );
  }
}

void 
Kernel::ResumeTask( Task * t )
{
  // LOGI( "Kernel::Resume Task %s ", t->Name() );
  if ( std::find( paused.begin(), paused.end(), t ) != paused.end() ) {
    t->OnResume();
    paused.remove( t );
    PriorityAdd( t );
  }
}

void 
Kernel::KillAllTasks() 
{
  // LOGI( "Kernel::Kill All Tasks" );
  for ( TaskListIterator it = tasks.begin(); it != tasks.end(); ++it ) {
    (*it)->SetCanKill( true );
  }
}

void
Kernel::PriorityAdd( Task * task )
{
  TaskListIterator it = tasks.begin();
  for ( ; it != tasks.end(); ++it ) {
    Task * t = * it;
    if ( t == task ) return;
    if ( t->Priority() > task->Priority() ) break;
  }
  tasks.insert( it, task ); // insert task before it
}

