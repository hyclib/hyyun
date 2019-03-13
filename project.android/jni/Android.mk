LOCAL_PATH 		:= $(call my-dir)/../..
MY_HEADER_PATH	:= $(LOCAL_PATH)/../../../hy_bin/latest

include $(CLEAR_VARS)

LOCAL_MODULE    		:= hyyun_c
LOCAL_C_INCLUDES 		:= $(MY_HEADER_PATH)

LOCAL_SRC_FILES := \
source/hyyun_c/distributed/_0_yunDistributed_c.cpp \
source/hyyun_c/thread/clientHeartBeatThread.cpp \
source/hyyun_c/dllmain.cpp \
source/hyyun_c/hyyun_c.cpp \
source/hyyun_c/hyyunStatistics.cpp
 
LOCAL_SHARED_LIBRARIES  := libhyc libhydb

include $(BUILD_SHARED_LIBRARY)
include $(MY_HEADER_PATH)/hyc/android/Android_export.mk
include $(MY_HEADER_PATH)/hydb/android/Android_export.mk
