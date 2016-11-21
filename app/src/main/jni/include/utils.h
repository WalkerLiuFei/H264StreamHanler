//
// Created by Administrator on 2016/11/18 0018.
//

#ifndef MEDIAHANDLERLIB_UTILS_H
#define MEDIAHANDLERLIB_UTILS_H

#include <jni.h>
//jfieldID getMirrorDataFiledId(JNIEnv , jobject );
void set_ctx(JNIEnv *,jobject,void* );
jint get_ctx(JNIEnv *,jobject);
#endif //MEDIAHANDLERLIB_UTILS_H
