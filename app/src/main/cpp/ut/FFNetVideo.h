//
// Created by chenjieliang on 2019/8/29.
//

#ifndef VIDEOPLAY_FFNETVIDEO_H
#define VIDEOPLAY_FFNETVIDEO_H


#include "INetVideo.h"

class FFNetVideo : public INetVideo {

public:
    virtual void init(int width, int height);

    virtual XParameter getParameter();

    virtual void receive(int type,              //I、P、A
                         int camType,
                         int width,
                         int height,
                         int fps,              //帧率
                         long pts,             //时间戳
                         unsigned char *dataArray,     //unint_8 、unsigned char
                         long size);

private:
    XParameter param;
};


#endif //VIDEOPLAY_FFNETVIDEO_H
