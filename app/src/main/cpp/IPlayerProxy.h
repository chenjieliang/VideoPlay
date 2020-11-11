//
// Created by chenjieliang on 2019/3/7.
//

#ifndef VIDEOPLAY_IPLAYERPROXY_H
#define VIDEOPLAY_IPLAYERPROXY_H

#include "IPlayer.h"

class IPlayerProxy: public IPlayer{
public:
    static IPlayerProxy*Get()
    {
        static IPlayerProxy px;
        return &px;
    }

    void Init(void *vm = 0);
    void InitNet(void *vm = 0);

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
    virtual bool Seek(double pos);
    virtual bool setSpeedRate(float rate);
    virtual bool setVolume(int percent);
    virtual bool openVolume(bool isOpen = true);
    virtual void SetPause(bool isP);
    virtual bool IsPause();

    //获取当前播放的进度
    virtual double PlayPos();

protected:
    IPlayerProxy(){}
    IPlayer *player = 0;
    std::mutex mux;

};
#endif //VIDEOPLAY_IPLAYERPROXY_H
