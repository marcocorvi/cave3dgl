
#ifndef SPLAY_POINT_COMPONENT_H
#define SPLAY_POINT_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "Points.h"

class PointsComponent : public Component
                      , public EventHandler
{
  private:
    static const unsigned int s_id = 6;

    Points * points;

  public:
    static unsigned int GetId() { return s_id; }

    explicit PointsComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
    { }
    virtual ~PointsComponent() { }

    virtual void InitializeComponent();

    Points * GetPoints() { return points; }
    void SetPoints( Points * pts ) { points = pts; }

    virtual void HandleEvent( EventType * evt );

};
#endif
