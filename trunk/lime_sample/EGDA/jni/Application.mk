#APP_ABI := armeabi
APP_ABI := armeabi-v7a
#APP_ABI := armeabi armeabi-v7a
APP_OPTIM := release

#APP_CPPFLAGS := -DLIME_GLES2 -Wall -Wno-multichar -Wcast-qual -Wcast-align -Wfloat-equal -Winit-self -Wpadded
APP_CPPFLAGS := -DLIME_GLES2 -DLIME_ENABLE_LOG -Wall -Wno-multichar -DLIME_EGDA_DISP_FPS

APP_C_INCLUDES := $(APP_PROJECT_PATH)../../ $(APP_PROJECT_PATH)../../ext/arm/include/ ../lib/anim/ ../tools/libConverter/
LIME_DIR := $(APP_PROJECT_PATH)../../..
LIBCONVERTER_DIR := ../../tools/libConverter
LIBANIM_DIR := ../../lib/anim
