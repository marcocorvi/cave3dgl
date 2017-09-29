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

class mainTask : public Task
               , public EventHandler
{
  private:
    Geometry  * geometry;        // 3D model (TherionModel) [owned]
    TransformShader * transformShader;    // This is the TransformShader of the model
    PointShader     * pointShader; 
    SurfaceShader   * surfaceShader; 
    std::string filepath;
    Vector4     color;
    // Transform   transform;
  
    Object        modelObject;
    Stations *    stations;      // owned
    Surface *     surface;       // owned
    SplayPoints * splayPoints;   // owned
    float scale;
  
  public:
    mainTask(const unsigned int priority, const char * filename = NULL );
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

  private:
    const Matrix4 * GetMVP() { return ( transformShader != NULL )? transformShader->GetMVP() : NULL; }

    Geometry * AddRenderableToModel();
    Status * AddStatusToModel( );
    Menu * AddMenuToModel();
    void AddTransformToModel( Geometry * geom, Status * status );

    Stations * AddStationsToModel( Geometry * geo );

    Surface * AddSurfaceToModel( Geometry * geom );

    SplayPoints * AddSplayPointsToModel( Geometry * geom );
};

#endif // MAIN_TASK_H_
