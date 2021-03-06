LOCAL_PATH := $(call my-dir)

PNG_PATH := /home/programs/android-sdk/samples/android-8/libpng_android

include $(CLEAR_VARS)

LOCAL_ARM_MODE   := arm
LOCAL_C_INCLUDES := $(LOCAL_PATH) 
LOCAL_MODULE     := Cave3Dgl

LOCAL_SRC_FILES  := \
Image_JPG.cpp \
Image_PNG.cpp \
Plane.cpp \
Transform.cpp \
Vector3.cpp \
Vector4.cpp \
Matrix3.cpp \
Matrix4.cpp \
Application.cpp \
Kernel.cpp \
Android.cpp \
CharMap.cpp \
Renderable.cpp \
Renderer.cpp \
Shader.cpp \
TransformShader.cpp \
TextureShader.cpp \
SurfaceShader.cpp \
PointShader.cpp \
UserFile.cpp \
Surface.cpp \
Texture.cpp \
StatusTexture.cpp \
StationTexture.cpp \
MenuTexture.cpp \
Object.cpp \
EventType.cpp \
EventManager.cpp \
RenderableComponent.cpp \
TransformComponent.cpp \
StatusComponent.cpp \
PointsComponent.cpp \
MenuComponent.cpp \
StationsComponent.cpp \
SurfaceComponent.cpp \
Geometry.cpp \
Model.cpp \
FileModel.cpp \
DistoXModel.cpp \
Points.cpp \
LoxFile.cpp \
LoxBitmap.cpp \
TherionFile.cpp \
TherionFlip.cpp \
TherionBitmap.cpp \
TherionStation.cpp \
TherionShot.cpp \
TherionSurvey.cpp \
TherionSurface.cpp \
mainTask.cpp \
mainApp.cpp \
main.cpp

UNUSED := \
Timer.cpp \
AssetFile.cpp \
BasicShader.cpp \
SimpleShader.cpp \
TestModel.cpp \
SimpleModel.cpp 

NO_CPP := \
Task 

OTHERS := \
Renderer.cpp \
Geometry.cpp \
Shaders/Shader.cpp \
Texture.cpp \
File.cpp \
TGAFile.cpp 

LOCAL_LDLIBS    	:= -llog -landroid -lEGL -lGLESv2 -lpng -ljpeg -lz

LOCAL_STATIC_LIBRARIES	:= android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

