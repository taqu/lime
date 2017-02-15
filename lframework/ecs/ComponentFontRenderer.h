#ifndef INC_LFRAMEWORK_COMPONENTFONTRENDERER_H__
#define INC_LFRAMEWORK_COMPONENTFONTRENDERER_H__
/**
@file ComponentFontRenderer.h
@author t-sakai
@date 2016/12/25 create
*/
#include "ComponentRenderer2D.h"
#include <lcore/LString.h>
#include <lcore/Array.h>
#include <lmath/Vector4.h>
#include <lgraphics/TextureRef.h>

namespace lfw
{
    class Entity;
    class Camera;
    class RenderQueue2D;
    class RenderContext2D;
    class Font;

    class ComponentFontRenderer : public ComponentRenderer2D
    {
    public:
        ComponentFontRenderer();
        virtual ~ComponentFontRenderer();

        virtual void onCreate();
        virtual void onStart();
        virtual void update();
        virtual void onDestroy();
        virtual void addQueue(RenderQueue2D& queue);
        virtual void draw(RenderContext2D& context);

        const Font* getFont() const;
        bool setFont(s32 fontSlot);
        inline f32 getLeft() const;
        inline void setLeft(f32 left);
        inline f32 getTop() const;
        inline void setTop(f32 top);
        inline f32 getScale() const;
        inline void setScale(f32 scale);
        inline void setColor(u32 abgr);

        s32 getMaterialId();

        inline const lcore::String& getText() const;
        inline lcore::String& getText();
        inline void clearText();
        void updateText();
        f32 calcTextWidth() const;
        static f32 calcLineHeight(s32 fontSlot);
    protected:
        ComponentFontRenderer(const ComponentFontRenderer&);
        ComponentFontRenderer& operator=(const ComponentFontRenderer&);

        struct Sprite2D
        {
            lmath::Vector4 rect_;
            u16 uv_[4];
        };

        Font* font_;
        f32 left_;
        f32 top_;
        f32 scale_;
        u32 abgr_;
        lcore::String text_;
        lcore::ArrayPOD<Sprite2D> sprites_;
    };

    inline const lcore::String& ComponentFontRenderer::getText() const
    {
        return text_;
    }

    inline lcore::String& ComponentFontRenderer::getText()
    {
        return text_;
    }

    inline void ComponentFontRenderer::clearText()
    {
        text_.clear();
        updateText();
    }

    inline f32 ComponentFontRenderer::getLeft() const
    {
        return left_;
    }

    inline void ComponentFontRenderer::setLeft(f32 left)
    {
        left_ = left;
    }

    inline f32 ComponentFontRenderer::getTop() const
    {
        return top_;
    }

    inline void ComponentFontRenderer::setTop(f32 top)
    {
        top_ = top;
    }

    inline f32 ComponentFontRenderer::getScale() const
    {
        return scale_;
    }

    inline void ComponentFontRenderer::setScale(f32 scale)
    {
        scale_ = scale;
    }

    inline void ComponentFontRenderer::setColor(u32 abgr)
    {
        abgr_ = abgr;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTFONTRENDERER_H__
