/**
@file Spectrum.cpp
@author t-sakai
@date 2015/09/23 create
*/
#include "Spectrum.h"

namespace lrender
{
    //---------------------------------------------------------------------
    //---
    //--- Color3
    //---
    //---------------------------------------------------------------------
    f32 Color3::getLuminance() const
    {
        return values_[0] * 0.212671f + values_[1] * 0.715160f + values_[2] * 0.072169f;
    }

    void Color3::getRGB(u8& r, u8& g, u8& b) const
    {
        r = static_cast<u8>(255 * lcore::clamp01(values_[0]));
        g = static_cast<u8>(255 * lcore::clamp01(values_[1]));
        b = static_cast<u8>(255 * lcore::clamp01(values_[2]));
    }

    void Color3::setRGB(u8 r, u8 g, u8 b)
    {
        static const f32 inv = 1.0f/255.0f;
        values_[0] = lcore::clamp01(inv * r);
        values_[1] = lcore::clamp01(inv * g);
        values_[2] = lcore::clamp01(inv * b);
    }

    Color3 Color3::linearToStandard() const
    {
        f32 v0 = (values_[0]<=0.0031308f)? 12.92f*values_[0] : 1.055f*lmath::pow(values_[0], 1.0f/2.4f) - 0.055f;
        f32 v1 = (values_[1]<=0.0031308f)? 12.92f*values_[1] : 1.055f*lmath::pow(values_[1], 1.0f/2.4f) - 0.055f;
        f32 v2 = (values_[2]<=0.0031308f)? 12.92f*values_[2] : 1.055f*lmath::pow(values_[2], 1.0f/2.4f) - 0.055f;
        return Color3(v0, v1, v2);
    }

    Color3 Color3::standardToLinear() const
    {
        f32 v0 = (values_[0]<=0.04045f)? values_[0]/12.92f : lmath::pow((values_[0]+0.055f)/1.055f, 2.4f);
        f32 v1 = (values_[1]<=0.04045f)? values_[1]/12.92f : lmath::pow((values_[1]+0.055f)/1.055f, 2.4f);
        f32 v2 = (values_[2]<=0.04045f)? values_[2]/12.92f : lmath::pow((values_[2]+0.055f)/1.055f, 2.4f);
        return Color3(v0, v1, v2);
    }

    Color3 Color3::gamma(f32 x) const
    {
        f32 v0 = lmath::pow(values_[0], x);
        f32 v1 = lmath::pow(values_[1], x);
        f32 v2 = lmath::pow(values_[2], x);
        return Color3(v0, v1, v2);
    }

    //---------------------------------------------------------------------
    //---
    //--- Color4
    //---
    //---------------------------------------------------------------------
    f32 Color4::getLuminance() const
    {
        return values_[0] * 0.212671f + values_[1] * 0.715160f + values_[2] * 0.072169f;
    }

    void Color4::getRGBA(u8& r, u8& g, u8& b, u8& a) const
    {
        r = static_cast<u8>(255 * lcore::clamp01(values_[0]));
        g = static_cast<u8>(255 * lcore::clamp01(values_[1]));
        b = static_cast<u8>(255 * lcore::clamp01(values_[2]));
        a = static_cast<u8>(255 * lcore::clamp01(values_[3]));
    }

    void Color4::setRGBA(u8 r, u8 g, u8 b, u8 a)
    {
        static const f32 inv = 1.0f/255.0f;
        values_[0] = lcore::clamp01(inv * r);
        values_[1] = lcore::clamp01(inv * g);
        values_[2] = lcore::clamp01(inv * b);
        values_[3] = lcore::clamp01(inv * a);
    }

    Color4 Color4::linearToStandard() const
    {
        f32 v0 = (values_[0]<=0.0031308f)? 12.92f*values_[0] : 1.055f*lmath::pow(values_[0], 1.0f/2.4f) - 0.055f;
        f32 v1 = (values_[1]<=0.0031308f)? 12.92f*values_[1] : 1.055f*lmath::pow(values_[1], 1.0f/2.4f) - 0.055f;
        f32 v2 = (values_[2]<=0.0031308f)? 12.92f*values_[2] : 1.055f*lmath::pow(values_[2], 1.0f/2.4f) - 0.055f;
        return Color4(v0, v1, v2, values_[3]);
    }

    Color4 Color4::standardToLinear() const
    {
        f32 v0 = (values_[0]<=0.04045f)? values_[0]/12.92f : lmath::pow((values_[0]+0.055f)/1.055f, 2.4f);
        f32 v1 = (values_[1]<=0.04045f)? values_[1]/12.92f : lmath::pow((values_[1]+0.055f)/1.055f, 2.4f);
        f32 v2 = (values_[2]<=0.04045f)? values_[2]/12.92f : lmath::pow((values_[2]+0.055f)/1.055f, 2.4f);
        return Color4(v0, v1, v2, values_[3]);
    }

    Color4 Color4::gamma(f32 x) const
    {
        f32 v0 = lmath::pow(values_[0], x);
        f32 v1 = lmath::pow(values_[1], x);
        f32 v2 = lmath::pow(values_[2], x);
        return Color4(v0, v1, v2, values_[3]);
    }
}
