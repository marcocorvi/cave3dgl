#include "Renderer.h"

#include <string.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "Log.h"
#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderable.h"
#include "Events.h"
#include "InputState.h"
#include "Android.h"

void
Renderer::InitRenderer( android_app * s )
{
  // LOGI("Renderer::init() initialized %d", initialized );
  // paused = false;
  if ( initialized ) return;

  state = s;
  DoPoints = false;
  display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
  eglInitialize( display, NULL, NULL );
  // LOGI("Renderer::init() display OK ");

  const EGLint config_attrs[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
    EGL_BLUE_SIZE,       8,
    EGL_GREEN_SIZE,      8,
    EGL_RED_SIZE,        8,
    EGL_NONE
  };
  EGLint n_config;
  EGLConfig config;
  eglChooseConfig( display, config_attrs, &config, 1, &n_config );
  // LOGI("Renderer::init() config OK ");

  EGLint format;
  eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );
  // LOGI("Renderer::init() format %d", format );

  ANativeWindow_setBuffersGeometry( state->window, 0, 0, format );
  // LOGI("Renderer::init() window OK ");

  surface = eglCreateWindowSurface( display, config, state->window, NULL );
  // LOGI("Renderer::init() surface OK ");

  EGLint context_attrs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  context = eglCreateContext( display, config, NULL, context_attrs );
  // LOGI("Renderer::init() context OK ");

  if ( eglMakeCurrent( display, surface, surface, context ) == EGL_FALSE ) {
    LOGW("WARNING: unable to EGL make current");
    return;
  }
  eglQuerySurface( display, surface, EGL_WIDTH, &width );
  eglQuerySurface( display, surface, EGL_HEIGHT, &height );
  // aspect = ((float)height) / ((float)width);
  aspect = ((float)width) / ((float)height);
  // LOGI("Renderer::init() width %d height %d ", width, height );

  // glEnable( GL_CULL_FACE );
  glDisable( GL_CULL_FACE );
  glDisable( GL_DEPTH_TEST );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDepthMask( false );

  LinkShaders();

  BuildFrustum( 60.0f, aspect, 1.0f, 1000.0f );
  initialized = true;
}

void
Renderer::LinkShaders()
{
  // LOGI("Renderer::Link Shaders: shaders %d, textures %d, renderables %d",
  //   shaders.size(), textures.size(), renderables.size() );
  for ( ShaderVectorIterator it=shaders.begin(); it != shaders.end(); ++it ) {
    (*it)->LinkShader();
  }
  for ( TextureVectorIterator it=textures.begin(); it != textures.end(); ++it ) {
    (*it)->InitTexture();
  }
}

