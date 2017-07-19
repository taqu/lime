#ifndef INC_LFRAMEWORK_COMPONENTUIIMAGE_H__
#define INC_LFRAMEWORK_COMPONENTUIIMAGE_H__
/**
@file ComponentUIImage.h
@author t-sakai
@date 2017/07/16 create
*/
#include "ComponentCanvasElement.h"
#include <lmath/Vector4.h>
#include <lgraphics/ViewRef.h>

namespace lfw
{
    class ComponentUIImage : public ComponentCanvasElement
    {
    public:
        ComponentUIImage();
        virtual ~ComponentUIImage();

        virtual void updateMesh(ComponentCanvas& canvas);

        inline const lmath::Vector4& getRect() const;
        inline void setRect(f32 left, f32 top, f32 right, f32 bottom);
        inline void setRect(const lmath::Vector4& rect);
        const lmath::Vector4& getUVRect() const;
        void setUVRect(f32 left, f32 top, f32 right, f32 bottom);

        inline f32 getScale() const;
        inline void setScale(f32 scale);
        inline void setColor(u32 abgr);

        inline lgfx::ShaderResourceViewRef& getImage();
        inline void setImage(lgfx::ShaderResourceViewRef& image);
    protected:
        ComponentUIImage(const ComponentUIImage&) = delete;
        ComponentUIImage& operator=(const ComponentUIImage&) = delete;

        lmath::Vector4 rect_; //left, top, width, height
        u16 uvRect_[4];
        f32 scale_;
        u32 abgr_;
        lgfx::ShaderResourceViewRef image_;
    };

    inline const lmath::Vector4& ComponentUIImage::getRect() const
    {
        return rect_;
    }

    inline void ComponentUIImage::setRect(f32 left, f32 top, f32 right, f32 bottom)
    {
        rect_.set(left, top, right, bottom);
    }

    inline void ComponentUIImage::setRect(const lmath::Vector4& rect)
    {
        rect_ = rect;
    }

    inline f32 ComponentUIImage::getScale() const
    {
        return scale_;
    }

    inline void ComponentUIImage::setScale(f32 scale)
    {
        scale_ = scale;
    }

    inline void ComponentUIImage::setColor(u32 abgr)
    {
        abgr_ = abgr;
    }

    inline lgfx::ShaderResourceViewRef& ComponentUIImage::getImage()
    {
        return image_;
    }

    inline void ComponentUIImage::setImage(lgfx::ShaderResourceViewRef& image)
    {
        image_ = image;
    }
}
#endif //INC_LFRAMEWORK_COMPONENTUIIMAGE_H__
