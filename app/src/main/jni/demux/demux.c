//
// Created by Administrator on 2016/11/22 0022.
//

#include "walker_person_mediahandlerlib_example_Demux.h"
#include "../include/libavformat/avformat.h"
#include "../include/utils.h"
#include "../include/libavutil/avutil.h"
#include "../include/libavcodec/avcodec.h"

typedef struct DemuxContext{
    AVFormatContext *formatContext;
    AVCodec *codec;
    AVCodecContext *codecContext;

}DemuxContext;



void extract_extradata(JNIEnv *env, jobject object,jbyteArray,AVCodecContext *);

JNIEXPORT jboolean JNICALL Java_walker_person_mediahandlerlib_example_Demux_initNativa
        (JNIEnv *env, jobject object){
    DemuxContext* context = calloc(1, sizeof(DemuxContext));
    set_ctx(env,object,context);
}



void extract_extradata(JNIEnv *env,jobject object,jbyteArray  jbyteArray1,AVCodecContext *codecContext) {
    //nt

}


JNIEXPORT void JNICALL Java_walker_person_mediahandlerlib_example_Demux_demux
        (JNIEnv *env, jobject object, jstring path){
    AVFormatContext *formatContext = avformat_alloc_context();
    uint8_t *extraData = NULL; //save sps pps
    int extra_data_length;
 //   int streamIndex = -1;
    char *src_filePath;
    int callBackMethodID;
    int getExtraDatacallBackMethodID;
    int getIDRDataCallBackMethodID;

    //一定要通过这种方式来打开获取从java层传来的路径值
    src_filePath = (*env)->GetStringUTFChars(env,path,0);
    D("%s",src_filePath);
 //   (*env)->GetByteArrayRegion(env,path,0,(*env)->GetArrayLength(env,path),src_filePath);
    av_register_all();
    //the format will be auto init
    if(avformat_open_input(&formatContext,src_filePath,NULL,NULL) < 0){
        E("open file failed");
        return;
    }
    //retrieve stream info
    if (avformat_find_stream_info(formatContext,NULL) < 0){
        E("could not find stream information");
        return ;
    }
   /* if (open_codec_context(&streamIndex,formatContext,AVMEDIA_TYPE_VIDEO) < 0){
        E("open codec failed!!");
        return NULL;
    }*/
    /* dump input information to stderr */

    av_dump_format(formatContext, 0, src_filePath, 0);
    AVPacket packet;
    av_init_packet(&packet);
    jclass  class =  (*env)->GetObjectClass(env,object);
    callBackMethodID = (*env)->GetMethodID(env,class,"callback","([BI)V");
    getIDRDataCallBackMethodID = (*env)->GetMethodID(env,class,"getIDRDataCallBack","([BI)V");
    jbyteArray jbyteArray1 = (*env)->NewByteArray(env,3480000);

    //find and copy out the sps and pps data
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVBitStreamFilterContext* bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    AVCodecContext *avCodecContext = formatContext->streams[streamIndex]->codec;
    av_bitstream_filter_filter(
            bsfc, avCodecContext, NULL, &extraData, &extra_data_length, NULL, 0, 0);
    (*env)->SetByteArrayRegion(env,jbyteArray1,0,avCodecContext->extradata_size,avCodecContext->extradata);
    getExtraDatacallBackMethodID = (*env)->GetMethodID(env,class,"getExtraDatacallBack","([BI)V");
    (*env)->CallVoidMethod(env,object,getExtraDatacallBackMethodID,jbyteArray1,avCodecContext->extradata_size);

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == AVMEDIA_TYPE_VIDEO){
            packet.data[0] = 0;
            packet.data[1] = 0;
            packet.data[2] = 0;
            packet.data[3] = 1;
            int naul_type = packet.data[4] & 0X1F;
            (*env)->SetByteArrayRegion(env,jbyteArray1,0,packet.size,packet.data);
            if (naul_type == 5 || naul_type == 6) //I 帧数据
                 (*env)->CallVoidMethod(env,object,getIDRDataCallBackMethodID,jbyteArray1,packet.size);
            else
                 (*env)->CallVoidMethod(env,object,callBackMethodID,jbyteArray1,packet.size);
        }
    }

}

void handlePacket(JNIEnv *env, jobject pobject, AVPacket avPacket) {




  //  (*env)->ReleaseByteArrayElements(env,jbyteArray1,avPacket.data,0);
    //av_packet_unref(&avPacket);
    //av_free_packet(&avPacket);

}
