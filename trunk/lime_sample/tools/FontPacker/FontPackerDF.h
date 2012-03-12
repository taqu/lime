#ifndef INC_FONTPACKERDF_H__
#define INC_FONTPACKERDF_H__
/**
@file FontPackerDF.h
@author t-sakai
@date 2011/11/30 create
*/
#include "FontPacker.h"

class FontPackerDF
{
public:
    static const unsigned int StartCode = 33;
    static const unsigned int EndCode = 126;
    static const unsigned int NumCode = EndCode - StartCode + 1;

    struct GlyphInfoDF
    {
        short width_;
        short fitWidth_;
        short fitHeight_;
        char offsetX_;
        char offsetY_;
        short dx_;
        short dy_;
        unsigned int code_;
    };

    FontPackerDF();
    ~FontPackerDF();

    bool create(HWND hwnd, const FontInfo& info);

    HBITMAP getBitmap(){ return bmp_;}

    void save(const char* bmppath, const char* infopath);
private:
    void release();

    bool createGlyphInfos(HWND hwnd);

    bool drawScaled(HWND hwnd);

    void saveDistanceFieldBMP(const char* bmppath);

    void sort(GlyphInfo** info);

    HFONT font_;

    HBITMAP bmp_;
    FontInfo info_;
    DWORD* dataBmp_;

    short textHeight_;
    short spaceWidth_;
    GlyphInfo glyphInfo_[NumCode];
    GlyphInfoDF glyphInfoDF_[NumCode];
};

#endif //INC_FONTPACKERDF_H__
