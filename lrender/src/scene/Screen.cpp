/**
@file Screen.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "scene/Screen.h"
#include "core/Spectrum.h"
#include "filter/Filter.h"

namespace lrender
{
    Screen::Screen()
        :width_(0)
        ,height_(0)
        ,invWidth_(1.0f)
        ,invHeight_(1.0f)
        ,pixels_(NULL)
        ,weights_(NULL)
    {
    }

    Screen::Screen(s32 width, s32 height, Filter* filter)
        :filter_(NULL)
        ,filterWeightsX_(NULL)
        ,filterWeightsY_(NULL)
        ,width_(width)
        ,height_(height)
        ,invWidth_(1.0f/width)
        ,invHeight_(1.0f/height)
        ,pixels_(NULL)
    {
        LASSERT(0<width);
        LASSERT(0<height);
        pixels_ = LNEW Color3[width_*height_];
        weights_ = LNEW f32[width_*height_];
        setFilter(filter);
    }

    Screen::~Screen()
    {
        LDELETE_ARRAY(filterWeightsX_);
        LDELETE_ARRAY(weights_);
        LDELETE_ARRAY(pixels_);
    }

    void Screen::clear(const Color3& rgb)
    {
        s32 size = width_*height_;
        for(s32 i=0; i<size; ++i){
            pixels_[i] = rgb;
        }
    }

    void Screen::clearWeights()
    {
        s32 size = width_*height_ * sizeof(f32);
        lcore::memset(weights_, 0, size);
    }

    const Color3& Screen::get(s32 x, s32 y) const
    {
        return pixels_[y*width_ + x];
    }

    void Screen::set(s32 x, s32 y, const Color3& rgb)
    {
        pixels_[y*width_ + x] = rgb;
    }

    void Screen::setFiltered(f32 x, f32 y, const Color3& rgb)
    {
        x -= 0.5f;
        y -= 0.5f;
        f32 radius = filter_->getRadius();
        s32 minx = lcore::maximum(lmath::ceilS32(x-radius), 0);
        s32 miny = lcore::maximum(lmath::ceilS32(y-radius), 0);
        s32 maxx = lcore::minimum(lmath::floorS32(x+radius), width_-1);
        s32 maxy = lcore::minimum(lmath::floorS32(y+radius), height_-1);

        for(s32 i=minx; i<=maxx; ++i){
            filterWeightsX_[i-minx] = filter_->evaluateDiscretized(i-x);
        }
        for(s32 i=miny; i<=maxy; ++i){
            filterWeightsY_[i-miny] = filter_->evaluateDiscretized(i-y);
        }

        for(s32 y=miny; y<=maxy; ++y){
            f32 weightY = filterWeightsY_[y-miny];
            s32 oy = y*width_;
            Color3* row = pixels_ + oy;
            f32* roww = weights_ + oy;
            for(s32 x=minx; x<=maxx; ++x){
                f32 weight = filterWeightsX_[x-minx] * weightY;
                row[x].muladd(weight, rgb);
                roww[x] += weight;
            }
        }
    }

    void Screen::setFilter(Filter* filter)
    {
        filter_ = filter;
        s32 radius = lmath::floorS32(filter_->getRadius()) + 1;

        LDELETE_ARRAY(filterWeightsX_);
        filterWeightsX_ = LNEW f32[radius*4];
        filterWeightsY_ = filterWeightsX_ + radius*2;
    }

    void Screen::divWeights()
    {
        s32 size = width_*height_;
        for(s32 i=0; i<size; ++i){
            if(lmath::isZeroPositive(weights_[i])){
                continue;
            }
            pixels_[i] /= weights_[i];
        }
    }

    void Screen::linearToStandardRGB()
    {
        lrender::linearToStandardRGB(width_, height_, pixels_);
    }

    void Screen::toneMapping(f32 scale)
    {
        lrender::toneMapping(width_, height_, pixels_, scale);
    }

    void Screen::swap(Screen& rhs)
    {
        lcore::swap(filter_, rhs.filter_);
        lcore::swap(filterWeightsX_, rhs.filterWeightsX_);
        lcore::swap(filterWeightsY_, rhs.filterWeightsY_);
        lcore::swap(width_, rhs.width_);
        lcore::swap(height_, rhs.height_);
        lcore::swap(invWidth_, rhs.invWidth_);
        lcore::swap(invHeight_, rhs.invHeight_);
        lcore::swap(pixels_, rhs.pixels_);
        lcore::swap(weights_, rhs.weights_);
    }

    bool Screen::savePPM(const char* filename) const
    {
        return lrender::savePPM(filename, pixels_, width_, height_);
    }
}
