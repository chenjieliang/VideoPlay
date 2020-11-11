//
// Created by chenjieliang on 2019/3/7.
//
#include "IPlayerProxy.h"
#include "builder/FFPlayerBuilder.h"
#include "Xlog.h"


void IPlayerProxy::Close()
{
    mux.lock();
    if(player)
        player->Close();
    mux.unlock();
}
void IPlayerProxy::Init(void *vm)
{
    mux.lock();
    if(vm)
    {
        FFPlayerBuilder::InitHard(vm);
    }

    if(!player)
    {
        player = FFPlayerBuilder::Get()->BuilderPlayer();
    }
    mux.unlock();
}

void IPlayerProxy::InitNet(void *vm)
{
    mux.lock();
    if(vm)
    {
        FFPlayerBuilder::InitHard(vm);
    }

    if(!player)
    {
        player = FFPlayerBuilder::Get()->BuilderNetPlayer();
    }
    mux.unlock();
}

bool IPlayerProxy::Open(const char *path)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        player->isHardDecode = isHardDecode;
        re = player->Open(path);
    }
    mux.unlock();
    return re;
}

bool IPlayerProxy::Open(int width,int height)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        player->isHardDecode = isHardDecode;
        re = player->Open(width,height);
    }
    mux.unlock();
    return re;
}

bool IPlayerProxy::Start()
{
    bool re = false;
    mux.lock();
    if(player)
    {
        re = player->Start();
    }
    mux.unlock();
    return re;
}

void IPlayerProxy::InitView(void *win)
{
    mux.lock();
    if(player)
    {
        player->InitView(win);
    }
    mux.unlock();
}

void IPlayerProxy::receiveVideoFrame(int type, int camType, int width, int height, int fps,
                                     long pts, unsigned char *dataArray, long size)
{
    mux.lock();
    if(player)
    {
        player->receiveVideoFrame(type,camType,width,height,fps,pts,dataArray,size);
    }
    mux.unlock();
}

void IPlayerProxy::receiveAudioFrame(int type, int channels, int sps, int bps, long pts,
                                unsigned char *dataArray, long size)
{
    mux.lock();
    if(player)
    {
        player->receiveAudioFrame(type,channels,sps,bps,pts,dataArray,size);
    }
    mux.unlock();
}

double IPlayerProxy::PlayPos()
{
    double pos = 0.0;
    mux.lock();
    if(player)
    {
       pos = player->PlayPos();
        //XLOGI("IPlayerProxy--> pos = %lf",pos);
    }
    mux.unlock();
    return pos;
}

bool IPlayerProxy::IsPause()
{
    bool re = false;
    mux.lock();
    if(player)
        re = player->IsPause();
    mux.unlock();
    return re;
}

void IPlayerProxy::SetPause(bool isP)
{
    mux.lock();
    if(player)
        player->SetPause(isP);
    mux.unlock();
}

bool IPlayerProxy::Seek(double pos)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        re = player->Seek(pos);
    }
    mux.unlock();
    return re;
}
bool IPlayerProxy::setSpeedRate(float rate)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        re = player->setSpeedRate(rate);
    }
    mux.unlock();
    return re;
}

bool IPlayerProxy::setVolume(int percent)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        re = player->setVolume(percent);
    }
    mux.unlock();
    return re;
}

bool IPlayerProxy::openVolume(bool isOpen)
{
    bool re = false;
    mux.lock();
    if(player)
    {
        re = player->openVolume(isOpen);
    }
    mux.unlock();
    return re;
}