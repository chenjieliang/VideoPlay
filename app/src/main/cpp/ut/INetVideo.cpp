//
// Created by chenjieliang on 2019/8/28.
//

#include "INetVideo.h"

void INetVideo::send(XData data)
{
    if (data.data) {
        Notify(data);
    }
};