#ifndef INC_LFRAMEWORK_COMPONENTUITEXT_H_
#define INC_LFRAMEWORK_COMPONENTUITEXT_H_
/**
@file ComponentUIText.h
@author t-sakai
@date 2017/07/16 create
*/
#include "ComponentCanvasElement.h"
#include <lcore/LString.h>
#include <lcore/Array.h>
#include <lmath/Vector4.h>
#include <lgraphics/TextureRef.h>

namespace lfw
{
    class Font;

    class ComponentUIText : public ComponentCanvasElement
    {
    public:
        enum Align
        {
            Align_LeftTop,
            Align_Center,
            Align_RightBottom,
        };

        ComponentUIText();
        virtual ~ComponentUIText();

        virtual void onCreate();
        virtual void onStart();
        virtual void updateMesh(ComponentCanvas& canvas);

        const Font* getFont() const;
        bool setFont(s32 fontSlot);

        inline const lmath::Vector4& getRect() const;
        inline void setRect(f32 left, f32 top, f32 width, f32 height);
        inline void setRect(const lmath::Vector4& rect);

        inline f32 getScale() const;
        inline void setScale(f32 scale);
        inline void setColor(u32 abgr);

        inline void setHorizontalAlign(Align align);
        inline void setVerticalAlign(Align align);

        inline const lcore::String& text() const;
        inline lcore::String& text();
        inline void clearText();
        void updateText();

        static void calcTextBoxSize(f32& width, f32& height, const Char* text, f32 screenWidth, f32 screenHeight, Font* font);
        static f32 getLineHeight(s32 fontSlot);
    protected:
        ComponentUIText(const ComponentUIText&) = delete;
        ComponentUIText& operator=(const ComponentUIText&) = delete;

        struct Sprite2D
        {
            lmath::Vector4 rect_;
            u16 uv_[4];
        };

        f32 calcLineWidth(const Char* text) const;

        Font* font_;
        lmath::Vector4 rect_; //left, top, width, height
        f32 scale_;
        u32 abgr_;
        Align verticalAlign_;
        Align horizontalAlign_;
        lcore::String text_;
        lcore::Array<Sprite2D> sprites_;
    };

    inline const lcore::String& ComponentUIText::text() const
    {
        return text_;
    }

    inline lcore::String& ComponentUIText::text()
    {
        return text_;
    }

    inline void ComponentUIText::clearText()
    {
        text_.clear();
        updateText();
    }

    inline const lmath::Vector4& ComponentUIText::getRect() const
    {
        return rect_;
    }

    inline void ComponentUIText::setRect(f32 left, f32 top, f32 width, f32 height)
    {
        rect_.set(left, top, width, height);
        setDirty();
    }

    inline void ComponentUIText::setRect(const lmath::Vector4& rect)
    {
        rect_ = rect;
        setDirty();
    }

    inline f32 ComponentUIText::getScale() const
    {
        return scale_;
    }

    inline void ComponentUIText::setScale(f32 scale)
    {
        scale_ = scale;
        setDirty();
    }

    inline void ComponentUIText::setColor(u32 abgr)
    {
        abgr_ = abgr;
        setDirty();
    }

    inline void ComponentUIText::setHorizontalAlign(Align align)
    {
        horizontalAlign_ = align;
        setDirty();
    }

    inline void ComponentUIText::setVerticalAlign(Align align)
    {
        verticalAlign_ = align;
        setDirty();
    }
}
#endif //INC_LFRAMEWORK_COMPONENTFONTRENDERER_H_
