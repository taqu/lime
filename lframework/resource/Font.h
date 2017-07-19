#ifndef INC_LFRAMEWORK_FONT_H__
#define INC_LFRAMEWORK_FONT_H__
/**
@file Font.h
@author t-sakai
@date 2016/12/25 create
*/
#include "../lframework.h"
#include <lgraphics/TextureRef.h>

namespace lgfx
{
    class VertexShaderRef;
    class PixelShaderRef;
}

namespace lfw
{
    class Font
    {
    public:
        struct Glyph
        {
            u16 id_;
            s16 xadvance_;
            u16 uv_[4]; //left, right, top, bottom
            s16 xoffset_;
            s16 yoffset_;
            u16 width_;
            u16 height_;
        };

        Font();
        ~Font();

        const Glyph* find(u16 id) const;
        inline lgfx::PixelShaderRef* getPS();
        inline lgfx::PixelShaderRef* getPSOutline();
        inline lgfx::Texture2DRef& getTexture();
        inline lgfx::ShaderResourceViewRef& getSRV();

        inline s16 getFontSize() const;
        inline s16 getLineHeight() const;
        inline u16 getWidth() const;
        inline u16 getHeight() const;
        inline f32 getInvWidth() const;
        inline f32 getInvHeight() const;
        inline s32 getNumGlyphes() const;
        inline const Glyph& getGlyph(s32 index) const;

        inline static bool less(const Glyph& lhs, const Glyph& rhs)
        {
            return (lhs.id_<rhs.id_);
        }

        inline static s32 comp(const Glyph& x, const u16& id)
        {
            return (x.id_<id)? -1 : x.id_-id;
        }
    private:
        Font(const Font&);
        Font& operator=(const Font&);

        friend class FontManager;

        s16 fontSize_;
        s16 lineHeight_;
        u16 width_;
        u16 height_;
        f32 invWidth_;
        f32 invHeight_;
        s32 numGlyphes_;
        Glyph* glyphes_;

        lgfx::PixelShaderRef* ps_;
        lgfx::PixelShaderRef* psOutline_;
        lgfx::Texture2DRef texture_;
        lgfx::ShaderResourceViewRef srv_;
    };

    inline lgfx::PixelShaderRef* Font::getPS()
    {
        return ps_;
    }

    inline lgfx::PixelShaderRef* Font::getPSOutline()
    {
        return psOutline_;
    }

    inline lgfx::Texture2DRef& Font::getTexture()
    {
        return texture_;
    }

    inline lgfx::ShaderResourceViewRef& Font::getSRV()
    {
        return srv_;
    }

    inline s16 Font::getFontSize() const
    {
        return fontSize_;
    }

    inline s16 Font::getLineHeight() const
    {
        return lineHeight_;
    }

    inline u16 Font::getWidth() const
    {
        return width_;
    }

    inline u16 Font::getHeight() const
    {
        return height_;
    }

    inline f32 Font::getInvWidth() const
    {
        return invWidth_;
    }

    inline f32 Font::getInvHeight() const
    {
        return invHeight_;
    }

    inline s32 Font::getNumGlyphes() const
    {
        return numGlyphes_;
    }

    inline const Font::Glyph& Font::getGlyph(s32 index) const
    {
        LASSERT(0<=index && index<numGlyphes_);
        return glyphes_[index];
    }
}
#endif //INC_LFRAMEWORK_FONTMANAGER_H__
