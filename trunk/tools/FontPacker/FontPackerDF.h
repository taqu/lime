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
    static const unsigned int NumTableEntries = charcode::NumEntries;
    static const unsigned int NumASCII = 158;

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

    void save(const TCHAR* bmppath, const TCHAR* infopath);
private:
    void release();

    bool drawScaled(HWND hwnd);

    void saveDistanceFieldBMP(const TCHAR* bmppath);

    void sort(GlyphInfo** info);

    HFONT font_;

    HBITMAP bmp_;
    FontInfo info_;
    DWORD* dataBmp_;

    unsigned int numCodes_;

    short textHeight_;
    short spaceWidth_;
    GlyphInfo glyphInfo_[NumTableEntries];
    GlyphInfoDF glyphInfoDF_[NumTableEntries];

    int outGlyphCount_;
};

#endif //INC_FONTPACKERDF_H__
