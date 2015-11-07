#ifndef INC_LRENDER_IMAGE_H__
#define INC_LRENDER_IMAGE_H__
/**
@file Image.h
@author t-sakai
@date 2013/05/23 create
*/
#include "../lrender.h"
#include "core/Spectrum.h"

namespace lrender
{
    class Color4;

    class Image
    {
    public:
        Image();
        Image(s32 width, s32 height);
        Image(s32 width, s32 height, Color4* data);
        ~Image();

        inline bool valid() const;

        inline s32 getWidth() const;
        inline s32 getHeight() const;

        void clear(const Color4& rgba);

        inline void get(f32& r, f32& g, f32& b, s32 x, s32 y) const;
        inline Color3 getRGB(s32 x, s32 y) const;
        inline void set(f32 r, f32 g, f32 b, s32 x, s32 y);
        inline void set(const Color3& c, s32 x, s32 y);

        inline void get(f32& r, f32& g, f32& b, f32& a, s32 x, s32 y) const;
        inline Color4 getRGBA(s32 x, s32 y) const;
        inline void set(f32 r, f32 g, f32 b, f32 a, s32 x, s32 y);
        inline void set(const Color4& c, s32 x, s32 y);

        void sampleNNRGB(f32& r, f32& g, f32& b, f32 u, f32 v) const;
        Color3 sampleNNRGB(f32 u, f32 v) const;
        void sampleLinearRGB(f32& r, f32& g, f32& b, f32 u, f32 v) const;
        Color3 sampleLinearRGB(f32 u, f32 v) const;

        /**
        @brief Linear RGB to standard RGB
        */
        void linearToStandardRGB();

        /**
        @brief Standard RGB to linear RGB
        */
        void standardToLinearRGB();

        void toneMapping(f32 scale);
        void swap(Image& rhs);

        Color4 average() const;
        Color4* getData(){ return data_;}

        bool savePPM(const char* filename) const;
    private:
        Image(const Image&);
        Image& operator=(const Image&);

        s32 width_;
        s32 height_;
        Color4* data_;
    };

    inline bool Image::valid() const
    {
        return NULL != data_;
    }

    inline s32 Image::getWidth() const
    {
        return width_;
    }

    inline s32 Image::getHeight() const
    {
        return height_;
    }

    inline void Image::get(f32& r, f32& g, f32& b, s32 x, s32 y) const
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        const Color4& rgb = data_[y*width_ + x];
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
    }

    inline Color3 Image::getRGB(s32 x, s32 y) const
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);

        const Color4& rgb = data_[y*width_ + x];
        return Color3(rgb[0], rgb[1], rgb[2]);
    }

    inline void Image::set(f32 r, f32 g, f32 b, s32 x, s32 y)
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        Color4& rgb = data_[y*width_ + x];
        rgb[0] = r;
        rgb[1] = g;
        rgb[2] = b;
    }

    inline void Image::set(const Color3& c, s32 x, s32 y)
    {
        Color4& rgb = data_[y*width_ + x];
        rgb[0] = c[0];
        rgb[1] = c[1];
        rgb[2] = c[2];
    }

    inline void Image::get(f32& r, f32& g, f32& b, f32& a, s32 x, s32 y) const
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        const Color4& rgb = data_[y*width_ + x];
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
        a = rgb[3];
    }

    inline Color4 Image::getRGBA(s32 x, s32 y) const
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        return data_[y*width_ + x];
    }

    inline void Image::set(f32 r, f32 g, f32 b, f32 a, s32 x, s32 y)
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        Color4& rgb = data_[y*width_ + x];
        rgb[0] = r;
        rgb[1] = g;
        rgb[2] = b;
        rgb[3] = a;
    }

    inline void Image::set(const Color4& c, s32 x, s32 y)
    {
        LASSERT(NULL != data_);
        LASSERT(0<=x && x<width_);
        LASSERT(0<=y && y<height_);
        data_[y*width_ + x] = c;
    }
}
#endif //INC_LRENDER_IMAGE_H__
