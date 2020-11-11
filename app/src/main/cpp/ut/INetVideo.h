//
// Created by chenjieliang on 2019/8/28.
//

#ifndef VIDEOPLAY_INETVIDEO_H
#define VIDEOPLAY_INETVIDEO_H


#include "../thread/IObserver.h"
#include "../XParameter.h"

class INetVideo : public IObserver {

public:

    virtual void init(int width, int height) = 0;

    virtual XParameter getParameter() = 0;

    virtual void receive(int type,              //I、P、A
                   int camType,
                   int width,
                   int height,
                   int fps,              //帧率
                   long pts,             //时间戳
                   unsigned char *dataArray,     //unint_8 、unsigned char
                   long size) = 0;

    void send(XData data);

};


#endif //VIDEOPLAY_INETVIDEO_H
