//
// Created by Administrator on 2016/11/18 0018.
//

#include "utils.h"
#include "jni.h"
#include "libavcodec/avcodec.h"

#define MIRROR_DATA "mirrorData"


jfieldID getMirrorDataFiledId(JNIEnv *env, jobject jobject1){
    jclass cls = (*env)->GetObjectClass(env,jobject1);
    //最后一个参数的含义表示这个属性的类型为 int
    return (*env)->GetFieldID(env, cls, MIRROR_DATA, "I");
}
extern void set_ctx(JNIEnv *env,jobject jobject1,void *ctx){
    jfieldID fid = getMirrorDataFiledId(env, jobject1);
    (*env)->SetIntField(env,jobject1,fid,ctx);
}
extern jint get_ctx(JNIEnv *env,jobject jobject1){
    jfieldID  fid = getMirrorDataFiledId(env, jobject1);
    return (*env)->GetIntField(env,jobject1,fid);
}
