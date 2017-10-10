#include "mainTask.h"

#include "Log.h"
#include "Events.h"
#include "TGAData.h"
// #include "MovementComponent.h"
// #include "PatrolComponent.h"
#include "TransformComponent.h"
#include "StatusComponent.h"
#include "StationsComponent.h"
#include "SurfaceComponent.h"
#include "MenuComponent.h"
#include "SurfaceShader.h"
#include "RenderableComponent.h"
#include "SplayPointComponent.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Transform.h"
#include "Geometry.h"
#include "Events.h"
#include "EventManagerFcts.h"

// #include "BasicShader.h"
// #include "SimpleShader.h"

// #include "TestModel.h"
// #include "SimpleModel.h"
#include "FileModel.h"
// #include "DistoXModel.h"
#include "Log.h"

mainTask::mainTask( const unsigned int priority, android_app * st, JNIEnv * en, const char * filename )
  : Task( priority, "main" )
  , EventHandler( "main" )
  , env( en )
  , state( st )
  , distoXModel( NULL )
  , distoXConnected( false )
  , filepath( filename ? filename : "" )
  , geometry( NULL )
  , transformShader( NULL )
  , pointShader( NULL )
  , surfaceShader( NULL )
  , stations( NULL )
  , splayPoints( NULL )
  , surface( NULL )
  , menu( NULL )
  , scale( 1.0f )
{
  LOGI( "mainTask cstr" );
}

void
mainTask::NotifiedModel( bool with_stations )
{
  // TODO update the geometry == distoXModel
  Model * model = dynamic_cast< Model * >( geometry );
  splayPoints->UpdateDataBuffers( model );

  if ( with_stations ) {
    AddStationsToModel( geometry );
  }

  float near = geometry->ZMin()-1; if ( near < 0 ) near = 0;
  float far  = geometry->ZMax()+1; if ( far  < 0 ) far  = 0;
  if ( far > near ) {
    Renderer::Instance()->RebuildFrustrum( near, far );
  }
}

// private
void
mainTask::setupTransformShader( /* Geometry * geom */ )
{
  // float dx = geom->XMax() - geom->XMin();
  // float dy = geom->YMax() - geom->YMin();
  // scale = ( 0.1f + (( dx > dy )? dx : dy ) );
  // LOGI( "main task::Add create transform shader");
  TransformShader * tmp = transformShader;
  transformShader = new TransformShader( /* scale */ );
  if ( tmp != NULL ) delete tmp;
}

// private
Geometry *
mainTask::AddRenderableToModel()
{
  LOGI( "main task::Add Renderable To Model() comps %d ", modelObject.GetNrComponents() );
  RenderableComponent * renderable_component = modelObject.AddComponent< RenderableComponent >( "renderable" );
  if ( renderable_component ) {
    Renderable & renderable = renderable_component->GetRenderable();

    // geometry = new SimpleModel();
    // TestModel * model = new TestModel();
    if ( filepath.length() > 0 ) {
      FileModel * file_model = new FileModel();
      file_model->Init( filepath.substr( 7 ).c_str() );
      geometry = file_model;
    } else {
      LOGI( "main task::Add create DistoX Model");
      distoXModel = new DistoXModel( state, env, this );
      distoXConnected = distoXModel->connect( "" ); // FIXME connect immediately or on OnResume
      geometry = distoXModel;
    }

    setupTransformShader( /* geometry */ );
    renderable.SetGeometry( geometry );
    renderable.SetShader( transformShader );

    Vector4 & color = renderable.GetAmbientColor();
    color.m_x = 0; // RGBA non-normalized
    color.m_y = 0;
    color.m_z = 0;
    color.m_w = 1.0f;
    // renderable.SetAmbientColor( color );

    // FIXME
    // Renderer::Instance()->AddRenderable( &renderable );
    LOGI("main task:done add model");

    return geometry;
  } else {
    LOGW( "main task::Add Renderable To Model() null renderable component" );
  }
  return NULL;
}

// private
SplayPoints *
mainTask::AddSplayPointsToModel( Geometry * geom )
{
  LOGI( "mainTask::Add Splay Points To Model() comps %d ", modelObject.GetNrComponents() );
  Model * model = dynamic_cast< Model * >( geom );
  if ( model == NULL ) {
    LOGW("WARNING NULL model");
    return NULL;
  }
  // LOGI( "mainTask add splays");
  SplayPointComponent * points_component = modelObject.AddComponent< SplayPointComponent >( "splay_points" );
  if ( points_component ) {
    pointShader = new PointShader( scale );
    splayPoints = new SplayPoints( model, pointShader );
    points_component->SetSplayPoints( splayPoints );
  }
  return splayPoints;
}

