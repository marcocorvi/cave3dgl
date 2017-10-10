#ifndef MAIN_TASK_H_
#define MAIN_TASK_H_

#include <stdlib.h>
#include <string.h>

#include "Task.h"
#include "Geometry.h"
#include "Texture.h"
// #include "StatusTexture.h"
#include "Renderable.h"
#include "TransformShader.h"
#include "PointShader.h"
#include "SurfaceShader.h"
#include "File.h"
#include "Object.h"
#include "Vector4.h"
#include "Transform.h"
#include "EventHandler.h"
#include "Status.h"
#include "Stations.h"
#include "SplayPoints.h"
#include "Surface.h"
#include "Menu.h"
#include "DistoXModel.h"
#include <android_native_app_glue.h>

class mainTask : public Task
               , public EventHandler
{
  private:
    JNIEnv * env;
    android_app * state;
    DistoXModel * distoXModel;   // non-null if displaying from DistoX
    bool          distoXConnected;

    Geometry        * geometry;        // 3D model (Model) [owned]
    TransformShader * transformShader; // This is the TransformShader of the model
    PointShader     * pointShader; 
    SurfaceShader   * surfaceShader; 
    std::string filepath;
    Vector4     color;
    // Transform   transform;
  
    Object        modelObject;
    Stations *    stations;      // owned
    Surface *     surface;       // owned
    SplayPoints * splayPoints;   // owned
    Menu *        menu;          // not owned
    float scale;
  
  public:
    mainTask( const unsigned int priority, android_app * st, JNIEnv * en, const char * filename = NULL );
    virtual ~mainTask();
  
    // From Task
    virtual bool  Start();
    virtual void  OnSuspend();
    virtual void  Update();
    virtual void  OnResume();
    virtual void  Stop();

    void AttachEventHandlers();
    void DetachEventHandlers();

    void HandleEvent( EventType * evt );

    void NotifiedModel( bool with_stations );

  private:
    const Matrix4 * GetMVP() { return ( transformShader != NULL )? transformShader->GetMVP() : NULL; }

    Geometry * AddRenderableToModel();
    Status * AddStatusToModel( );
    Menu * AddMenuToModel();
    void AddTransformToModel( Geometry * geom, Status * status );

    Stations * AddStationsToModel( Geometry * geo );

    Surface * AddSurfaceToModel( Geometry * geom );

    SplayPoints * AddSplayPointsToModel( Geometry * geom );

    void setupTransformShader( /* Geometry * geom */ );

};

#endif // MAIN_TASK_H_
