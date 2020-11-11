//
// Created by chenjieliang on 2019/3/4.
//
#include "FFPlayerBuilder.h"
#include "../demux/FFDemux.h"
#include "../decode/FFDecode.h"
#include "../resample/FFResample.h"
#include "../video/GLVideoView.h"
#include "../audio/SLAudioPlay.h"
#include "../ut/FFNetVideo.h"
#include "../ut/FFNetAudio.h"

IDemux * FFPlayerBuilder::CreateDemux()
{
    IDemux *ffdemux = new FFDemux();
    return ffdemux;
};

INetVideo * FFPlayerBuilder::CreateNetVideo()
{
    INetVideo *ffNetVideo = new FFNetVideo();
    return ffNetVideo;
}

INetAudio * FFPlayerBuilder::CreateNetAudio()
{
    INetAudio *ffNetAudio = new FFNetAudio();
    return ffNetAudio;
}

IDecode * FFPlayerBuilder::CreateDecode()
{
    IDecode *ffdecode = new FFDecode();
    return ffdecode;
};

IResample * FFPlayerBuilder::CreateResample()
{
    IResample *ffresample = new FFResample();
    return ffresample;
};

STFilter * FFPlayerBuilder::CreateSTFilter()
{
    STFilter *stFilter = new STFilter();
    return stFilter;
};

IVideoView * FFPlayerBuilder::CreateVideoView()
{
    IVideoView *glVideoView = new GLVideoView();
    return glVideoView;
};

IAudioPlay * FFPlayerBuilder::CreateAudioPlay()
{
    IAudioPlay *slAudioPlay = new SLAudioPlay();
    return slAudioPlay;
};

IPlayer *FFPlayerBuilder::CreatePlayer(unsigned char index)
{
    return IPlayer::Get(index);
};

void FFPlayerBuilder::InitHard(void *vm)
{
    FFDecode::InitHard(vm);
}
