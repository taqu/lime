/**
@file ComponentFontRenderer.cpp
@author t-sakai
@date 2016/12/25 create
*/
#include "ecs/ComponentFontRenderer.h"
#include <lmath/lmath.h>
#include "Application.h"
#include "ecs/ECSManager.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "render/Renderer.h"
#include "render/RenderQueue2D.h"
#include "render/RenderContext2D.h"
#include "resource/Font.h"
#include "resource/FontManager.h"
#include "resource/Resources.h"

namespace lfw
{
    ComponentFontRenderer::ComponentFontRenderer()
        :font_(NULL)
        ,left_(0.0f)
        ,top_(0.0f)
        ,scale_(1.0f)
        ,abgr_(0xFFFFFFFFU)
    {
    }

    ComponentFontRenderer::~ComponentFontRenderer()
    {
    }

    void ComponentFontRenderer::onCreate()
    {
    }

    void ComponentFontRenderer::onStart()
    {
    }

    void ComponentFontRenderer::update()
    {
    }

    void ComponentFontRenderer::onDestroy()
    {
    }

    void ComponentFontRenderer::addQueue(RenderQueue2D& queue)
    {
        if(NULL != font_ && 0<sprites_.size()) {
            const ComponentGeometric* geometric = getEntity ().getGeometric ();
            queue.add (geometric->getPosition ().z_, getMaterialId (), this);
        }
    }

    void ComponentFontRenderer::draw(RenderContext2D& context)
    {
        for(s32 i=0; i<sprites_.size(); ++i){
            context.addRect(sprites_[i].rect_, abgr_, sprites_[i].uv_, font_->getSRV(), *font_->getPSOutline());
        }
    }

    const Font* ComponentFontRenderer::getFont() const
    {
        return font_;
    }

    bool ComponentFontRenderer::setFont(s32 fontSlot)
    {
        Font* font = Resources::getInstance().getFontManager().get(fontSlot);
        if(NULL != font){
            font_ = font;
            return true;
        }
        return false;
    }

    s32 ComponentFontRenderer::getMaterialId ()
    {
        lcore::uintptr_t ptr = reinterpret_cast<lcore::uintptr_t>(font_->getTexture().get());
        return static_cast<s32>(ptr);
    }

    void ComponentFontRenderer::updateText()
    {
        if(NULL == font_){
            return;
        }

        RenderContext2D& context2D = Application::getInstance().getRenderer().getRenderContext2D();
        f32 fontToScreenX = font_->getInvWidth() * context2D.getScreenWidth() * scale_;
        f32 fontToScreenY = font_->getInvHeight() * context2D.getScreenHeight() * scale_;
        f32 left = left_;
        f32 top = top_;

        //LALIGN16 f32 boffset[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
        //LALIGN16 f32 bscale[4] = {2.0f*screen.invWidth_, -2.0f*screen.invHeight_, 2.0f*screen.invWidth_, -2.0f*screen.invHeight_};

        //lmath::lm128 offset = _mm_load_ps(boffset);
        //lmath::lm128 scale = _mm_load_ps(bscale);

        sprites_.resize(text_.length());
        s32 count = 0;
        for(s32 i=0; i<text_.length(); ++i){
            if(text_[i] == lcore::CharLF){
                left = left_;
                top += fontToScreenY * font_->getLineHeight();
                continue;
            }
            if(text_[i] == ' '){
                //TODO:‘SŠp
                left += fontToScreenX * font_->getFontSize()*0.5f;
                continue;
            }
            const Font::FontChar* fontChar = font_->find(text_[i]);
            if(NULL == fontChar){
                continue;
            }
            Sprite2D& sprite = sprites_[count];
            sprite.rect_.x_ = left + fontToScreenX * fontChar->xoffset_;
            sprite.rect_.y_ = top + fontToScreenY * fontChar->yoffset_;
            sprite.rect_.z_ = sprite.rect_.x_ + fontToScreenX * fontChar->width_;
            sprite.rect_.w_ = sprite.rect_.y_ + fontToScreenY * fontChar->height_;

            sprite.uv_[0] = fontChar->uv_[0];
            sprite.uv_[1] = fontChar->uv_[1];
            sprite.uv_[2] = fontChar->uv_[2];
            sprite.uv_[3] = fontChar->uv_[3];

            left += fontToScreenX * fontChar->xadvance_;
            ++count;
        }
        sprites_.resize(count);
    }

    f32 ComponentFontRenderer::calcTextWidth() const
    {
        if(NULL == font_){
            return 0.0f;
        }

        RenderContext2D& context2D = Application::getInstance().getRenderer().getRenderContext2D();
        f32 fontToScreenX = font_->getInvWidth() * context2D.getScreenWidth() * scale_;

        f32 width=0.0f;
        f32 lineWidth = 0.0f;
        for(s32 i=0; i<text_.length(); ++i){
            if(text_[i] == lcore::CharLF){
                width = lcore::maximum(width, lineWidth);
                lineWidth = 0.0f;
                continue;
            }
            if(text_[i] == ' '){
                //TODO:‘SŠp
                lineWidth += fontToScreenX * font_->getFontSize()*0.5f;
                continue;
            }
            const Font::FontChar* fontChar = font_->find(text_[i]);
            if(NULL == fontChar){
                continue;
            }
            lineWidth += fontToScreenX * fontChar->xadvance_;
        }
        return lcore::maximum(width, lineWidth);
    }

    f32 ComponentFontRenderer::calcLineHeight(s32 fontSlot)
    {
        Font* font = Resources::getInstance().getFontManager().get(fontSlot);
        if(NULL == font){
            return 0.0f;
        }
        RenderContext2D& context2D = Application::getInstance().getRenderer().getRenderContext2D();
        f32 fontToScreenY = font->getInvHeight() * context2D.getScreenWidth();
        return fontToScreenY * font->getLineHeight();
    }
}
