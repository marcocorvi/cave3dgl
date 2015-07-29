#ifndef TIMER_H
#define TIMER_H

#include <time.h> // clock_gettime
#include "Task.h"
#include "Singleton.h"

class Timer : public Task
            , public Singleton<Timer>
{
  public:
    typedef long long TimeUnits;

  private:
    TimeUnits NanoTime()
    {
      timespec now;
      int err = clock_gettime( CLOCK_MONOTONIC, &now );
      return now.tv_sec * 100000000L + now.tv_nsec;
    }

    TimeUnits timeLastFrame;
    float     frameDt;
    float     simDt;
    float     simMult;

  private:
    Timer( ) 
      : Task( TIMER_PRIORITY, "timer" ) 
    { }

  public:
    ~Timer() { }

    float GetTimeFrame() const { return frameDt; }
    float GetTimeSim() const   { return simDt; }
    float SetSimMult( float m ) { simMult = m; }

    bool Start() { timeLastFrame = NanoTime(); return true; }
    void OnSuspend() { }
    void OnResume() {timeLastFrame = NanoTime(); }
    void Stop() { }
    void Update();
};

#endif
