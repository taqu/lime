/**
@file Image.cpp
@author t-sakai
@date 2013/05/23 create
*/
#include "Image.h"

namespace lrender
{
    Image::Image()
        :width_(0)
        ,height_(0)
        ,data_(NULL)
    {
    }

    Image::Image(s32 width, s32 height)
        :width_(width)
        ,height_(height)
    {
        data_ = LIME_NEW Color4[width*height];
    }

    Image::Image(s32 width, s32 height, Color4* data)
        :width_(width)
        ,height_(height)
        ,data_(data)
    {
    }

    Image::~Image()
    {
        LIME_DELETE_ARRAY(data_);
    }

    void Image::clear(const Color4& rgba)
    {
        for(s32 i=0; i<(width_*height_); ++i){
            data_[i] = rgba;
        }
    }

    void Image::sampleNNRGB(f32& r, f32& g, f32& b, f32 u, f32 v) const
    {
        LASSERT(0.0f<=u && u<=1.0f);
        LASSERT(0.0f<=v && v<=1.0f);
        u *= width_;
        v *= height_;

        s32 iu0 = lcore::clamp(static_cast<s32>(u), 0, width_-1);
        s32 iv0 = lcore::clamp(static_cast<s32>(v), 0, height_-1);

        get(r, g, b, iu0, iv0);
    }

    Color3 Image::sampleNNRGB(f32 u, f32 v) const
    {
        f32 r,g,b;
        sampleNNRGB(r, g, b, u, v);
        return Color3(r, g, b);
    }

    void Image::sampleLinearRGB(f32& r, f32& g, f32& b, f32 u, f32 v) const
    {
        LASSERT(0.0f<=u && u<=1.0f);
        LASSERT(0.0f<=v && v<=1.0f);
        u *= width_;
        v *= height_;

        s32 iu0 = lcore::clamp(static_cast<s32>(u), 0, width_-1);
        s32 iv0 = lcore::clamp(static_cast<s32>(v), 0, height_-1);

        s32 iu1 = lcore::clamp(iu0+1, iu0, width_-1);
        s32 iv1 = lcore::clamp(iv0+1, iv0, height_-1);

        f32 tu = u - iu0;
        f32 tv = v - iv0;

        f32 r0, g0, b0;
        f32 r1, g1, b1;

        get(r0, g0, b0, iu0, iv0);
        get(r1, g1, b1, iu1, iv0);

        r = lcore::lerp(r0, r1, tu);
        g = lcore::lerp(g0, g1, tu);
        b = lcore::lerp(b0, b1, tu);

        get(r0, g0, b0, iu0, iv1);
        get(r1, g1, b1, iu1, iv1);

        r0 = lcore::lerp(r0, r1, tu);
        g0 = lcore::lerp(g0, g1, tu);
        b0 = lcore::lerp(b0, b1, tu);


        r = lcore::lerp(r, r0, tv);
        g = lcore::lerp(g, g0, tv);
        b = lcore::lerp(b, b0, tv);
    }

    Color3 Image::sampleLinearRGB(f32 u, f32 v) const
    {
        f32 r,g,b;
        sampleLinearRGB(r, g, b, u, v);
        return Color3(r, g, b);
    }

    void Image::linearToStandardRGB()
    {
        lrender::linearToStandardRGB(width_, height_, data_);
    }

    void Image::standardToLinearRGB()
    {
        lrender::standardToLinearRGB(width_, height_, data_);
    }

    void Image::toneMapping(f32 scale)
    {
        lrender::toneMapping(width_, height_, data_, scale);
    }

    void Image::swap(Image& rhs)
    {
        lcore::swap(width_, rhs.width_);
        lcore::swap(height_, rhs.height_);
        lcore::swap(data_, rhs.data_);
    }

    Color4 Image::average() const
    {
        if(width_<=0 || height_<=0){
            return Color4::black();
        }

        __m128 avg = _mm_setzero_ps();

        s32 count = width_*height_;
        for(s32 i=0; i<count; ++i){
            avg = _mm_add_ps(avg, _mm_loadu_ps((f32*)&data_[i]));
        }
        __m128 c = _mm_set1_ps((f32)count);
        avg = _mm_div_ps(avg, c);
        Color4 ret;
        _mm_storeu_ps((f32*)&ret, avg);
        return ret;
    }

    bool Image::savePPM(const char* filename) const
    {
        return lrender::savePPM(filename, data_, width_, height_);
    }
}
