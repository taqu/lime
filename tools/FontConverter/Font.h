#ifndef INC_FONT_FONT_H__
#define INC_FONT_FONT_H__

namespace font
{
    typedef __int8  s8;
    typedef __int16 s16;
    typedef __int32 s32;

    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;

    typedef float f32;

    struct Header
    {
        s16 lineHeight_;
        s16 baseLine_;
        s16 width_;
        s16 height_;
        s32 numGlyphs_;
    };

    struct Glyph
    {
        u16 code_;
        s8 width_;
        s8 height_;

        u16 u0_;
        u16 v0_;
        u16 u1_;
        u16 v1_;
        s8 xoffset_;
        s8 yoffset_;
        s8 xadvance_;
        s8 reserved0_;
    };
}
#endif //INC_FONT_FONT_H__