// private
Stations *
mainTask::AddStationsToModel( Geometry * geom )
{
  LOGI( "mainTask::Add Stations To Model() comps %d ", modelObject.GetNrComponents() );
  Model * model = dynamic_cast< Model * >( geom );
  if ( model == NULL ) {
    LOGW("WARNING not a model");
    return NULL;
  }
  int ns = model->GetNStations();
  const char ** name = model->GetStations();
  if ( name == NULL ) return NULL;

  if ( stations != NULL ) delete stations;
  stations = new Stations( ns, geom, name, GetMVP() );
  StationsComponent * stations_component = modelObject.AddComponent< StationsComponent >( "stations" );
  if ( stations_component ) {
    stations_component->SetStations( stations );
  }
  // LOGI("done add stations");
  return stations;
}

// private
Surface *
mainTask::AddSurfaceToModel( Geometry * geom )
{
  LOGI( "mainTask::Add Surface To Model() comps %d ", modelObject.GetNrComponents() );
  FileModel * model = dynamic_cast< FileModel * >( geom );
  if ( model == NULL ) {
    LOGW("WARNING NULL model");
    return NULL;
  }
  const float * s = model->GetDEM();
  if ( s == NULL ) {
    LOGW("WARNING NULL surface");
    return NULL;
  }
  SurfaceComponent * surface_component = modelObject.AddComponent< SurfaceComponent >( "surface" );
  if ( surface_component ) {
    // NOTE use the mainTask' shader
    surfaceShader = new SurfaceShader( );
    surface = new Surface( s, model->GetDEMCols(), model->GetDEMRows(), model->GetDEMStride(), surfaceShader );
    surface_component->SetSurface( surface );
  }
  // LOGI("done add surface");
  return surface;
}

// private
Status *
mainTask::AddStatusToModel()
{
  LOGI( "mainTask::Add Status To Model() comps %d ", modelObject.GetNrComponents() );
  Status * status = NULL;
  StatusComponent * status_component = modelObject.AddComponent< StatusComponent >( "status" );
  if ( status_component ) {
    status = status_component->GetStatus();
  }
  // LOGI("done add status");
  return status;
}

// private
Menu *
mainTask::AddMenuToModel()
{
  LOGI( "mainTask::Add Menu To Model() comps %d ", modelObject.GetNrComponents() );
  MenuComponent * menu_component = modelObject.AddComponent< MenuComponent >( "menu" );
  if ( menu_component ) {
    return menu_component->GetMenu();
  }
  return NULL;
}

// private
void
mainTask::AddTransformToModel( Geometry * geom, Status * status )
{
  LOGI( "mainTask::Add Transform To Model() comps %d geom %s", modelObject.GetNrComponents(), geom->Name() );
  TransformComponent * transform_component = modelObject.AddComponent<TransformComponent>( "transform" );
  if ( transform_component )
  {
    // if ( status == NULL ) LOGI("Add Transform: NULL status ");
    transform_component->SetStatus( status );
    Transform * transform = transform_component->GetComponentTransform();
    transform->TranslateCenter( geom->XCenter(), geom->YCenter(), geom->ZCenter() );
    // transform.RotateZ( true );
    // LOGI("mainTask trasnform component translate center %.0f %.0f %.0f",
    //   geom->XCenter(), geom->YCenter(), geom->ZCenter() );

  } else {
    LOGW( "mainTask::Add Tansform To Model() null transform component" );
  }
  // LOGI("done add transform");
}

void
mainTask::AttachEventHandlers()
{
  TransformComponent * transform_component = component_cast<TransformComponent>( modelObject );
  if ( transform_component ) attachEvent( MOTION_EVENT, *transform_component );

  StatusComponent * status_component = component_cast< StatusComponent >( modelObject );
  if ( status_component ) attachEvent( PRERENDER_EVENT, *status_component );

  MenuComponent * menu_component = component_cast< MenuComponent >( modelObject );
  if ( menu_component ) attachEvent( PRERENDER_EVENT, *menu_component );

  SplayPointComponent * splayPoint_component = component_cast< SplayPointComponent >( modelObject );
  if ( splayPoint_component ) attachEvent( RENDER_EVENT, *splayPoint_component );

  StationsComponent * station_component = component_cast< StationsComponent >( modelObject );
  if ( station_component ) {
    attachEvent( POSTMOTION_EVENT, *station_component );
    attachEvent( PRERENDER_EVENT, *station_component );
  }

  RenderableComponent * renderable_component = component_cast< RenderableComponent >( modelObject );
  if ( renderable_component ) attachEvent( RENDER_EVENT, *renderable_component );

  SurfaceComponent * surface_component = component_cast< SurfaceComponent >( modelObject );
  if ( surface_component ) attachEvent( RENDER_EVENT, *surface_component );

}