void 
Renderer::DestroyRenderer()
{
  if ( ! initialized ) return;
  // LOGI("Renderer::destroy() initialized %d", initialized );

  // paused = true;
  initialized = false;
  if ( display != EGL_NO_DISPLAY ) {
    eglMakeCurrent( display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    if ( context != EGL_NO_CONTEXT ) {
      eglDestroyContext( display, context );
    }
    if ( surface != EGL_NO_SURFACE ) {
      eglDestroySurface( display, surface );
    }
    eglTerminate( display );
  }
  display = EGL_NO_DISPLAY;
  surface = EGL_NO_SURFACE;
  context = EGL_NO_CONTEXT;
  initialized = false;
}

void
Renderer::Update()
{
  // if ( renderables.size() == 0 ) {
  //   LOGI("Renderer::update() init %d renderables %d ", initialized, renderables.size() );
  // }
  if ( ! ( initialized && Android::CanRender() ) ) {
    return;
  }
  glClearColor( 0.95f, 0.95f, 0.95f, 1 );
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
    
  for (RenderableVectorIterator it = renderables.begin(); it != renderables.end(); ++it) {
    Renderable * r = *it;
    if ( r ) {
      Draw( r );
    }
  }
  eglSwapBuffers( display, surface );
  renderables.clear();
}

void
Renderer::Draw( Renderable * r )
{
  if ( r == NULL ) return;
  Geometry * g = r->GetGeometry();
  Shader *   s = r->GetShader();
  // if ( strcmp( g->Name(), "string" ) == 0 && strcmp( s->Name(), "texture" ) == 0 ) {
  //   LOGI("Renderer::Draw renderable geom %s shader %s", g->Name(), s->Name() );
  // }
  if ( g == NULL ) {
    LOGW("Renderer::Draw renderable NULL geometry" );
    return;
  }
  if ( s == NULL ) {
    LOGW("Renderer::Draw renderable NULL shader" );
    return;
  }
  if ( g->TestFlag(flag)  ) return;

  s->SetupShader( *r );

  switch ( g->GetFlag() ) {
    case FLAG_STATIONS:
    case FLAG_STATUS:
    case FLAG_MENU:
      glDrawElements( g->GetType(), g->NIndex(), GL_UNSIGNED_SHORT, g->Index() );
      break;
    case FLAG_POINTS:
      glDrawElements( g->GetType(), g->NIndex(), GL_UNSIGNED_SHORT, g->Index() );
      // glDrawArrays( g->GetType(), 0, g->NIndex() );
      // {
      //   int nv = 3*g->NVertex();
      //   float * v = (float *)g->Vertex();
      //   glBegin( GL_POINTS );
      //   glPointSize( 10.0f );
      //   for ( int k=0; k<nv; k+=3 ) glVertex3f( v[k], v[k+1], v[k+2] );
      //   glEnd();
      // }
      break;
    case FLAG_SURFACE:
      glDrawElements( g->GetType(), g->NIndex(), GL_UNSIGNED_SHORT, g->Index() );
      break;
    case FLAG_LINE:
    case FLAG_NONE:
      int off = 2 * g->NLegs(); 
      glLineWidth( 4.0f );
      glDrawElements( g->GetType(), off, GL_UNSIGNED_SHORT, g->Index() );
      if ( (flag & FLAG_SPLAYS) != FLAG_SPLAYS ) {
        glLineWidth( 2.0f );
        void * splay = (void*)(((unsigned short *)g->Index())+off); // indexes of splays
        glDrawElements( g->GetType(), g->NIndex() - off, GL_UNSIGNED_SHORT, splay );
      }
      break;
  }

  // unsigned short * index = (unsigned short *)(g->Index() );
  // LOGI("Renderer Draw(): type %d Nindex %d index %d %d %d ...\n", 
  //   g->GetType(), g->NIndex(), index[0], index[1], index[2] );
}

void
Renderer::HandleEvent( EventType * evt )
{
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case PAUSE_EVENT:
      // paused = true;
      break;
    case RESUME_EVENT:
      // paused = false;
      break;
  }
}

void
Renderer::AddShader( Shader * s )
{ 
  if ( ! initialized || s == NULL ) return;
  for ( ShaderVectorIterator it=shaders.begin(); it != shaders.end(); ++it ) {
    if ( (*it) == s ) {
      LOGW("WARNING Renderer::Add Shader() %s already there", s->Name() );
      return;
    }
  }
  // LOGI("Renderer::Add Shader() %s adding", s->Name() );
  s->LinkShader();
  shaders.push_back( s );
}

void
Renderer::RemoveShader( Shader * s ) 
{
  // LOGI("Renderer::RemoveShader() %s", s->Name() );
  if ( s == NULL ) return;
  for ( ShaderVectorIterator it=shaders.begin(); it != shaders.end(); ++it ) {
    if ( (*it) == s ) {
      shaders.erase( it );
      break;
    }
  }
}

void
Renderer::AddTexture( Texture * t )
{ 
  // LOGI("Renderer::Add Texture() %d", t->Id() );
  if ( ! initialized || t == NULL ) return;
  t->InitTexture();
  textures.push_back( t );
}

void
Renderer::RemoveTexture( Texture * t ) 
{
  // LOGI("Renderer::Remove Texture() %d", t->Id() );
  if ( t == NULL ) return;
  for ( TextureVectorIterator it=textures.begin(); it != textures.end(); ++it ) {
    if ( (*it) == t ) {
      textures.erase( it );
      break;
    }
  }
}

