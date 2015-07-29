#ifndef KERNEL_H
#define KERNEL_H

#include <list>
#include "Task.h"
#include "Android.h"
#include "Log.h"

class Kernel
{
  private:
    typedef std::list< Task * > TaskList;
    typedef std::list< Task * >::iterator TaskListIterator;

    TaskList tasks;
    TaskList paused;

    void PriorityAdd( Task * task );

  public:
    Kernel()
    {
      LOGI("Kernel cstr");
    }

    virtual ~Kernel() 
    { 
      LOGI("Kernel dstr");
      KillAllTasks();
    }

    void Execute();

    bool AddTask( Task * t );
    void RemoveTask( Task * t );
    void SuspendTask( Task * t );
    void ResumeTask( Task * t );
    void KillAllTasks();

    bool HasTasks() const { return tasks.size() > 0; }

};

#endif
