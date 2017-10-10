#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

// #include <unordered_map>
#include <map>
#define MAP std::map

// #include "Component.h"
class Component;

class Object
{
  template<class T> friend T* component_cast( Object & obj );
  template<class T> friend T* component_cast( Object * obj );

  private:
    typedef MAP< unsigned int, Component * >           ComponentMap;
    typedef MAP< unsigned int, Component * >::iterator ComponentMapIterator;

    ComponentMap components;


    Component * GetComponent( unsigned int id )
    {
      ComponentMapIterator it = components.find( id );
      return ( it == components.end() )? NULL : it->second;
    }

  public:
    Object() { }
    ~Object();

    size_t GetNrComponents() const { return components.size(); }

    template<class T> T* GetComponent() { return static_cast<T*>( GetComponent( T::GetId() ) ); }

    /** return the component of class T
     *  ( create and add it if it did not exist )
     */
    template<class T> T * AddComponent( const char * name )
    {
       ComponentMapIterator it = components.find( T::GetId() );
       if ( it == components.end() ) {
         T * t = new T( this, name );
         if ( t ) {
           std::pair<unsigned int, Component*> comp( T::GetId(), t );
           std::pair< ComponentMapIterator, bool > res = components.insert( comp );
           if (res.second) {
             return t;
           } else {
             delete t;
           }
         }
       } else {
         return dynamic_cast<T*>(it->second);
       }
       return NULL;
     }


};

template<class T> 
T* component_cast( Object & obj )
{
  return obj.GetComponent<T>();
}

template<class T> 
T* component_cast( Object * obj )
{
  return ( obj != NULL )? obj->GetComponent<T>() : NULL;
}

#endif
