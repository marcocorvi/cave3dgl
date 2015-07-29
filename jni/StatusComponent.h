#ifndef STATUS_COMPONENT_H
#define STATUS_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "Status.h"
// #include "StatusTexture.h"
#include "Vector4.h"
#include "Transform.h"
#include "Log.h"

class StatusComponent : public Component
                      , public EventHandler
{
  private:
    static const unsigned int s_id = 2;

    Status status;

  public:
    static unsigned int GetId() { return s_id; }

    explicit StatusComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
      , status( 40 )
    { 
      // LOGI("StatusComponent cstr");
    }

    virtual ~StatusComponent() { }

    virtual void InitializeComponent();

    Status * GetStatus() { return & status; }

    // void SetStatus( const char * msg ) { status.SetStatusString( msg ); }

    void HandleEvent( EventType * evt );

};

#endif
