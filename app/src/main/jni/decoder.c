//
// Created by Administrator on 2016/11/18 0018.
//
#include "decoder.h"
#include "walker_person_mediahandlerlib_video_h264_H264Decoder.h"
#include <android/log.h>
#include "include/libavformat/avformat.h"
#include "include/libavutil/log.h"
#include "include/libavcodec/avcodec.h"
#include "include/libswscale/swscale.h"
#include "util.h"
#include "include/utils.h"

#define  D(...)  __android_log_print(ANDROID_LOG_INFO, "h264decoder", __VA_ARGS__)
#define  E(...)  __android_log_print(ANDROID_LOG_ERROR, "h264decoder", __VA_ARGS__)

typedef struct DecoderContext {
    int color_format;
    struct AVCodec *codec;
    struct AVCodecContext *codec_ctx;
    struct AVFrame *src_frame;
    struct AVFrame *dst_frame;
    struct SwsContext *convert_ctx;
    int frame_ready;
    long timeBase;
    int frameIndex;
} DecoderContext;


int decodeToFrame(JNIEnv* , jobject pJobject, jbyteArray pArray);

jbyteArray getStreamData(JNIEnv *, jobject pJobject);

JNIEXPORT jboolean JNICALL Java_walker_person_mediahandlerlib_video_natives_H264Decoder_initDecoder
        (JNIEnv * env, jobject object){
    D("get into init method");
    DecoderContext *ctx = calloc(1, sizeof(DecoderContext));

    D("Creating native H264 decoder context");
    av_register_all();
    ctx->codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    ctx->codec_ctx = avcodec_alloc_context3(ctx->codec);
    ctx->src_frame = av_frame_alloc();
    ctx->dst_frame = av_frame_alloc();
    ctx->color_format = PIX_FMT_RGB565LE;
    int openRes = avcodec_open2(ctx->codec_ctx, ctx->codec, NULL);
    // D(openRes);
    //为这个对象的cdata设置数据
    set_ctx(env, object, ctx);
}
int decodeToFrame(JNIEnv *env, jobject pJobject, jbyteArray pArray) {
    DecoderContext *ctx = (DecoderContext *) get_ctx(env, pJobject);
    void *buf = NULL;
    if (pArray == NULL) {
        D("Received null buffer, sending empty packet to decoder");
        return NULL;
    }
    else {
        buf  = (*env)->GetByteArrayElements(env,pArray,0);
        if (buf == NULL) {
            D("Error getting direct buffer address");
            (*env)->ReleaseByteArrayElements(env,pArray,buf,0);
            return NULL;
        }
    }
    //init a AVPacket
    AVPacket packet = {0};
    av_init_packet(&packet);
    packet.data =  (uint8_t *) buf;
    packet.size = (*env)->GetArrayLength(env,pArray);

    int frameFinished = 0;
    int res = avcodec_decode_video2(ctx->codec_ctx, ctx->src_frame, &frameFinished, &packet);
    if (frameFinished){
        ctx->frame_ready = 1;
    }

    D("goted frame is : %d", frameFinished);
    (*env)->ReleaseByteArrayElements(env,pArray, buf, 0);
    return frameFinished;
}
/**
 * 获取到转码后的数据
 */

jbyteArray getStreamData(JNIEnv *env, jobject pJobject) {
    DecoderContext *ctx = (DecoderContext *) get_ctx(env, pJobject);
    int pic_buf_size = avpicture_get_size(ctx->color_format, ctx->codec_ctx->width,
                                          ctx->codec_ctx->height);
    jbyteArray out_buffer = (*env)->NewByteArray(env,pic_buf_size);
    void *out_buf = (*env)->GetByteArrayElements(env, out_buffer, 0); //将java 的byte array 映射到C中的指针
    if (out_buffer == NULL) {
        D("Error getting direct buffer address");
        return NULL;
    }


    if (ctx->color_format == PIX_FMT_YUV420P)
        memcpy(ctx->src_frame->data, out_buffer, pic_buf_size);
    else {
        if (ctx->convert_ctx == NULL) {
            D("pixfmt is d%", ctx->codec_ctx->pix_fmt);
            ctx->convert_ctx = sws_getContext(ctx->codec_ctx->width, ctx->codec_ctx->height,
                                              ctx->codec_ctx->pix_fmt,
                                              ctx->codec_ctx->width, ctx->codec_ctx->height,
                                              ctx->color_format, SWS_FAST_BILINEAR, NULL, NULL,
                                              NULL);
        }

        avpicture_fill((AVPicture *) ctx->dst_frame, (uint8_t *) out_buf, ctx->color_format,
                       ctx->codec_ctx->width,
                       ctx->codec_ctx->height);

        sws_scale(ctx->convert_ctx, (const uint8_t **) ctx->src_frame->data,
                  ctx->src_frame->linesize, 0, ctx->codec_ctx->height,
                  ctx->dst_frame->data, ctx->dst_frame->linesize);
    }

    ctx->frame_ready = 0;

    if (ctx->src_frame->pkt_pts == AV_NOPTS_VALUE) {
        D("No PTS was passed from avcodec_decode!");
    }
    return out_buffer;
}

JNIEXPORT jbyteArray JNICALL Java_walker_person_mediahandlerlib_video_natives_H264Decoder_consumeStream
        (JNIEnv *env, jobject object, jbyteArray array){
    jbyteArray outBuffer = NULL;
    if (!decodeToFrame(env,object,array)){
        return  NULL;
    }else{
        return getStreamData(env, object);
    }

    jclass objclass = (*env)->GetObjectClass(env, object);
    //最后一个参数为 callback 的形参？
    jmethodID mid = (*env)->GetMethodID(env, objclass, "callback", "([B)V" ); //"[B means byteArray in java , B represent jni signature of byteas"
    (*env)->CallVoidMethod(env,object,mid,outBuffer); //CallVoidMethod mean that the method return 's type is void
}


JNIEXPORT jboolean JNICALL Java_walker_person_mediahandlerlib_video_natives_H264Decoder_releaseDecoder
        (JNIEnv *env, jobject object){
    DecoderContext *ctx = (DecoderContext *) get_ctx(env, object);

    D("Destroying native H264 decoder context");

    avcodec_close(ctx->codec_ctx);
    avcodec_free_context(&(ctx->codec_ctx));
    av_free(ctx->src_frame);
    av_free(ctx->dst_frame);
    free(ctx);
}
