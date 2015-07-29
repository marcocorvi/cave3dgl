

default: ndk apk install

ndk:
	ndk-build

apk:
	ant clean
	ant debug

install:
	adb install -r bin/OpenGL-debug.apk

clean:
	rm -f obj/local/armeabi/objs/Test/*.o

distclean:
	rm -rf obj
