#include "Timer.h"

void 
Timer::Update()
{
  TimeUnits time = NanoTime();
  const float MULT = 0.000000001f;
  frameDt = ( time - timeLastFrame ) * MULT;
  simDt   = frameDt * simMult;
  timeLastFrame = time;
}

