//
// Created by Administrator on 2019-8-16.
//

#ifndef VIDEOPLAY_IFILTER_H
#define VIDEOPLAY_IFILTER_H


#include "../thread/IObserver.h"
#include "../XParameter.h"
#include "../SoundTouch/SoundTouchWrapper.h"

class STFilter : public IObserver
{
public:
    virtual bool Open(XParameter in);
    virtual void Update(XData data);
    virtual void Close();
    // 设置声道
    inline void setChannels(uint numChannels) {
        if (mSoundTouch != NULL) {
            mSoundTouch->setChannels(numChannels);
        }
    }

    // 设置采样率
    inline void setSampleRate(uint srate) {
        if (mSoundTouch != NULL) {
            mSoundTouch->setSampleRate(srate);
        }
    }

    // 设置播放速度
    inline void setRate(double rate) {
        this->playbackRate = rate;
        if (mSoundTouch != NULL) {
            mSoundTouch->setRate(rate);
        }
    }

    // 设置音调
    inline void setPitch(double pitch) {
        this->playbackPitch = pitch;
        if (mSoundTouch != NULL) {
            mSoundTouch->setPitch(pitch);
        }
    }

    // 设置节拍
    inline void setTempo(double tempo) {
        this->playbackTempo = tempo;
        if (mSoundTouch != NULL) {
            mSoundTouch->setTempo(tempo);
        }
    }

    // 设置速度改变
    inline void setRateChange(double rateChange) {
        this->playbackRateChanged = rateChange;
        if (mSoundTouch != NULL) {
            mSoundTouch->setRateChange(rateChange);
        }
    }

    // 设置节拍改变
    inline void setTempoChange(double tempoChange) {
        this->playbackTempoChanged = tempoChange;
        if (mSoundTouch != NULL) {
            mSoundTouch->setTempoChange(tempoChange);
        }
    }

    // 设置八度音调节
    inline void setPitchOctaves(double pitchOctaves) {
        this->playbackPitchOctaves = pitchOctaves;
        if (mSoundTouch != NULL) {
            mSoundTouch->setPitchOctaves(pitchOctaves);
        }
    }

    // 设置半音调节
    inline void setPitchSemiTones(double semiTones) {
        this->playbackPitchSemiTones = semiTones;
        if (mSoundTouch != NULL) {
            mSoundTouch->setPitchSemiTones(semiTones);
        }
    }
protected:
    std::mutex mux;
    //每次采样的字节数
    int bytes_per_sample;
    int channels;
    int sampleRate;
private:

    // 判断是否需要调整播放声音
    inline bool isNeedToAdjustAudio() {
        return (playbackRate != 1.0f || playbackPitch != 1.0f
                || playbackTempo != 1.0 || playbackRateChanged != 0 || playbackTempoChanged != 0
                || playbackPitchOctaves != 0 || playbackPitchSemiTones != 0);
    }

    short *touchBuffer; // soundtouch

    // 音频变速变调处理
    float playbackRate;                         // 播放速度
    float playbackPitch;                        // 播放音调
    double playbackTempo;                       // 节拍
    double playbackRateChanged;                 // 速度改变(-50 ~ 100%)
    double playbackTempoChanged;                // 节拍改变(-50 ~ 100%)
    double playbackPitchOctaves;                // 八度音调整(-1.0 ~ 1.0)
    double playbackPitchSemiTones;              // 半音调整(-12，12)
    SoundTouch *mSoundTouch;
};


#endif //VIDEOPLAY_IFILTER_H
