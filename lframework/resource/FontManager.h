#ifndef INC_LFRAMEWORK_FONTMANAGER_H_
#define INC_LFRAMEWORK_FONTMANAGER_H_
/**
@file FontManager.h
@author t-sakai
@date 2016/12/25 create
*/
#include "../lframework.h"

namespace lcore
{
    class FileProxy;
}

namespace lfw
{
    class Font;

    class FontManager
    {
    public:
        static const s32 MaxSlots = 8;

        FontManager();
        ~FontManager();

        bool load(s32 slot, const Char* path);

        Font* get(s32 index);

    private:
        FontManager(const FontManager&);
        FontManager& operator=(const FontManager&);

        static const u32 Identifier = 'FMB'|(3<<24);
        struct FmtInfo
        {
            //u8 id_;
            //u32 size_;

            s16 fontSize_;
            u8 bitField_;
            u8 charSet_;
            u16 stretchH_;
            u8 aa_;
            u8 paddingUp_;
            u8 paddingRight_;
            u8 paddingDown_;
            u8 paddingLeft_;
            u8 spacingHoriz_;
            u8 spacingVert_;
            u8 outline_;
            //Char* fontName_;//null terminated string
        };

        struct FmtCommon
        {
            //u8 id_;
            //u32 size_;
            u16 lineHeight_;
            u16 base_;
            u16 scaleW_;
            u16 scaleH_;
            u16 pages_;
            u8 bitField_;
            u8 alphaChnl_;
            u8 retChnl_;
            u8 greenChnl_;
            u8 blueChnl_;
        };

        struct FmtPages
        {
            //u8 id_;
            //u32 size_;
            //Char* pageNames_; //null terminated strings
        };

        struct FmtChar
        {
            u32 id_;
            u16 x_;
            u16 y_;
            u16 width_;
            u16 height_;
            s16 xoffset_;
            s16 yoffset_;
            s16 xadvance_;
            u8 page_;
            u8 chnl_;
        };
        struct FmtChars
        {
            //u8 id_;
            //u32 size_;
            //FmtChar* chars_;
        };

        struct FmtKerning
        {
            //u8 id_;
            //u32 size_;
            u32 first_;
            u32 second_;
            u16 amount_;
        };

        s64 readFmtInfo(FmtInfo& fmtInfo, s64 offset, lcore::FileProxy* file);
        s64 readFmtCommon(FmtCommon& fmtCommon, s64 offset, lcore::FileProxy* file);
        s64 readFmtPages(s64 offset, lcore::FileProxy* file);
        s64 readFmtChars(s32& numChars, FmtChar** chars, s64 offset, lcore::FileProxy* file);
        s64 readFmtKerning(s64 offset, lcore::FileProxy* file);
        s64 readFmtUnknown(s64 offset, lcore::FileProxy* file);

        Font* fonts_[MaxSlots];
    };
}
#endif //INC_LFRAMEWORK_FONTMANAGER_H_
