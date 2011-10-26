#ifndef INC_FONTPACKER_H__
#define INC_FONTPACKER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>

struct FontInfo
{
    std::string face_;
    int size_;
    int width_;
    int height_;
    int outline_;
    bool bold_;
};

struct PackHeader
{
    int textHeight_;
    unsigned int startCode_;
    unsigned int endCode_;
    int resolutionX_;
    int resolutionY_;
};

struct GlyphInfo
{
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
    static const unsigned int StartCode = 33;
    static const unsigned int EndCode = 126;
    static const unsigned int NumCode = EndCode - StartCode + 1;

    FontPacker();
    ~FontPacker();

    void create(HWND hwnd, const FontInfo& info);

    HBITMAP getBitmap(){ return bmp_;}

    void save(const char* bmppath, const char* infopath);
private:
    void release();
    void draw(HWND hwnd);

    void sort(GlyphInfo** info);

    HFONT font_;

    HBITMAP bmp_;
    HBITMAP bmpOutlined_;
    FontInfo info_;
    DWORD* dataBmp_;
    DWORD* dataOutlined_;

    short textHeight_;
    GlyphInfo glyphInfo_[NumCode];
};

#endif //INC_FONTPACKER_H__
