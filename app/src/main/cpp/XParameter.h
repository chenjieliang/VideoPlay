//
// Created by chenjieliang on 2019/2/12.
//

#ifndef VIDEOPLAY_XPARAMETER_H
#define VIDEOPLAY_XPARAMETER_H

struct AVCodecParameters;
struct AVRational;
class XParameter
{
public:
    AVCodecParameters *para = 0;

    int channels = 2;
    int sample_rate = 0;//44100;
    AVRational *time_base;

    /**
     * Video only. The aspect ratio (width / height) which a single pixel
     * should have when displayed.
     *
     * When the aspect ratio is unknown / undefined, the numerator should be
     * set to 0 (the denominator may have any value).
     */
    AVRational *sample_aspect_ratio;
};
#endif //VIDEOPLAY_XPARAMETER_H
