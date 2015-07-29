#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "Application.h"
#include "Kernel.h"
#include "mainTask.h"
#include <android_native_app_glue.h>

class mainApp : public Application
{
  private:
    JNIEnv * env;
    android_app * state;
    mainTask * theTask;

  public:
    mainApp( android_app * pState );
    virtual ~mainApp();

    virtual bool Initialize();
};

#endif // MAIN_APP_H
