#ifndef INC_LRENDER_SAMPLER_H__
#define INC_LRENDER_SAMPLER_H__
/**
@file Sampler.h
@author t-sakai
@date 2009/05/11 create
*/
#include "../lrendercore.h"

namespace lrender
{
    struct Sample
    {
        f32 x_;
        f32 y_;
        s32 subX_;
        s32 subY_;
    };

    class Sampler
    {
    public:
        static const s32 MAX_SAMPLES_PER_PIXEL = 64-1;
        static const s32 MAX_SAMPLES_BUFFER_SIZE = MAX_SAMPLES_PER_PIXEL+1;

        Sampler();

        Sampler(
            u32 numberPerPixel,
            s32 startx,
            s32 starty,
            s32 width,
            s32 height,
            u32 screenW,
            u32 screenH);

        ~Sampler();

        void init(
            u32 numberPerPixel,
            s32 startx,
            s32 starty,
            s32 width,
            s32 height,
            u32 screenW,
            u32 screenH);
        
        void getFirstSample(Sample& sample);
        void getNext(Sample& sample);
        bool isEnd() const{ return (totalCount_<=count_);}

        u32 getWidth() const{ return width_;}
        u32 getHeight() const{ return height_;}
    private:

        void initSamples();

        inline void convertScreenToVirtualScreen(f32& x, f32& y, s32 sx, s32 sy, f32 dx, f32 dy) const;

        s32 startSubX_;
        s32 startSubY_;
        s32 endSubX_;
        s32 endSubY_;

        u32 numPerPixel_; /// ピクセル辺りサンプル数

        u32 screenWidth_;
        u32 screenHeight_;
        f32 invScreenWidth_;
        f32 invScreenHeight_;
        u32 width_; /// 総X方向サンプル数
        u32 height_; /// 総Y方向サンプル数

        Vector2 samples_[MAX_SAMPLES_BUFFER_SIZE];

        u32 count_;
        u32 totalCount_;
    };
}

#endif //INC_LRENDER_SAMPLER_H__
