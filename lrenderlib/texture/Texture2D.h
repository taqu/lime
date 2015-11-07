#ifndef INC_LRENDER_TEXTURE2D_H__
#define INC_LRENDER_TEXTURE2D_H__
/**
@file Texture2D.h
@author t-sakai
@date 2015/10/01 create
*/
#include "Texture.h"
#include "image/Image.h"

namespace lrender
{
    class Intersection;
    class Texture2D;

    class Texture2D : public Texture
    {
    public:
        Texture2D();
        explicit Texture2D(Image& image);
        virtual ~Texture2D();

        void setImage(Image& image);

        virtual Color3 sample(const Intersection& intersection) const;
        virtual Color3 sample(const Vector2& uv) const;
        virtual void getResolution(s32 resolution[3]) const;

        virtual Color3 avarage() const;
    protected:
        Texture2D(const Texture2D&);
        Texture2D& operator=(const Texture2D&);

        Color3 triangle(const Vector2& uv);

        Color3 sample(const Vector2& uv, Vector2 d0, Vector2 d1) const;
        Image image_;
    };
}
#endif //INC_LRENDER_TEXTURE2D_H__
