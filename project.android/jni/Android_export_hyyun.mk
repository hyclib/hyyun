LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    		:= hyyun_c
LOCAL_SRC_FILES 		:= libhyyun_c.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../..

include $(PREBUILT_SHARED_LIBRARY)
