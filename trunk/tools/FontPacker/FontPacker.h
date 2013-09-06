#ifndef INC_FONTPACKER_H__
#define INC_FONTPACKER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include "charcode/sjis1.h"

inline bool isTwoBytes(const char* s)
{
    unsigned char c = *((unsigned char*)&(s[0]));
    if((0x81U<=c && c<=0x9FU)
        || (0xE0U<=c && c<=0xFCU))
    {
        c = *((unsigned char*)&(s[1]));
        if((0x40U<=c && c<=0xFCU)){
            return true;
        }
    }
    return false;

}

inline int SJISToU16(unsigned short& code, const char* s)
{
    unsigned char c = *((unsigned char*)&(s[0]));
    code = c;

    if((0x81U<=c && c<=0x9FU)
        || (0xE0U<=c && c<=0xFCU))
    {
        c = *((unsigned char*)&(s[1]));
        if((0x40U<=c && c<=0xFCU)){
            unsigned short code2 = c;
            code |= (code2<<8);
            return 2;
        }
    }
    return 1;
}

inline int U16ToSTR(unsigned short& dst, unsigned short code)
{
    char* str = (char*)&dst;

    str[1] = code & 0xFFU;
    str[0] = (code>>8) & 0xFFU;

    if(isTwoBytes(str)){
        return 2;
    }else{
        str[0] = code & 0xFFU;
        str[1] = 0;
        return 1;
    }
}

struct FontInfo
{
    std::string face_;

    int size_;
    int width_;
    int height_;
    int outline_;
    bool bold_;
    bool distanceField_;
    bool asciiOnly_;
    bool dummy1_;
    int distanceScale_;
    int distanceSpread_;
};

struct PackHeader
{
    int textHeight_;
    int spaceWidth_;
    int numCodes_;
    int resolutionX_;
    int resolutionY_;
    int distanceField_;
};

struct GlyphInfo
{
    unsigned int code_;
    short x_;
    short y_;
    short width_;
    short fitWidth_;
    short fitHeight_;
    short fitWCoded_;
    short fitHCoded_;
    char offsetX_;
    char offsetY_;
};

class FontPacker
{
public:
    static const unsigned int NumCode = charcode::NumEntries;

    FontPacker();
    ~FontPacker();

    bool create(HWND hwnd, const FontInfo& info);

    HBITMAP getBitmap(){ return bmp_;}

    void save(const TCHAR* bmppath, const TCHAR* infopath);
private:
    void release();
    bool draw(HWND hwnd);

    void saveBMP(const TCHAR* bmppath);

    void sort(GlyphInfo** info);

    HFONT font_;

    HBITMAP bmp_;
    HBITMAP bmpOutlined_;
    FontInfo info_;
    DWORD* dataBmp_;
    DWORD* dataOutlined_;

    unsigned int numCodes_;

    short textHeight_;
    short spaceWidth_;
    GlyphInfo glyphInfo_[NumCode];
};

#endif //INC_FONTPACKER_H__
