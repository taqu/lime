#ifndef INC_LRENDER_TEXTURE_H__
#define INC_LRENDER_TEXTURE_H__
/**
@file Texture.h
@author t-sakai
@date 2015/09/23 create
*/
#include "../lrender.h"
#include "../core/Spectrum.h"

namespace lrender
{
    class Intersection;
    class Texture2D;

    class Texture : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<Texture> pointer;

        enum FilterType
        {
            FilterType_NearestNeighbor = 0,
            FilterType_Linear,
        };

        enum WrapType
        {
            WrapType_Repeat =0,
            WrapType_Mirror,
            WrapType_Clamp,
        };

        virtual ~Texture()
        {}

        virtual Color3 sample(const Intersection& intersection) const=0;
        virtual Color3 sample(const Vector2& uv) const=0;
        virtual void getResolution(s32 resolution[3]) const=0;
        virtual Color3 avarage() const=0;
    protected:
        Texture(const Texture&);
        Texture& operator=(const Texture&);

        Texture()
            :filterType_(FilterType_Linear)
            ,wrapType_(WrapType_Repeat)
        {}

        FilterType filterType_;
        WrapType wrapType_;
    };

    class TextureConstant : public Texture
    {
    public:
        TextureConstant()
        {}

        explicit TextureConstant(const Color3& value)
            :value_(value)
        {}

        virtual ~TextureConstant()
        {}

        virtual Color3 sample(const Intersection& /*intersection*/) const
        {
            return value_;
        }

        virtual Color3 sample(const Vector2& /*uv*/) const
        {
            return value_;
        }

        virtual void getResolution(s32 resolution[3]) const
        {
            resolution[0] = resolution[1] = resolution[2] = 1;
        }

        virtual Color3 avarage() const
        {
            return value_;
        }

        const Color3& operator()() const
        {
            return value_;
        }

        Color3& operator()()
        {
            return value_;
        }
    protected:
        TextureConstant(const TextureConstant&);
        TextureConstant& operator=(const TextureConstant&);

        Color3 value_;
    };
}
#endif //INC_LRENDER_TEXTURE_H__
