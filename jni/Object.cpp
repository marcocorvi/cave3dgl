#include "Object.h"

#include "Component.h"
#include "Log.h"

Object::~Object()
{
  LOGI("Object::dstr()");
  for ( ComponentMapIterator it = components.begin(); it != components.end(); ++it ) {
    Component * c = it->second;
    if ( c ) {
      delete c;
      it->second = NULL;
    }
  }
}

