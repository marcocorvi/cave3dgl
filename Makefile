
BUILD = debug

default: ndk apk install

ndk:
	ndk-build

apk:
	ant $(BUILD)

install:
	adb install -r bin/Cave3Dgl-$(BUILD).apk

clean:
	ant clean
	rm -f obj/local/armeabi/objs/Cave3Dgl/*.o
	rm -f obj/local/armeabi/objs/Cave3Dgl/*.o.d

distclean:
	rm -rf obj
