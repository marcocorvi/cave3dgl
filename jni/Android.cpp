
#include "Android.h"

#include "Log.h"
#include "Renderer.h"
#include "Events.h"
#include "EventManagerFcts.h"
#include "InputState.h"

bool onMenu = false; // whether to display menu

void android_handle_cmd( struct android_app * s, int32_t cmd )
{
  Renderer * r = NULL;
  switch ( cmd ) {
    case APP_CMD_INPUT_CHANGED:
      break;
    case APP_CMD_INIT_WINDOW:
      // LOGI("INIT WINDOW");
      r = Renderer::Instance();
      if ( r != NULL ) r->InitRenderer( s, 1.0f, 1000.0f ); // near, far Z-planes
      Android::SetRender( true );
      break;
    case APP_CMD_TERM_WINDOW:
      // LOGI("TERM WINDOW");
      Android::SetRender( false );
      r = Renderer::Instance();
      if ( r != NULL ) r->DestroyRenderer();
      break;
    case APP_CMD_WINDOW_RESIZED:
      // LOGI("WINDOW RESIZED");
      Android::SetRender( true );
      break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
      // LOGI("WINDOW REDRAW NEEDED");
      break;
    case APP_CMD_CONTENT_RECT_CHANGED:
      break;
    case APP_CMD_GAINED_FOCUS:
      // LOGI("GAIN FOCUS");
      Android::SetFocus( true );
      break;
    case APP_CMD_LOST_FOCUS:
      // LOGI("LOST FOCUS");
      Android::SetFocus( false );
      break;
    case APP_CMD_CONFIG_CHANGED:
      break;
    case APP_CMD_LOW_MEMORY:
      // LOGI("LOW MEMORY");
      break;
    case APP_CMD_START:
      // LOGI("START"); // cannot InitRenderer here
      Android::ClearClosing( );
      break;
    case APP_CMD_RESUME:
      // LOGI("RESUME");
      Android::SetPaused( false );
      sendEvent( RESUME_EVENT );
      break;
    case APP_CMD_SAVE_STATE:
      break;
    case APP_CMD_PAUSE:
      // LOGI("PAUSE");
      Android::SetPaused( true  );
      sendEvent( PAUSE_EVENT );
      break;
    case APP_CMD_STOP:
      // LOGI("STOP");
      // Android::SetClosing( true  );
      // r = Renderer::Instance();
      // if ( r != NULL ) r->DestroyRenderer();
      break;
    case APP_CMD_DESTROY:
      // LOGI("DESTROY");
      // Renderer::ReleaseInstance();
      ANativeActivity_finish( s->activity );
      break;
    default:
      LOGW("UNKNOWN CMD %d ", cmd );
  }
}

int32_t android_handle_input_event( struct android_app * s, AInputEvent * evt )
{
  int type = AInputEvent_getType( evt );
  Renderer * r = NULL;
  switch ( type ) {
    case AINPUT_EVENT_TYPE_KEY:
      {
        int32_t key = AKeyEvent_getKeyCode( evt );
        int action = 0xff & AKeyEvent_getAction( evt );
        if ( action == AKEY_EVENT_ACTION_DOWN ) {
          switch ( key ) {
            // case AKEYCODE_S: // toggle station display
            case AKEYCODE_MENU:
              // LOGI("key MENU");
              r = Renderer::Instance();
              if ( r != NULL ) onMenu = ! r->ToggleMenu();
              // if ( r != NULL ) r->ToggleStations();
              // LOGI("onMenu %s ", onMenu ? "true" : "false" );
              return 1;
            // case AKEYCODE_BACK:
            //   LOGI("key BACK");
            //   break;
            // case AKEYCODE_HOME:
            //   LOGI("key HOME");
            //   break;
            // case AKEYCODE_SEARCH:
            //   LOGI("key SEARCH");
            //   break;
            default:
              LOGW("unhandled key %d ", key );
          }
        }
      }
      break;
    case AINPUT_EVENT_TYPE_MOTION:
      {
        bool updated_light = false;
        int action = 0xff & AMotionEvent_getAction( evt );
        size_t np = AMotionEvent_getPointerCount( evt );
        // LOGI("android motion event action %d np %d", action, np );
        InputState * state = InputState::Instance();
        // state->SetMode( np );
        if ( onMenu ) {
          if ( np == 1 && action == AMOTION_EVENT_ACTION_DOWN ) {
            float x0 = AMotionEvent_getX( evt, 0 ) / (float)(Renderer::Instance()->Width());
            float y0 = AMotionEvent_getY( evt, 0 ) / (float)(Renderer::Instance()->Height());
            // LOGI(" x0 %.2f y0 %.2f", x0, y0 );
            if ( x0 > 0.9 ) {
              r = Renderer::Instance();
              if ( y0 < 0.95 && y0 > 0.80 ) {
                if ( r != NULL ) r->ToggleStations();
              } else if ( y0 < 0.65 && y0 > 0.50 ) {
                if ( r != NULL ) r->ToggleSplays();
              } else if ( y0 < 0.35 && y0 > 0.20 ) {
                if ( r != NULL ) r->ToggleSurface();
              }
            }
          }
        } else {
          if ( np <= 2 ) {
            float x0, y0, x1, y1;
            if ( np > 0 ) {
              x0 = AMotionEvent_getX( evt, 0 );
              y0 = AMotionEvent_getY( evt, 0 );
              x1 = (np > 1)? AMotionEvent_getX( evt, 1 ) : x0;
              y1 = (np > 1)? AMotionEvent_getY( evt, 1 ) : y0;
            } else {
              x0 = x1 = y0 = y1 = 0.0f;
            }
            switch ( action ) {
              case AMOTION_EVENT_ACTION_POINTER_DOWN: // 5
              case AMOTION_EVENT_ACTION_POINTER_UP:   // 6
              case AMOTION_EVENT_ACTION_DOWN:         // 0
              case AMOTION_EVENT_ACTION_UP:           // 1
              case AMOTION_EVENT_ACTION_MOVE:         // 2
                r = Renderer::Instance();
                if ( np <= 1 && r != NULL ) updated_light = r->UpdateLight( np, x0, y0 );
                if ( ! updated_light ) {
                  if ( state->SetMotionAndZoom( np, x0, y0, x1, y1 ) ) {
                    sendEvent( MOTION_EVENT );
                    sendEvent( POSTMOTION_EVENT );
                  }
                }
                return 1;
              default:
                LOGW("android motion event unhandled action %d", action );
            }
          }
        }
      }
      break;
    default:
      LOGW("android input event unhandled type %d ", type );
  }
  return 0; // not handled
}

bool Android::closing = false;
bool Android::paused  = true;
bool Android::focus   = false;
bool Android::render  = false;

void
Android::ResetFlags()
{
  closing = false;
  paused  = true;
  focus   = false;
  render  = false;
}

Android::Android( android_app * s, unsigned int p )
  : Task( p, "android" )
  , state( s )
{
  // LOGI("Android::cstr()");
  state->onAppCmd     = android_handle_cmd;
  state->onInputEvent = android_handle_input_event;
}

