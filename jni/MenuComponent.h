#ifndef MENU_COMPONENT_H
#define MENU_COMPONENT_H

#include "Component.h"
#include "EventHandler.h"
#include "EventType.h"
#include "Menu.h"
#include "Log.h"

class MenuComponent : public Component
                    , public EventHandler
{
  private:
    static const unsigned int s_id = 5;

    Menu menu;

  public:
    static unsigned int GetId() { return s_id; }

    explicit MenuComponent( Object * o, const char * n )
      : Component( o, n )
      , EventHandler( n )
      , menu( )
    { 
      // LOGI("MenuComponent cstr");
    }

    virtual ~MenuComponent() { }

    virtual void InitializeComponent() { }

    Menu * GetMenu() { return & menu; }

    void HandleEvent( EventType * evt );
};

#endif
