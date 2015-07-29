#ifndef INPUT_STATE_H
#define INPUT_STATE_H

#include "Singleton.h"
#include "Log.h"

enum State {
  STATE_NONE,
  STATE_ROTATE,
  STATE_MOVE,
};

class InputState : public Singleton<InputState>
{
  private:
    int np; // number of pointers
    State mode; 
    float xc, yc;
    float dc;
    float x0, y0, x1, y1; // pointer(s) position(s)

    float zoom, deltax, deltay, deltat, deltap;
   
  public:
    InputState()
      : np( 0 )
      , mode( STATE_NONE )
      , dc( 1.0f )
      , zoom( 1.0f )
      , deltax( 0.0f )
      , deltay( 0.0f )
      , deltat( 0.0f )
      , deltap( 0.0f )
    { }

    // bool SetMode( int n )
    // {
    //   if ( np == n ) return false;
    //   np = n;
    //   // switch ( np ) {
    //   //   case 1: mode = STATE_ROTATE; break;
    //   //   case 2: mode = STATE_MOVE;   break;
    //   //   default: mode = STATE_NONE;
    //   // }
    //   return true;
    // }

    State Mode() const { return mode; }

       

    void InitMotionAndZoom( int n, float xx0, float yy0, float xx1, float yy1 )
    {
      np = n;
      mode = STATE_NONE;
      xc = (xx0 + xx1)/2;
      yc = (yy0 + yy1)/2;
      x0 = xx0;
      y0 = yy0;
      x1 = xx1;
      y1 = yy1;
      if ( np > 1 ) {
        dc = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
      } else {
        dc = 1.0;
      }
      deltax = 0.0f;
      deltay = 0.0f;
      deltat = 0.0f;
      deltap = 0.0f;
      // LOGI("State init (np %d) pos %.3f %.3f d %.3f", np, xc, yc, dc );
      if ( np == 1 ) {
        mode = STATE_ROTATE;
      } else if ( np == 2 ) {
        mode = STATE_MOVE;
      }
    }

    bool SetMotionAndZoom( int n, float xx0, float yy0, float xx1, float yy1 )
    {
      if ( n != np ) {
        InitMotionAndZoom( n, xx0, yy0, xx1, yy1 );
      } else {
        if ( np == 2 && mode == STATE_MOVE ) {
          float d0 = fabs( xx0 - x0 ) + fabs( yy0 - y0 ) + fabs( xx1 - x1 ) + fabs( yy1 - y1 );
          float d1 = fabs( xx0 - x1 ) + fabs( yy0 - y1 ) + fabs( xx1 - x0 ) + fabs( yy1 - y0 );
          if ( d1 < d0 ) {
            float t = xx0; xx0 = xx1; xx1 = xx0;
            t = yy0; yy0 = yy1; yy1 = yy0;
          }
          float xxc = (xx0 + xx1)/2;
          float yyc = (yy0 + yy1)/2;
          // translation
          deltax = - (yyc - yc)/20;  // N.B. Rotated by Z
          deltay =   (xxc - xc)/20;
          if ( fabs( deltax ) > 10 ) deltax = 0;
          if ( fabs( deltay ) > 10 ) deltay = 0;
          xc = xxc;
          yc = yyc;
          x0 = xx0;
          y0 = yy0;
          x1 = xx1;
          y1 = yy1;
          zoom = 1.0f;
          float dd = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
          if ( dd > 0.01f && fabs( 1.0f - dc/dd ) < 0.2f ) {
            if ( dc > 0.01f ) {
              // float z = 1.0f + (dd / dc - 1.0f) / 10.0f;
              float z = dc / dd;
              if ( z > 0.8f && z < 1.2f ) zoom = z;
            }
          } else {
            deltax = 0.0f;
            deltay = 0.0f;
          }
          dc = dd;
          deltap = 0.0f;
          deltat = 0.0f;
          // LOGI("State set (np %d) pos %.3f %.3f", np, xc, yc );
          return true;
        } else if ( np == 1 && mode == STATE_ROTATE ) {
          deltax = 0.0f;
          deltay = 0.0f;
          zoom = 1.0f;
          deltat = - xx0 + xc; // theta
          deltap =   yy0 - yc; // phi
          if ( fabs( deltap ) > 20 ) deltap = 0;
          if ( fabs( deltat ) > 20 ) deltat = 0;
          xc = xx0;
          yc = yy0;
          // LOGI("State set (np %d) pos %.3f %.3f", np, xc, yc );
          return true;
        } else {
          mode = STATE_NONE;
        }
      }
      return false;
    }

    float Zoom() const { return zoom; }
    float DeltaX() const { return deltax; }
    float DeltaY() const { return deltay; }
    float DeltaT() const { return deltat; }
    float DeltaP() const { return deltap; }

};
#endif
