
#ifndef SPLAY_POINT_COMPONENT_H
#define SPLAY_POINT_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "SplayPoints.h"

class SplayPointComponent : public Component
                          , public EventHandler
{
  private:
    static const unsigned int s_id = 6;

    SplayPoints * splayPoints;

  public:
    static unsigned int GetId() { return s_id; }

    explicit SplayPointComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
    { }
    virtual ~SplayPointComponent() { }

    virtual void InitializeComponent();

    SplayPoints * GetSplayPoints() { return splayPoints; }
    void SetSplayPoints( SplayPoints * s ) { splayPoints = s; }

    virtual void HandleEvent( EventType * evt );


};
#endif
