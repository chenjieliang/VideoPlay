//
// Created by chenjieliang on 2019/2/12.
//
#include "IVideoView.h"
#include "../Xlog.h"

void IVideoView::Update(XData data)
{
    this->Render(data);
}
