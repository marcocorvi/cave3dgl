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
#include "Renderable.h"
#include "Renderer.h"
#include "Transform.h"
#include "Geometry.h"
#include "Events.h"
#include "EventManagerFcts.h"

#include "TransformShader.h"
// #include "BasicShader.h"
// #include "SimpleShader.h"

// #include "TestModel.h"
// #include "SimpleModel.h"
#include "TherionModel.h"

mainTask::mainTask(const unsigned int priority, const char * filename )
  : Task( priority, "main" )
  , EventHandler( "main" )
  , filepath( filename ? filename : "" )
  , geometry( NULL )
  , shader( NULL )
  , stations( NULL )
  , surface( NULL )
  , scale( 1.0f )
{
  // LOGI( "mainTask cstr" );
}

Geometry *
mainTask::AddRenderableToModel()
{
  // LOGI( "mainTask::Add Renderable To Model() comps %d ", modelObject.GetNrComponents() );
  RenderableComponent * renderable_component = modelObject.AddComponent< RenderableComponent >( "renderable" );
  if ( renderable_component ) {
    Renderable & renderable = renderable_component->GetRenderable();

    // geometry = new SimpleModel();
    // TestModel * model = new TestModel();
    TherionModel * model = new TherionModel();
    if ( filepath.length() > 0 ) {
      model->Init( filepath.substr( 7 ).c_str() );
    }

    geometry = model;

    float dx = geometry->XMax() - geometry->XMin();
    float dy = geometry->YMax() - geometry->YMin();
    scale = ( 0.1f + (( dx > dy )? dx : dy ) );

    shader = new TransformShader( scale );

    Vector4 & color = renderable.GetAmbientColor();
    color.m_x = 0; // RGBA non-normalized
    color.m_y = 0;
    color.m_z = 0;
    color.m_w = 1.0f;
    // renderable.SetAmbientColor( color );

    renderable.SetGeometry( geometry );
    renderable.SetShader( shader );

    // FIXME
    // Renderer::Instance()->AddRenderable( &renderable );
    // LOGI("done add model");
    return geometry;
  } else {
    LOGW( "mainTask::Add Renderable To Model() null renderable component" );
  }
  return NULL;
}

Stations *
mainTask::AddStationsToModel( Geometry * geom )
{
  // LOGI( "mainTask::Add Stations To Model() comps %d ", modelObject.GetNrComponents() );
  TherionModel * model = dynamic_cast< TherionModel * >( geom );
  if ( model == NULL ) {
    LOGI("WARNING NULL model");
    return NULL;
  }
  const char ** name = model->GetStations();
  if ( name == NULL ) return NULL;
  int ns = model->GetNStations();

  stations = new Stations( ns, geom, name, GetMVP() );
  StationsComponent * stations_component = modelObject.AddComponent< StationsComponent >( "stations" );
  if ( stations_component ) {
    stations_component->SetStations( stations );
  }
  // LOGI("done add stations");
  return stations;
}

Surface *
mainTask::AddSurfaceToModel( Geometry * geom )
{
  // LOGI( "mainTask::Add Surface To Model() comps %d ", modelObject.GetNrComponents() );
  TherionModel * model = dynamic_cast< TherionModel * >( geom );
  if ( model == NULL ) {
    LOGI("WARNING NULL model");
    return NULL;
  }
  const float * s = model->GetDEM();
  if ( s == NULL ) {
    LOGI("WARNING NULL surface");
    return NULL;
  }
  SurfaceComponent * surface_component = modelObject.AddComponent< SurfaceComponent >( "surface" );
  if ( surface_component ) {
    // NOTE use the mainTask' shader
    Shader * shader = new SurfaceShader( );
    surface = new Surface( s, model->GetDEMCols(), model->GetDEMRows(), model->GetDEMStride(), shader );
    surface_component->SetSurface( surface );
  }
  // LOGI("done add surface");
  return surface;
}

Status *
mainTask::AddStatusToModel()
{
  // LOGI( "mainTask::Add Status To Model() comps %d ", modelObject.GetNrComponents() );
  Status * status = NULL;
  StatusComponent * status_component = modelObject.AddComponent< StatusComponent >( "status" );
  if ( status_component ) {
    status = status_component->GetStatus();
  }
  // LOGI("done add status");
  return status;
}

Menu *
mainTask::AddMenuToModel()
{
  // LOGI( "mainTask::Add Menu To Model() comps %d ", modelObject.GetNrComponents() );
  Menu * menu = NULL;
  MenuComponent * menu_component = modelObject.AddComponent< MenuComponent >( "menu" );
  if ( menu_component ) {
    menu = menu_component->GetMenu();
  }
  // LOGI("done add menu");
  return menu;
}

void
mainTask::AddTransformToModel( Geometry * geom, Status * status )
{
  // LOGI( "mainTask::Add Transform To Model() comps %d geom %s", modelObject.GetNrComponents(), geom->Name() );
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
      break;
    case RESUME_EVENT:
      // LOGI( "mainTask handle event RESUME");
      AttachEventHandlers();
      break;
  }
}

mainTask::~mainTask()
{
  // LOGI( "mainTask dstr" );
  DetachEventHandlers();
  // if ( shader ) {
  //   Renderer::Instance()->RemoveShader( shader );
  // }

  // RenderableComponent * renderable_component = component_cast< RenderableComponent >( modelObject );
  // if ( renderable_component ) {
  //   Renderable & renderable = renderable_component->GetRenderable();
  //   renderable.SetGeometry( NULL );
  //   renderable.SetShader( NULL );
  // }

  if ( shader   ) delete shader;
  if ( geometry ) delete geometry;
  if ( stations ) delete stations;
  if ( surface  ) delete surface;
}

bool
mainTask::Start()
{
  // LOGI( "mainTask::Start()" );
  // AddMovementToModel();

  Geometry * geom = AddRenderableToModel();
  // at this point mainTask has a pointe to the MVP matrix (inside the TransformShader)

  AddStationsToModel( geom );

  AddSurfaceToModel( geom );

  Status * status = AddStatusToModel(); 

  Menu * menu = AddMenuToModel();

  AddTransformToModel( geom, status );

  AttachEventHandlers( );  // must come after the previous two

  if ( shader ) {
    Renderer::Instance()->AddShader( shader );
  } else {
    LOGI("WARNING null transform shader");
  }
  // LOGI( "mainTask::Start() done" );
  return true;
}

void mainTask::OnSuspend()
{
  // LOGI( "mainTask::OnSuspend()" );
}

void mainTask::Update()
{
  // LOGI( "mainTask::update()" );
  sendEvent( UPDATE_EVENT );
  sendEvent( POSTUPDATE_EVENT );
  sendEvent( PRERENDER_EVENT );
  sendEvent( RENDER_EVENT );
}

void mainTask::OnResume()
{
  // LOGI( "mainTask::OnResume()" );
}

void mainTask::Stop()
{
  // LOGI( "mainTask::Stop()" );
  // DetachEventHandlers();
}
