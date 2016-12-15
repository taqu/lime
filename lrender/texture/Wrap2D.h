#ifndef INC_LRENDER_WRAP2D_H__
#define INC_LRENDER_WRAP2D_H__
/**
@file Wrap2D.h
@author t-sakai
@date 2015/10/02 create
*/
#include "../lrender.h"
#include "Texture.h"

namespace lrender
{
    class Wrap2D
    {
    public:
        inline static void wrap(f32& u, f32& v, const Vector2& uv, Texture::WrapType type);
        inline static Vector2 wrap(const Vector2& uv, Texture::WrapType type);
        inline static f32 repeat(f32 x);
        inline static f32 mirror(f32 x);
    };

    inline void Wrap2D::wrap(f32& u, f32& v, const Vector2& uv, Texture::WrapType type)
    {
        switch(type)
        {
        case Texture::WrapType_Repeat:
            u = repeat(uv.x_);
            v = repeat(uv.y_);
            break;

        case Texture::WrapType_Mirror:
            u = mirror(uv.x_);
            v = mirror(uv.y_);
            break;

        case Texture::WrapType_Clamp:
            u = lcore::clamp01(uv.x_);
            v = lcore::clamp01(uv.y_);
            break;

        default:
            u = uv.x_;
            v = uv.y_;
            break;
        }
    }

    inline Vector2 Wrap2D::wrap(const Vector2& uv, Texture::WrapType type)
    {
        switch(type)
        {
        case Texture::WrapType_Repeat:
            return Vector2(repeat(uv.x_), repeat(uv.y_));

        case Texture::WrapType_Mirror:
            return Vector2(mirror(uv.x_), mirror(uv.y_));

        case Texture::WrapType_Clamp:
            return Vector2(lcore::clamp01(uv.x_), lcore::clamp01(uv.y_));

        default:
            return uv;
        }
    }

    inline f32 Wrap2D::repeat(f32 x)
    {
        x = lmath::fmod(x, 1.0f);
        return (x<0.0f)? (1.0f+x) : x;
    }

    inline f32 Wrap2D::mirror(f32 x)
    {
        x = lmath::fmod(x, 2.0f);
        if(1.0f<=x){
            return 2.0f-x;
        }else if(x<-1.0f){
            return 2.0f+x;
        }else{
            return 1.0f+x;
        }
    }
}
#endif //INC_LRENDER_WRAP2D_H__
