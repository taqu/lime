/**
@file ComponentUIText.cpp
@author t-sakai
@date 2017/07/16 create
*/
#include "ecs/ComponentUIText.h"
#include <lmath/lmath.h>
#include "System.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentCanvas.h"
#include "render/Renderer.h"
#include "resource/Font.h"
#include "resource/FontManager.h"
#include "resource/Resources.h"

namespace lfw
{
    ComponentUIText::ComponentUIText()
        :font_(NULL)
        ,rect_(lmath::Vector4::zero())
        ,scale_(1.0f)
        ,abgr_(0xFFFFFFFFU)
        ,verticalAlign_(Align_Center)
        ,horizontalAlign_(Align_Center)
    {
    }

    ComponentUIText::~ComponentUIText()
    {
    }

    void ComponentUIText::onStart()
    {
        ComponentCanvasElement::onStart();
        if(0<=text_.length()){
            updateText();
        }
    }

    void ComponentUIText::updateMesh(ComponentCanvas& canvas)
    {
        if(NULL != font_){
            for(s32 i=0; i<sprites_.size(); ++i){
                canvas.addRect(sprites_[i].rect_, abgr_, sprites_[i].uv_, font_->getSRV(), *font_->getPSOutline());
            }
        }
    }

    const Font* ComponentUIText::getFont() const
    {
        return font_;
    }

    bool ComponentUIText::setFont(s32 fontSlot)
    {
        Resources& resources = System::getResources();

        Font* font = resources.getFontManager().get(fontSlot);
        if(NULL != font){
            font_ = font;
            return true;
        }
        return false;
    }

    void ComponentUIText::updateText()
    {
        if(NULL == font_ || NULL == canvas_){
            return;
        }

        f32 fontToScreenX = scale_;//font_->getInvWidth() * canvas_->getWidth() * scale_;
        f32 fontToScreenY = scale_;//font_->getInvHeight() * canvas_->getHeight() * scale_;

        f32 lineHeight = fontToScreenY * font_->getLineHeight();

        f32 textBoxWidth;
        f32 textBoxHeight;
        {
            f32 lineWidth = 0.0f;
            textBoxWidth = 0.0f;
            textBoxHeight = lineHeight;
            for(s32 i=0; i<text_.length(); ++i){
                if(text_[i] == lcore::CharLF){
                    textBoxWidth = lcore::maximum(textBoxWidth, lineWidth);
                    lineWidth = 0.0f;
                    textBoxHeight += lineHeight;
                    continue;
                }
                if(text_[i] == ' '){
                    lineWidth += font_->getFontSize()*0.5f;
                    continue;
                }
                if(text_[i] == '�@'){
                    lineWidth += font_->getFontSize();
                    continue;
                }
                const Font::Glyph* glyph = font_->find(text_[i]);
                if(NULL == glyph){
                    continue;
                }
                lineWidth += glyph->xadvance_;
            }
            textBoxWidth = lcore::maximum(textBoxWidth, lineWidth) * fontToScreenX;
        }

        f32 top;
        f32 lineLeft;
        switch(verticalAlign_)
        {
        case Align_LeftTop:
            top = rect_.y_;
            break;
        case Align_Center:
            top = (rect_.y_ + rect_.w_*0.5f)*fontToScreenY - textBoxHeight*0.5f;
            break;
        default:
            top = (rect_.y_ + rect_.w_)*fontToScreenY - textBoxHeight;
            break;
        };

        switch(verticalAlign_)
        {
        case Align_LeftTop:
            lineLeft = rect_.x_;
            break;
        case Align_Center:
            lineLeft = (rect_.x_ + rect_.z_*0.5f)*fontToScreenX - textBoxWidth*0.5f;
            break;
        default:
            lineLeft = (rect_.x_ + rect_.z_)*fontToScreenX - textBoxWidth;
            break;
        };

        f32 lineWidth = calcLineWidth(text_.c_str()) * fontToScreenX;
        f32 left = lineLeft + (textBoxWidth-lineWidth)*0.5f;
        sprites_.reserve(text_.length());
        s32 count = 0;
        for(s32 i=0; i<text_.length(); ++i){
            if(text_[i] == lcore::CharLF){
                lineWidth = calcLineWidth(&text_[i+1]) * fontToScreenX;
                left = lineLeft + (textBoxWidth-lineWidth)*0.5f;
                top += lineHeight;
                continue;
            }
            if(text_[i] == ' '){
                left += fontToScreenX * font_->getFontSize()*0.5f;
                continue;
            }
            if(text_[i] == '�@'){
                left += fontToScreenX * font_->getFontSize();
                continue;
            }
            const Font::Glyph* glyph = font_->find(text_[i]);
            if(NULL == glyph){
                continue;
            }
            Sprite2D& sprite = sprites_[count];
            sprite.rect_.x_ = left + fontToScreenX * glyph->xoffset_;
            sprite.rect_.y_ = top + fontToScreenY * glyph->yoffset_;
            sprite.rect_.z_ = sprite.rect_.x_ + fontToScreenX * glyph->width_;
            sprite.rect_.w_ = sprite.rect_.y_ + fontToScreenY * glyph->height_;

            sprite.uv_[0] = glyph->uv_[0];
            sprite.uv_[1] = glyph->uv_[1];
            sprite.uv_[2] = glyph->uv_[2];
            sprite.uv_[3] = glyph->uv_[3];

            left += fontToScreenX * glyph->xadvance_;
            ++count;
        }
        sprites_.resize(count);
        canvas_->requestUpdate();
    }

