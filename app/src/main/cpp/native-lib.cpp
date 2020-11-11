//
// Created by chenjieliang on 2019/1/25.
//

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "builder/FFPlayerBuilder.h"
#include "IPlayerProxy.h"
#include "Xlog.h"


extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    FFPlayerBuilder::InitHard(vm);

    IPlayerProxy::Get()->Init(vm);

    return JNI_VERSION_1_4;
}
//===================   初始化View   ===================
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_XPlayView_InitView(JNIEnv *env, jobject instance, jobject surface) {
    //视频显示
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win);
}

//===================   打开播放地址   ===================
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_PlayListActivity_OpenUrl(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    IPlayerProxy::Get()->Open(url);
    IPlayerProxy::Get()->Start();

    env->ReleaseStringUTFChars(url_, url);
}

//===================   获取当前播放位置   ===================

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_videoplay_MainActivity_getPlayPos(JNIEnv *env, jobject instance) {
    jdouble pos = IPlayerProxy::Get()->PlayPos();
    return pos;
}

//===================         滑动       ===================
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_MainActivity_seekTo(JNIEnv *env, jobject instance, jdouble pos) {
    IPlayerProxy::Get()->Seek(pos);
}

//=========================    暂停  =========================
static bool isOpen = true;
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_XPlayView_playOnPause(JNIEnv *env, jobject instance) {
    IPlayerProxy::Get()->SetPause(!IPlayerProxy::Get()->IsPause());
    //isOpen = !isOpen;
    //IPlayerProxy::Get()->openVolume(isOpen);
}

//=========================    视频采集封装  =========================

extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_MainActivity_play(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    IPlayerProxy::Get()->Open(url);
    IPlayerProxy::Get()->Start();

    env->ReleaseStringUTFChars(url_, url);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_videoplay_TestActivity_getPlayPos(JNIEnv *env, jobject instance) {

    // TODO

}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_videoplay_TestActivity_seekTo(JNIEnv *env, jobject instance, jdouble pos) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_TestActivity_play(JNIEnv *env, jobject instance) {

    // TODO
    IPlayerProxy::Get()->Open(1280,720);
    IPlayerProxy::Get()->Start();

   // bool re = IPlayerProxy::Get()->setSpeedRate(1.0);

}extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_TestActivity_receiverVideoFrame(JNIEnv *env, jobject instance, jint type,
                                                           jint camType, jint width, jint height,
                                                           jint fps, jlong pts,
                                                           jbyteArray dataArray_, jlong size) {
    jbyte *dataArray = env->GetByteArrayElements(dataArray_, NULL);

    // TODO
    IPlayerProxy::Get()->receiveVideoFrame(type, camType, width, height, fps, pts,
                                           (unsigned char *)dataArray, size);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplay_TestActivity_receiverAudioFrame(JNIEnv *env, jobject instance, jint type,
                                                           jint channels, jint sps, jint bps,
                                                           jlong pts, jbyteArray dataArray_,
                                                           jlong size) {
    jbyte *dataArray = env->GetByteArrayElements(dataArray_, NULL);

    // TODO
    IPlayerProxy::Get()->receiveAudioFrame(type,channels,sps,bps,pts,(unsigned char *)dataArray,size);

}