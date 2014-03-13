LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include E:/OpenCV-2.4.8-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := thinning
LOCAL_SRC_FILES := thinning.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
