#ifndef LOG_H
#define LOG_H

#include <android/log.h>

#define TAG "CaveGL"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))

#endif
