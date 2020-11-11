//
// Created by chenjieliang on 2019/8/29.
//

#ifndef VIDEOPLAY_FFNETAUDIO_H
#define VIDEOPLAY_FFNETAUDIO_H


#include "INetAudio.h"
#include "../XParameter.h"

class FFNetAudio : public INetAudio{

public:
    virtual void init();

    virtual XParameter getParameter();

    virtual void receive(
            int type,             //I、P、A
            int channels,         //声道数
            int sps,              //采样率
            int bps,              //
            long pts,             //时间戳
            unsigned char * dataArray,     //unint_8 、unsigned char
            long size);

private:
    XParameter param;

};


#endif //VIDEOPLAY_FFNETAUDIO_H