void
Renderer::AddRenderable( Renderable * r )
{ 
  if ( ! initialized || r == NULL ) return;
  // LOGI("Renderer::Add Renderable() %s", (r==NULL)? "null" : r->GetGeometry()->Name() );

  for ( RenderableVectorIterator it=renderables.begin(); it != renderables.end(); ++it ) {
    if ( (*it) == r ) {
      // LOGI("Renderer::Add Renderable() renderable already present");
      return;
    }
  }
  // FIXME r->Initialize();
  renderables.push_back( r );
  // LOGI("Renderer::Add Renderable() renderable added: size %d", renderables.size() );
}

void
Renderer::RemoveRenderable( Renderable * r ) 
{
  // LOGI("Renderer::RemoveRenderable() %s", r->GetGeometry()->Name() );
  if ( r == NULL ) return;
  for ( RenderableVectorIterator it=renderables.begin(); it != renderables.end(); ++it ) {
    if ( (*it) == r ) {
      renderables.erase( it );
      break;
    }
  }
}

void 
Renderer::BuildFrustum( float fov, float aspect, float near, float far )
{
  float halfangle = 0.5f * fov * 3.1415926536f / 180.0f;
  frustumParams[ TOP ]    = (float)tan( halfangle );
  frustumParams[ BOTTOM ] = - frustumParams[ TOP ];
  frustumParams[ RIGHT ]  = aspect * frustumParams[ TOP ];
  frustumParams[ LEFT ]   = - frustumParams[ RIGHT ];
  frustumParams[ NEAR ]   = near;
  frustumParams[ FAR ]    = far;

  // projection matric: P*V*M 
  projMatrix.m_m[0] = 2.0f * frustumParams[NEAR]/(frustumParams[RIGHT] - frustumParams[LEFT]);
  projMatrix.m_m[1] = 0.0f;
  projMatrix.m_m[2] = -(frustumParams[RIGHT] + frustumParams[LEFT])/(frustumParams[RIGHT] - frustumParams[LEFT]);
  projMatrix.m_m[3] = 0.0f;

  projMatrix.m_m[4] = 0.0f;
  projMatrix.m_m[5] = 2.0f * frustumParams[NEAR]/(frustumParams[TOP] - frustumParams[BOTTOM]);
  projMatrix.m_m[6] = -(frustumParams[TOP] + frustumParams[BOTTOM])/(frustumParams[TOP] - frustumParams[BOTTOM]);
  projMatrix.m_m[7] = 0.0f;

  projMatrix.m_m[8]  = 0.0f;
  projMatrix.m_m[9]  = 0.0f;
  projMatrix.m_m[10] = (frustumParams[NEAR] + frustumParams[FAR]) / (frustumParams[FAR] - frustumParams[NEAR]);
  projMatrix.m_m[11] = -2.0f * frustumParams[NEAR] * frustumParams[FAR] / (frustumParams[FAR] - frustumParams[NEAR]);
  
  projMatrix.m_m[12] = 0.0f;
  projMatrix.m_m[13] = 0.0f;
  projMatrix.m_m[14] = 1.0f;
  projMatrix.m_m[15] = 0.0f;

  // Vector3 camX( cameraMatrix.m_m[0], cameraMatrix.m_m[1], cameraMatrix.m_m[2] ); // X right
  // Vector3 camY( cameraMatrix.m_m[4], cameraMatrix.m_m[5], cameraMatrix.m_m[6] ); // Y up
  // Vector3 camZ( cameraMatrix.m_m[8], cameraMatrix.m_m[9], cameraMatrix.m_m[10] ); // Z forward
  // Vector3 camPos( cameraMatrix.m_m[12], cameraMatrix.m_m[13], cameraMatrix.m_m[14] ); // position
  Vector3 camX( 1, 0, 0 );
  Vector3 camY( 0, 1, 0 );
  Vector3 camZ( 0, 0, 1 );
  Vector3 camPos( 0, 0, 0 );
  Vector3 centerN = camZ;
  centerN.Multiply( frustumParams[ NEAR ] );
  centerN.Add( camPos );
  Vector3 centerF = camZ;
  centerF.Multiply( frustumParams[ FAR ] );
  centerF.Add( camPos );

  Vector3 nTop = camY;
  nTop.Multiply( frustumParams[ TOP ] );
  nTop.Add( centerN );
  nTop.Subtract( camPos );
  nTop.Normalize();
  nTop = camX.Cross( nTop );
  frustumPlanes[ TOP ].BuildPlane( camPos, nTop );

  Vector3 nBot = camY;
  nBot.Multiply( frustumParams[ BOTTOM ] );
  nBot.Add( centerN );
  nBot.Subtract( camPos );
  nBot.Normalize();
  nBot = nBot.Cross( camX );
  frustumPlanes[ BOTTOM ].BuildPlane( camPos, nBot );

  Vector3 nRgt = camX;
  nRgt.Multiply( frustumParams[ RIGHT ] );
  nRgt.Add( centerN );
  nRgt.Subtract( camPos );
  nRgt.Normalize();
  nRgt = nRgt.Cross( camY );
  frustumPlanes[ RIGHT ].BuildPlane( camPos, nRgt );

  Vector3 nLft = camY;
  nLft.Multiply( frustumParams[ LEFT ] );
  nLft.Add( centerN );
  nLft.Subtract( camPos );
  nLft.Normalize();
  nLft = camY.Cross( nLft );
  frustumPlanes[ LEFT ].BuildPlane( camPos, nLft );

  Vector3 nNear = camZ;
  frustumPlanes[ NEAR ].BuildPlane( centerN, nNear );

  Vector3 nFar = camZ;
  nFar.Negate();
  frustumPlanes[ FAR ].BuildPlane( centerF, nFar );
}

