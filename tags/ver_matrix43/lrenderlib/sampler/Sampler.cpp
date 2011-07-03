/**
@file Sampler.cpp
@author t-sakai
@date 2009/05/11
*/
#include "Sampler.h"

namespace lrender
{
    Sampler::Sampler()
        :startSubX_(0)
        ,startSubY_(0)
        ,endSubX_(0)
        ,endSubY_(0)
        ,numPerPixel_(1)
        ,screenWidth_(2)
        ,screenHeight_(2)
        ,invScreenWidth_(1.0f)
        ,invScreenHeight_(1.0f)
        ,width_(0)
        ,height_(0)
        ,count_(0)
        ,totalCount_(0)
    {
    }

    Sampler::Sampler(
        u32 numberPerPixel,
        s32 startx,
        s32 starty,
        s32 width,
        s32 height,
        u32 screenW,
        u32 screenH)
        :numPerPixel_(numberPerPixel)
        ,screenWidth_(screenW)
        ,screenHeight_(screenH)
    {
        LASSERT(screenW > 1);
        LASSERT(screenH > 1);

        startSubX_ = numPerPixel_ * startx;
        startSubY_ = numPerPixel_ * starty;
        endSubX_ = startSubX_ + numPerPixel_ * width - 1;
        endSubY_ = startSubY_ + numPerPixel_ * height - 1;

        invScreenWidth_ = (screenW == 1)? 1.0f : 1.0f/(screenW-1);
        invScreenHeight_ = (screenH == 1)? 1.0f : 1.0f/(screenH-1);

        width_ = lmath::absolute(width);
        height_ = lmath::absolute(height);

        initSamples();
    }

    Sampler::~Sampler()
    {
    }

    void Sampler::init(
        u32 numberPerPixel,
        s32 startx,
        s32 starty,
        s32 width,
        s32 height,
        u32 screenW,
        u32 screenH)
    {
        LASSERT(screenW > 1);
        LASSERT(screenH > 1);

        numPerPixel_ = numberPerPixel;

        startSubX_ = numPerPixel_ * startx;
        startSubY_ = numPerPixel_ * starty;
        endSubX_ = startSubX_ + numPerPixel_ * width - 1;
        endSubY_ = startSubY_ + numPerPixel_ * height - 1;

        screenWidth_ = screenW;
        screenHeight_ = screenH;

        invScreenWidth_ = (screenW <= 1)? 1.0f : 1.0f/(screenW-1);
        invScreenHeight_ = (screenH <= 1)? 1.0f : 1.0f/(screenH-1);

        width_ = lmath::absolute(width);
        height_ = lmath::absolute(height);

        initSamples();
    }

    void Sampler::initSamples()
    {
        LASSERT(numPerPixel_ <= MAX_SAMPLES_PER_PIXEL);

        f32 sqrtSamples = lmath::sqrt( static_cast<f32>(numPerPixel_) );
        
        s32 sx = lmath::ceil(sqrtSamples);    //サブピクセル列数
        s32 sy = (sx<=1)? 1 : numPerPixel_/sx;//サブピクセル行数
        numPerPixel_ = sx*sy;

        f32 subw = 1.0f/sx; //サブピクセルの幅
        f32 subh = 1.0f/sy; //サブピクセルの高さ

        //左上のサンプル点座標
        f32 samplex = 0.5f*subw  - 0.5f;
        f32 sampley = -0.5f*subh  + 0.5f;

        for(s32 j=0; j<sy; ++j){
            samplex = 0.5f*subw - 0.5f;
            for(s32 i=0; i<sx; ++i){
                samples_[j*sx+i].set(samplex, sampley);
                samplex += subw;
            }
            sampley -= subh;
        }

        //総サンプル数
        totalCount_ = width_ * height_ * numPerPixel_;
    }

    void Sampler::getFirstSample(Sample& sample)
    {
        sample.subX_ = startSubX_;
        sample.subY_ = startSubY_;

        s32 screenX = sample.subX_ / numPerPixel_;
        s32 screenY = sample.subY_ / numPerPixel_;

        f32 virtualX;
        f32 virtualY;
        convertScreenToVirtualScreen(virtualX, virtualY, screenX, screenY, samples_[0]._x, samples_[0]._y);

        sample.x_ = virtualX;
        sample.y_ = virtualY;

        count_ = 0;
    }

    void Sampler::getNext(Sample& sample)
    {
        ++count_;

        // １ピクセル内のサンプル数を超えれば次のピクセルへ
        if(sample.subX_ >= endSubX_){
            if(sample.subY_ >= endSubY_){
                return;
            }
            sample.subX_ = startSubX_;
            ++sample.subY_;
        }else{
            ++sample.subX_;
        }

        s32 screenX = sample.subX_ / numPerPixel_;
        s32 screenY = sample.subY_ / numPerPixel_;

        s32 index = count_ % numPerPixel_;
        f32 virtualX;
        f32 virtualY;
        convertScreenToVirtualScreen(virtualX, virtualY, screenX, screenY, samples_[index]._x, samples_[index]._y);
        sample.x_ = virtualX;
        sample.y_ = virtualY;
    }

    inline void Sampler::convertScreenToVirtualScreen(f32& x, f32& y, s32 sx, s32 sy, f32 dx, f32 dy) const
    {
        x = (2.0f*sx - screenWidth_ + 1.0f + dx) * invScreenWidth_;
        y = (2.0f*sy - screenHeight_ + 1.0f + dy) * -invScreenHeight_;
    }
}

