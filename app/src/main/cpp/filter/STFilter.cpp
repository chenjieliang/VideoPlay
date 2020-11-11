//
// Created by Administrator on 2019-8-16.
//

#include "STFilter.h"
#include "../Xlog.h"
#include <libavcodec/avcodec.h>

void STFilter::Update(XData data)
{
    if (data.size==0)
    {
        return;
    }
    if (mSoundTouch!=NULL && isNeedToAdjustAudio()== true) {
        mux.lock();
        uint8_t *buffer = data.data;
        int outsize = data.size;
        int len = data.samplesLen;
        //int channels = data.channels;
        int bytes_per_sample = data.bytes_per_sample;
        touchBuffer = (short*)malloc(outsize * 2 * 2);
        for (auto i = 0; i < outsize / 2 + 1; i++) {
            touchBuffer[i] = (buffer[i * 2] | (buffer[i * 2 + 1] << 8));
        }
        // 每个声道采样数量
        int put_n_sample = len / this->channels;
        int nb = 0;
        int pcm_data_size = 0;
        // 压入采样数据
        mSoundTouch->putSamples((const SAMPLETYPE *) touchBuffer, len);

        do {
            XData outData;
            outData.Alloc(outsize * 2 * 2);
            // 获取转换后的数据
            short *buf = (short *)(outData.data);
            nb = mSoundTouch->receiveSamples(buf, len);
            //计算转换后的数量大小
            pcm_data_size = nb * this->channels * bytes_per_sample;
            if (pcm_data_size > 0) {
                outData.pts = data.pts;
                outData.size = pcm_data_size;
                this->Notify(outData);
            } else {
                data.Drop();
                outData.Drop();
                delete buf;
                //mSoundTouch->flush();
                mux.unlock();
                return;
            }
        } while (nb != 0);

        mux.unlock();
    } else {
        this->Notify(data);
    }
}

void STFilter::Close()
{
    mux.lock();
    if (mSoundTouch) {
        mSoundTouch->clear();
        delete mSoundTouch;
        mSoundTouch = NULL;
    }
    mux.unlock();
}

bool STFilter::Open(XParameter in)
{
    Close();

    this->channels = in.channels;
    this->sampleRate = in.sample_rate;

    mSoundTouch = new SoundTouch();
    playbackRate = 1.0;
    playbackPitch = 1.0;
    playbackTempo = 1.0;
    playbackRateChanged = 0;
    playbackTempoChanged = 0;
    playbackPitchOctaves = 0;
    playbackPitchSemiTones = 0;

    // 设置采样率
    setSampleRate(this->sampleRate);
    // 设置声道数
    setChannels(this->channels);
    // 设置播放速度
     setRate(playbackRate);
    // 设置播放节拍
    setTempo(playbackTempo);
    setPitch(playbackPitch);
    return true;
}