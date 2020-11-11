//
// Created by chenjieliang on 2019/9/5.
//

#ifndef VIDEOPLAY_DECODEINIT_H
#define VIDEOPLAY_DECODEINIT_H

extern "C"
{
#include <libavformat/avformat.h>
}

enum {
    FLV_TAG_TYPE_AUDIO = 0x08,
    FLV_TAG_TYPE_VIDEO = 0x09,
    FLV_TAG_TYPE_META  = 0x12,
};


static class DecodeInit {

public:
    static AVStream *create_stream(AVFormatContext *s, int codec_type)
    {
        AVStream *st = avformat_new_stream(s, NULL);
        if (!st)
            return NULL;
        st->codec->codec_type = (AVMediaType)(codec_type);
        return st;
    }

    static int get_video_extradata(AVFormatContext *s, int video_index)
    {
        int  type, size, flags, pos, stream_type;
        int ret = -1;
        int64_t dts;
        bool got_extradata = false;

        if (!s || video_index < 0 || video_index > 2)
            return ret;

        for (;; avio_skip(s->pb, 4)) {
            pos  = avio_tell(s->pb);
            type = avio_r8(s->pb);
            size = avio_rb24(s->pb);
            dts  = avio_rb24(s->pb);
            dts |= avio_r8(s->pb) << 24;
            avio_skip(s->pb, 3);

            if (0 == size)
                break;
            if (FLV_TAG_TYPE_AUDIO == type || FLV_TAG_TYPE_META == type) {
                /*if audio or meta tags, skip them.*/
                avio_seek(s->pb, size, SEEK_CUR);
            } else if (type == FLV_TAG_TYPE_VIDEO) {
                /*if the first video tag, read the sps/pps info from it. then break.*/
                size -= 5;
                s->streams[video_index]->codec->extradata = (uint8_t *)(malloc(size + FF_INPUT_BUFFER_PADDING_SIZE));
                if (NULL == s->streams[video_index]->codec->extradata)
                    break;
                memset(s->streams[video_index]->codec->extradata, 0, size + FF_INPUT_BUFFER_PADDING_SIZE);
                memcpy(s->streams[video_index]->codec->extradata, s->pb->buf_ptr + 5, size);
                s->streams[video_index]->codec->extradata_size = size;
                ret = 0;
                got_extradata = true;
            } else  {
                /*The type unknown,something wrong.*/
                break;
            }

            if (got_extradata)
                break;
        }

        return ret;
    }

    static int init_decode(AVFormatContext *s) {
        int video_index = -1;
        int audio_index = -1;
        int ret = -1;

        if (!s)
            return ret;

        /*
        Get video stream index, if no video stream then create it.
        And audio so on.
        */
        if (0 == s->nb_streams) {
            create_stream(s, AVMEDIA_TYPE_VIDEO);
            create_stream(s, AVMEDIA_TYPE_AUDIO);
            video_index = 0;
            audio_index = 1;
        } else if (1 == s->nb_streams) {
            if (AVMEDIA_TYPE_VIDEO == s->streams[0]->codec->codec_type) {
                create_stream(s, AVMEDIA_TYPE_AUDIO);
                video_index = 0;
                audio_index = 1;
            } else if (AVMEDIA_TYPE_AUDIO == s->streams[0]->codec->codec_type) {
                create_stream(s, AVMEDIA_TYPE_VIDEO);
                video_index = 1;
                audio_index = 0;
            }
        } else if (2 == s->nb_streams) {
            if (AVMEDIA_TYPE_VIDEO == s->streams[0]->codec->codec_type) {
                video_index = 0;
                audio_index = 1;
            } else if (AVMEDIA_TYPE_VIDEO == s->streams[1]->codec->codec_type) {
                video_index = 1;
                audio_index = 0;
            }
        }

        /*Error. I can't find video stream.*/
        if (video_index != 0 && video_index != 1)
            return ret;

        //Init the audio codec(AAC).
        s->streams[audio_index]->codec->codec_id = AV_CODEC_ID_AAC;
        s->streams[audio_index]->codec->sample_rate = 48000;
        s->streams[audio_index]->codec->time_base.den = 48000;
        s->streams[audio_index]->codec->time_base.num = 1;
        s->streams[audio_index]->codec->bits_per_coded_sample = 16;
        s->streams[audio_index]->codec->channels = 2;
        s->streams[audio_index]->codec->channel_layout = 3;
        s->streams[audio_index]->pts_wrap_bits = 64;
        s->streams[audio_index]->time_base.den = 48000;
        s->streams[audio_index]->time_base.num = 1;

        //Init the video codec(H264).
        s->streams[video_index]->codec->codec_id = AV_CODEC_ID_H264;
        s->streams[video_index]->codec->width = 1920;
        s->streams[video_index]->codec->height = 1080;
        s->streams[video_index]->codec->ticks_per_frame = 2;
        s->streams[video_index]->codec->pix_fmt = (AVPixelFormat)(0);
        s->streams[video_index]->pts_wrap_bits = 64;
        s->streams[video_index]->time_base.den = 13540;
        s->streams[video_index]->time_base.num = 1;
        s->streams[video_index]->avg_frame_rate.den = 50;
        s->streams[video_index]->avg_frame_rate.num = 667;
        /*Need to change, different condition has different frame_rate. 'r_frame_rate' is new in ffmepg2.3.3*/
        s->streams[video_index]->r_frame_rate.den = 50;
        s->streams[video_index]->r_frame_rate.num = 667;
        /* H264 need sps/pps for decoding, so read it from the first video tag.*/
        ret = get_video_extradata(s, video_index);

        /*Update the AVFormatContext Info*/
        s->nb_streams = 2;
        /*empty the buffer.*/
        s->pb->buf_ptr = s->pb->buf_end;
        /*
         something wrong.
         TODO: find out the 'pos' means what.
         then set it.
         */
        s->pb->pos = (int64_t)(s->pb->buf_end);

        return ret;
    }
};


#endif //VIDEOPLAY_DECODEINIT_H
