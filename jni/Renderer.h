#ifndef RENDERER_H
#define RENDERER_H

#include <assert.h>
#include <math.h>
#include <vector>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

#include "Log.h"
#include "Task.h"
#include "TaskPriority.h"
#include "Singleton.h"
#include "EventHandler.h"
#include "EventManagerFcts.h"
#include "Events.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Plane.h"
#include "DrawFlags.h"

class Shader;
class Texture;
class Renderable;

enum FrustumIndex {
  NEAR = 0,
  FAR  = 1,
  RIGHT = 2,
  LEFT  = 3,
  TOP   = 4,
  BOTTOM = 5,
};

class Renderer : public Singleton<Renderer>
               , public Task
               , public EventHandler
{
  public:
    typedef std::vector< Shader * >             ShaderVector;
    typedef std::vector< Shader * >::iterator   ShaderVectorIterator;
    typedef std::vector< Texture * >            TextureVector;
    typedef std::vector< Texture * >::iterator  TextureVectorIterator;
    typedef std::vector< Renderable * >            RenderableVector;
    typedef std::vector< Renderable * >::iterator  RenderableVectorIterator;

  private:
    android_app * state;
    EGLDisplay    display;
    EGLContext    context;
    EGLSurface    surface;
    bool          DoPoints;
    int           width;
    int           height;
    float         aspect; // aspect-ratio H / W
    bool          initialized;
    // bool          paused;
    unsigned int  flag;
    float         lightTheta;
    float         lightPhi;
    Vector3       light;  // light direction

    int           lightNP; // internal, for update the light
    float         theta;
    float         phi;

    ShaderVector     shaders;
    TextureVector    textures;
    RenderableVector renderables;

    Matrix4          projMatrix;
    Plane            frustumPlanes[6];
    float            frustumParams[6];

  private:
    void Draw( Renderable * r );

    bool ShouldDraw( Renderable * r ) const;
    void BuildFrustum( float fov, float aspect, float near, float far );

  public:
    Renderer( )
      : Task( RENDER_PRIORITY, "renderer" )
      , EventHandler( "renderer" )
      , state( NULL )
      , display( EGL_NO_DISPLAY )
      , surface( EGL_NO_SURFACE )
      , context( EGL_NO_CONTEXT )
      , width( 1 )
      , height( 1 )
      , aspect( 1.0f )
      , initialized( false )
      // , paused( true )
      , flag( FLAG_STATIONS | FLAG_MENU )   // matching flags are NOT drawn
      , lightTheta( 45.0f )
      , lightPhi( 45.0f )
      , lightNP( -1 )
    { 
      // LOGI("Renderer cstr()");
      ComputeLight();
    }

    bool ToggleMenu() 
    { 
      flag ^= FLAG_MENU;
      // LOGI("Renderer Toggle Menu flag 0x%02x ", flag );
      return ( ( flag & FLAG_MENU ) == FLAG_MENU);
    }

    void ComputeLight()
    {
      light.m_x = (float)( cos( lightTheta * M_PI / 180.0f ) * cos( lightPhi * M_PI / 180.0f ) );
      light.m_y = (float)( cos( lightTheta * M_PI / 180.0f ) * sin( lightPhi * M_PI / 180.0f ) );
      light.m_z = (float)( sin( lightTheta * M_PI / 180.0f ) );
    }

    bool UpdateLight( int np, float t, float p );

    Vector3 & GetLight() { return light; }

    void ToggleStations() { flag ^= FLAG_STATIONS; }
    void ToggleSurface()  { flag ^= FLAG_SURFACE; }
    void ToggleTheSplays() { flag ^= FLAG_SPLAYS; }
    void ToggleThePoints() { flag ^= FLAG_POINTS; }
    void ToggleSplays()
    {
      if ( DoPoints ) {
        DoPoints = false;
        ToggleThePoints();
      } else {
        DoPoints = true;
        ToggleTheSplays();
      }
    }

  public:
    ~Renderer()
    {
      // LOGI("Renderer dstr()");
    }

    void InitRenderer( android_app * s );
    void DestroyRenderer();

    Matrix4 & Projection() { return projMatrix; }

    bool Initialized() const { return initialized; }
    int  Width() const  { return width; }
    int  Height() const { return height; }
    float Aspect() const { return aspect; }

    bool Start()
    { 
      // LOGI("Renderer::Start()" );
      attachEvent( PAUSE_EVENT, *this );
      attachEvent( RESUME_EVENT, *this );
      // attachEvent( MOTION_EVENT, *this ); // Renderer MOTION is called by Android
      return true;
    }
    void OnSuspend() { /* paused = true; */ }
    void OnResume()  { /* paused = false; */ }
    void Stop()
    { 
      // paused = true;
      detachEvent( PAUSE_EVENT, *this );
      detachEvent( RESUME_EVENT, *this );
    }
    void Update();

    void AddShader( Shader * s );
    void RemoveShader( Shader * s );
    void AddTexture( Texture * t );
    void RemoveTexture( Texture * t );
    void AddRenderable( Renderable * r );
    void RemoveRenderable( Renderable * r );

    void HandleEvent( EventType * evt );

  private:
    void LinkShaders();
};

#endif
