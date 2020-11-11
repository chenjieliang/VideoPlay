//
// Created by chenjieliang on 2019/8/29.
//

#include "INetAudio.h"

void INetAudio::send(XData data)
{
    if (data.data) {
        Notify(data);
    }
};