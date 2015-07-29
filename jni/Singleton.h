#ifndef SINGLETON_H
#define SINGLETON_H

#include <assert.h>
#include <stdlib.h>

// template<typename T> class Singleton;
// 
// template<typename T>
// void release_instance( void )
// {
//   Singleton<T>::ReleaseInstance();
// }

template <typename T>
class Singleton
{
  private:
    static T*  ms_singleton;

  protected:
    Singleton()
    {
      assert( ms_singleton == NULL );
      ms_singleton = static_cast<T*>(this);
    }

  public:
    virtual ~Singleton()
    {
      assert(ms_singleton);
      ms_singleton = 0;
    }

    // static T& GetSingleton()    { return *ms_singleton; }
    // static T* GetSingletonPtr() { return ms_singleton; }
    static T * Instance()
    { 
      if ( ms_singleton == NULL ) {
        ms_singleton = new T();
        atexit( &Singleton<T>::ReleaseInstance );
      }
      return ms_singleton;
    }

    static void ReleaseInstance()
    {
      if ( ms_singleton != NULL ) delete ms_singleton;
      ms_singleton = NULL;
    }
};

template <typename T> T* Singleton<T>::ms_singleton = 0;

#endif // SINGLETON_H_
