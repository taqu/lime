#ifndef INC_LFRAMEWORK_COMPONENTSPRITE2D_H__
#define INC_LFRAMEWORK_COMPONENTSPRITE2D_H__
/**
@file ComponentSprite2D.h
@author t-sakai
@date 2016/12/20 create
*/
#include "ComponentRenderer2D.h"
#include "../render/Texture.h"
#include <lmath/Vector4.h>
#include <lgraphics/TextureRef.h>

namespace lfw
{
    class Entity;
    class Camera;
    class RenderQueue2D;
    class RenderContext2D;

    class ComponentSprite2D : public ComponentRenderer2D
    {
    public:
        ComponentSprite2D();
        virtual ~ComponentSprite2D();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue2D& queue);
        virtual void draw(RenderContext2D& context);

        inline void setScreenPosition(f32 x, f32 y, f32 width, f32 height);
        void setScreenRect(f32 left, f32 top, f32 right, f32 bottom);

        inline const lmath::Vector4& getRect() const;
        inline lmath::Vector4& getRect();
        inline void setRect(const lmath::Vector4& rect);

        inline const u16* getTexcoord() const;
        inline u16* getTexcoord();
        void setTexcoord(f32 u0, f32 v0, f32 u1, f32 v1);

        void setTexture(lgfx::Texture2DRef& texture, lgfx::ShaderResourceViewRef& srv);
        inline s32 getMaterialId();
    protected:
        ComponentSprite2D(const ComponentSprite2D&);
        ComponentSprite2D& operator=(const ComponentSprite2D&);

        lmath::Vector4 rect_; //left, top, right, bottom
        u16 texcoord_[4]; //left, top, right, bottom
        u32 color_;
        lgfx::Texture2DRef texture_;
        lgfx::ShaderResourceViewRef srv_;
    };

    inline s32 ComponentSprite2D::getMaterialId()
    {
        lcore::uintptr_t ptr = reinterpret_cast<lcore::uintptr_t>(texture_.get());
        return static_cast<s32>(ptr);
    }

    inline void ComponentSprite2D::setScreenPosition(f32 x, f32 y, f32 width, f32 height)
    {
        setScreenRect(x, y, x+width, y+height);
    }

    inline const lmath::Vector4& ComponentSprite2D::getRect() const
    {
        return rect_;
    }

    inline lmath::Vector4& ComponentSprite2D::getRect()
    {
        return rect_;
    }

    inline void ComponentSprite2D::setRect(const lmath::Vector4& rect)
    {
        rect_ = rect;
    }

    inline const u16* ComponentSprite2D::getTexcoord() const
    {
        return texcoord_;
    }

    inline u16* ComponentSprite2D::getTexcoord()
    {
        return texcoord_;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTSPRITE2D_H__