void
mainTask::DetachEventHandlers()
{
  SurfaceComponent * surface_component = component_cast< SurfaceComponent >( modelObject );
  if ( surface_component ) detachEvent( RENDER_EVENT, *surface_component );

  RenderableComponent * renderable_component = component_cast< RenderableComponent >( modelObject );
  if ( renderable_component ) detachEvent( RENDER_EVENT, *renderable_component );

  StationsComponent * station_component = component_cast< StationsComponent >( modelObject );
  if ( station_component ) {
    detachEvent( PRERENDER_EVENT, *station_component );
    detachEvent( POSTMOTION_EVENT, *station_component );
  }

  SplayPointComponent * splayPoint_component = component_cast< SplayPointComponent >( modelObject );
  if ( splayPoint_component ) detachEvent( RENDER_EVENT, *splayPoint_component );

  MenuComponent * menu_component = component_cast< MenuComponent >( modelObject );
  if ( menu_component ) detachEvent( PRERENDER_EVENT, *menu_component );

  StatusComponent * status_component = component_cast< StatusComponent >( modelObject );
  if ( status_component ) detachEvent( PRERENDER_EVENT, *status_component );

  TransformComponent * transform_component = component_cast<TransformComponent>( modelObject );
  if ( transform_component ) detachEvent( MOTION_EVENT, *transform_component );

}

void
mainTask::HandleEvent( EventType * evt )
{
  if ( evt == NULL ) return;
  switch ( evt->GetId() ) {
    case PAUSE_EVENT:
      // LOGI( "mainTask handle event PAUSE");
      DetachEventHandlers();
      if ( distoXModel ) distoXModel->disconnect();
      break;
    case RESUME_EVENT:
      // LOGI( "mainTask handle event RESUME");
      AttachEventHandlers();
      if ( distoXModel ) distoXModel->connect("");
      break;
  }
}

mainTask::~mainTask()
{
  // LOGI( "mainTask dstr" );
  DetachEventHandlers();
  // if ( transformShader ) {
  //   Renderer::Instance()->RemoveShader( transformShader );
  // }

  // RenderableComponent * renderable_component = component_cast< RenderableComponent >( modelObject );
  // if ( renderable_component ) {
  //   Renderable & renderable = renderable_component->GetRenderable();
  //   renderable.SetGeometry( NULL );
  //   renderable.SetShader( NULL );
  // }

  if ( transformShader   ) delete transformShader;
  if ( pointShader )       delete pointShader;
  if ( surfaceShader )     delete surfaceShader;

  if ( geometry ) delete geometry;
  if ( stations ) delete stations;
  if ( splayPoints ) delete splayPoints;
  if ( surface  ) delete surface;
}

bool
mainTask::Start()
{
  // LOGI( "main task::Start()" );
  // AddMovementToModel();

  Geometry * geom = AddRenderableToModel();
  // at this point mainTask has a pointer to the MVP matrix (inside the TransformShader)

  if ( distoXModel ) {
    AddSplayPointsToModel( geom );
    AddStationsToModel( geom );
  } else {
    AddSplayPointsToModel( geom );
    AddStationsToModel( geom );
    AddSurfaceToModel( geom );
  }
  Status * status = AddStatusToModel(); 
  menu = AddMenuToModel();
  AddTransformToModel( geom, status );
  AttachEventHandlers( );  // must come after the previous two

  Renderer::Instance()->SetMenu( menu );

  // IT IS NOT NECEESSARY TO ADD SHADERS TO THE RENDERER
  // if ( transformShader ) {
  //   Renderer::Instance()->AddShader( transformShader );
  // } else {
  //   LOGW("WARNING null transform shader");
  // }

  // LOGI( "main task::Start() done" );
  return true;
}


void mainTask::OnSuspend()
{
  LOGI( "main task::OnSuspend()" );
  if ( distoXModel != NULL && distoXConnected ) {
    distoXModel->disconnect( );
    distoXConnected = false;
  }
}

void mainTask::Update()
{
  // LOGI( "main task::update()" );
  sendEvent( UPDATE_EVENT );
  sendEvent( POSTUPDATE_EVENT );
  sendEvent( PRERENDER_EVENT );
  sendEvent( RENDER_EVENT );
}

void mainTask::OnResume()
{
  LOGI( "main task::OnResume()" );
  if ( distoXModel != NULL && ! distoXConnected ) {
    distoXModel->connect( "" );
  }
}

void mainTask::Stop()
{
  LOGI( "main task::Stop()" );
  DetachEventHandlers();
}