bool
Renderer::ShouldDraw( Renderable * r ) const
{
  Vector3 min = r->GetBoundMin();
  Vector3 max = r->GetBoundMax();

  min.Add( r->GetTransformTranslation() );
  max.Add( r->GetTransformTranslation() );

  static const unsigned int N_PTS = 8;
  Vector3 pts[ N_PTS ];
  pts[0] = min;
  pts[1] = max;

  pts[2].m_x = min.m_x;
  pts[2].m_y = min.m_y;
  pts[2].m_z = max.m_z;

  pts[3].m_x = min.m_x;
  pts[3].m_y = max.m_y;
  pts[3].m_z = min.m_z;

  pts[4].m_x = max.m_x;
  pts[4].m_y = min.m_y;
  pts[4].m_z = min.m_z;

  pts[5].m_x = min.m_x;
  pts[5].m_y = max.m_y;
  pts[5].m_z = max.m_z;

  pts[6].m_x = max.m_x;
  pts[6].m_y = max.m_y;
  pts[6].m_z = min.m_z;

  pts[7].m_x = max.m_x;
  pts[7].m_y = min.m_y;
  pts[7].m_z = max.m_z;

  for ( unsigned int j=0; j<N_PTS; ++j ) {
    unsigned int i = 0; // nr planes in front
    for ( ; i<6; ++i ) {
      if ( ! frustumPlanes[i].IsInFront( pts[j] ) ) break;
    } 
    if ( i == 6 ) return true;
  }
  return false;
}


bool 
Renderer::UpdateLight( int np, float t, float p )
{
  bool ret = false;
  if ( np > 0 ) {
    if ( np == lightNP ) {
      // LOGI("T %.0f P %.0f   %d X %d", t, p, width, height );
      if ( p < height/5 ) {
        lightTheta += t - theta;
        if ( lightTheta > 90 ) lightTheta = 90;
        if ( lightTheta <  0 ) lightTheta =  0;
        ComputeLight();
        ret = true;
      } else if ( t > 4*width/5 ) {
        lightPhi += p - phi;
        if ( lightPhi > 360 ) lightPhi -= 360;
        if ( lightPhi <   0 ) lightPhi += 360;
        ComputeLight();
        ret = true;
      }
    }
    lightNP = np;
    theta = t;
    phi = p;
  }
  return ret;
}

