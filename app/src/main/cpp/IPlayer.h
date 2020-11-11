//
// Created by chenjieliang on 2019/3/4.
//

#ifndef VIDEOPLAY_IPLAYER_H
#define VIDEOPLAY_IPLAYER_H


#include "thread/XThread.h"
#include "demux/IDemux.h"
#include "decode/IDecode.h"
#include "resample/IResample.h"
#include "video/IVideoView.h"
#include "audio/IAudioPlay.h"
#include "filter/STFilter.h"
#include "ut/INetVideo.h"
#include "ut/INetAudio.h"

class IPlayer : public XThread{
public:
    static IPlayer *Get(unsigned char index = 0);
    virtual bool Open(const char *path);
    virtual bool Open(int width,int height);
    virtual bool Start();
    virtual void InitView(void *win);
    virtual void Close();

    virtual void receiveVideoFrame(int type,              //I、P、A
                                   int camType,
                                   int width,
                                   int height,
                                   int fps,              //帧率
                                   long pts,             //时间戳
                                   unsigned char *dataArray,     //unint_8 、unsigned char
                                   long size);

    virtual void receiveAudioFrame(int type,             //I、P、A
                                   int channels,         //声道数
                                   int sps,              //采样率
                                   int bps,              //
                                   long pts,             //时间戳
                                   unsigned char * dataArray,     //unint_8 、unsigned char
                                   long size);

    //获取当前的播放进度
    virtual double PlayPos();
    virtual bool Seek(double pos);
    virtual void SetPause(bool isP);

    virtual bool setSpeedRate(float rate=1.0);
    virtual bool setVolume(int percent=50);
    virtual bool openVolume(bool isOpen = true);

    //是否视频硬解码
    bool isHardDecode = true;

    //音频输出参数配置
    XParameter outPara;

    IDemux *demux = 0;
    INetVideo *netVideo = 0;
    INetAudio *netAudio = 0;
    IDecode *vdecode = 0;
    IDecode *adecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;
    STFilter *stFilter = 0;

protected:
    //用作音视频同步
    void Main();

    IPlayer(){};
    std::mutex mux;
};


#endif //VIDEOPLAY_IPLAYER_H