    void ComponentUIText::calcTextBoxSize(f32& width, f32& height, const Char* text, f32 screenWidth, f32 screenHeight, Font* font)
    {
        LASSERT(NULL != text);
        LASSERT(NULL != font);
        f32 fontToScreenX = font->getInvWidth() * screenWidth;
        f32 fontToScreenY = font->getInvHeight() * screenHeight;
        f32 lineHeight = fontToScreenY * font->getLineHeight();
        f32 lineWidth = 0.0f;
        width = 0.0f;
        height = lineHeight;
        while(lcore::CharNull != *text){
            lcore::Char c = *text;
            ++text;
            if(c == lcore::CharLF){
                width = lcore::maximum(width, lineWidth);
                lineWidth = 0.0f;
                height += lineHeight;
                continue;
            }
            if(c == ' '){
                lineWidth += font->getFontSize()*0.5f;
                continue;
            }
            if(c == '�@'){
                lineWidth += font->getFontSize();
                continue;
            }
            const Font::Glyph* glyph = font->find(c);
            if(NULL == glyph){
                continue;
            }
            lineWidth += glyph->xadvance_;
        }
        width = lcore::maximum(width, lineWidth) * fontToScreenX;
    }

    f32 ComponentUIText::calcLineWidth(const Char* text) const
    {
        f32 width = 0.0f;
        while(lcore::CharNull != *text && lcore::CharLF != *text){
            lcore::Char c = *text;
            ++text;
            if(c == ' '){
                width += font_->getFontSize()*0.5f;
                continue;
            }
            if(c == '�@'){
                width += font_->getFontSize();
                continue;
            }
            const Font::Glyph* glyph = font_->find(c);
            if(NULL == glyph){
                continue;
            }
            width += glyph->xadvance_;
        }
        return width;
    }

    //f32 ComponentUIText::calcLineHeight(s32 fontSlot, f32 screenHeight)
    //{
    //    Resources& resources = System::getResources();
    //    Font* font = resources.getFontManager().get(fontSlot);
    //    if(NULL == font){
    //        return 0.0f;
    //    }
    //    f32 fontToScreenY = font->getInvHeight() * screenHeight;
    //    return fontToScreenY * font->getLineHeight();
    //}
}