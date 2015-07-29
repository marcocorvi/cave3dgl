# Cave3D with OpenGL ES 2.0

Cave3dgl is a Therion projects 3D visualizer written in C++ using the android native glue.

COMPILE and INSTALL:
  You need android sdk, android ndk, and ant. 

There is a Makefile:
  a default "make" should build the apk and install it to your Android
  (you should attach it, and start the adb server before you "make").


HOW TO USE THE APP:
  . Select a Therion project file with your favourite FileManager and open it with Cave3D-gl.

FEATURES:
  Zoom in/out.
  Rotation and tilt.
  Optional display of station, splays, surface.
  Moving the position of the light source.

TODO:
  Surface texture.
  Color by survey / altitude.
  Wall rendering.
  Loch file support.
