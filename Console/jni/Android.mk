LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_LIB_TYPE:=STATIC
include $(OPENCV_ANDROID_HOME)/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := thinning
LOCAL_SRC_FILES := thinning.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_EXECUTABLE)
