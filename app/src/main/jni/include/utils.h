//
// Created by Administrator on 2016/11/18 0018.
//

#ifndef MEDIAHANDLERLIB_UTILS_H
#define MEDIAHANDLERLIB_UTILS_H

#include <jni.h>
#include <android/log.h>
#include "libavcodec/avcodec.h"

#define  D(...)  __android_log_print(ANDROID_LOG_INFO, "h264decoder", __VA_ARGS__)
#define  E(...)  __android_log_print(ANDROID_LOG_ERROR, "h264decoder", __VA_ARGS__)

void set_ctx(JNIEnv *,jobject,void* );
jint get_ctx(JNIEnv *,jobject);
#endif //MEDIAHANDLERLIB_UTILS_H
