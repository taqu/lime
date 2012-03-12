#include "stdafx.h"

#include "FontPacker.h"
#include "DistanceField.h"


FontPacker::FontPacker()
:font_(NULL)
,bmp_(NULL)
,bmpOutlined_(NULL)
,dataBmp_(NULL)
,dataOutlined_(NULL)
,textHeight_(0)
,spaceWidth_(0)
{
}

FontPacker::~FontPacker()
{
    release();
}

bool FontPacker::create(HWND hwnd, const FontInfo& info)
{
    release();

    info_ = info;

    bool ret = draw(hwnd);
    return ret;
}

void FontPacker::release()
{
    if(bmp_ != NULL){
        DeleteObject(bmp_);
        bmp_ = NULL;
        dataBmp_ = NULL;
    }else if(dataBmp_ != NULL){
        delete[] dataBmp_;
        dataBmp_ = NULL;
    }

    if(bmpOutlined_ != NULL){
        DeleteObject(bmpOutlined_);
        bmpOutlined_ = NULL;
        dataOutlined_ = NULL;
    }

    if(font_ != NULL){
        DeleteObject(font_);
        font_ = NULL;
    }
}


bool FontPacker::draw(HWND hwnd)
{
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = info_.width_;
    bmi.bmiHeader.biHeight = -info_.height_;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    bmi.bmiColors[0].rgbBlue = 0xFFU;
    bmi.bmiColors[0].rgbGreen = 0xFFU;
    bmi.bmiColors[0].rgbRed = 0xFFU;
    bmi.bmiColors[0].rgbReserved = 0;

    bmp_ = CreateDIBSection(
        NULL,
        &bmi,
        DIB_RGB_COLORS,
        (VOID**)&dataBmp_,
        NULL,
        0);

    bmpOutlined_ = CreateDIBSection(
        NULL,
        &bmi,
        DIB_RGB_COLORS,
        (VOID**)&dataOutlined_,
        NULL,
        0);

    if(NULL == bmp_ || NULL == bmpOutlined_){
        return false;
    }

    memset(dataBmp_, 0, sizeof(DWORD)*info_.width_*info_.height_);
    memset(dataOutlined_, 0, sizeof(DWORD)*info_.width_*info_.height_);

    int fontWeight = (info_.bold_)? FW_BOLD : FW_NORMAL;
    font_ = CreateFontA(
        info_.size_,
        0,
        0,
        0,
        fontWeight,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE|DEFAULT_PITCH,
        info_.face_.c_str());


    HDC memDC = NULL;
    HDC memDCOutlined = NULL;
    {
        HDC hdc = GetDC(hwnd);
        memDC = CreateCompatibleDC(hdc);
        memDCOutlined = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd, hdc);
    }

    HPEN pen = (HPEN)CreatePen(PS_SOLID, info_.outline_, RGB(255,255,255));
    HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));


    HBITMAP oldBMP = (HBITMAP)SelectObject(memDC, bmp_);
    HBITMAP oldBMPOutlined = (HBITMAP)SelectObject(memDCOutlined, bmpOutlined_);

    HFONT oldFont = (HFONT)SelectObject(memDC, font_);
    HFONT oldFontOutlined = (HFONT)SelectObject(memDCOutlined, font_);

    HPEN oldPen = (HPEN)SelectObject(memDC, pen);
    HPEN oldPenOutlined = (HPEN)SelectObject(memDCOutlined, pen);

    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);
    HBRUSH oldBrushOutlined = (HBRUSH)SelectObject(memDCOutlined, brush);

    SetBkMode(memDC, TRANSPARENT);
    SetBkMode(memDCOutlined, TRANSPARENT);

    TEXTMETRICA  metrics;
    GetTextMetricsA(memDC, &metrics);

    int outlineOffset = (info_.outline_+1) >> 1;
    int outlineWidth = outlineOffset << 1;
    textHeight_ = metrics.tmHeight + outlineWidth;

    char str[2] = {'\0', '\0'};

    int padding = outlineOffset+1;

    GLYPHMETRICS glyphMetrics;

    //2x2変換行列
    MAT2 mat=
    {
        {0,1}, {0,0},
        {0,0}, {0,1}
    };

    //空白サイズ
    GetGlyphOutlineA(memDC, 32, GGO_METRICS, &glyphMetrics, 0, NULL, &mat);
    spaceWidth_ = glyphMetrics.gmCellIncX + outlineWidth;

    GlyphInfo* tmpInfo[NumCode];
    for(unsigned int i=0; i<NumCode; ++i){
        unsigned int code = i + StartCode;
        GetGlyphOutlineA(memDC, code, GGO_METRICS, &glyphMetrics, 0, NULL, &mat);

        glyphInfo_[i].fitWCoded_ = code;
        glyphInfo_[i].width_ = glyphMetrics.gmCellIncX + outlineWidth;
        glyphInfo_[i].fitWidth_ = glyphMetrics.gmBlackBoxX + outlineWidth;
        glyphInfo_[i].fitHeight_ = glyphMetrics.gmBlackBoxY + outlineWidth;
        glyphInfo_[i].offsetX_ = glyphMetrics.gmptGlyphOrigin.x + outlineOffset;
        glyphInfo_[i].offsetY_ = metrics.tmAscent - glyphMetrics.gmptGlyphOrigin.y + outlineOffset;

        tmpInfo[i] = &(glyphInfo_[i]);
    }

    sort(tmpInfo);

    float ratioX = 32767.0f/info_.width_;
    float ratioY = 32767.0f/info_.height_;

    int x=padding;
    int y=padding;
    int prevHeight = tmpInfo[0]->fitHeight_;
    for(unsigned int i=0; i<NumCode; ++i){
        GlyphInfo& info = *tmpInfo[i];

        int ex = x + info.fitWidth_ + padding;
        if(ex>info_.width_){
            x = padding;
            y += prevHeight + padding;
            prevHeight = tmpInfo[i]->fitHeight_;
        }

        str[0] = info.fitWCoded_;

        //(0からテクスチャサイズ)を符号付16ビットにコード化
        info.fitWCoded_ = info.fitWidth_*ratioX;
        info.fitHCoded_ = info.fitHeight_*ratioY;

        info.x_ = x;
        info.y_ = y;

        int offsetx = x-info.offsetX_ + outlineOffset;
        int offsety = y-info.offsetY_ + outlineOffset;

        BeginPath(memDC);
        TextOutA(memDC, offsetx, offsety, str, 1);
        EndPath(memDC);
        FillPath(memDC);

        BeginPath(memDCOutlined);
        TextOutA(memDCOutlined, offsetx, offsety, str, 1);
        EndPath(memDCOutlined);
        StrokePath(memDCOutlined);

        x += info.fitWidth_ + padding;
    }


    SelectObject(memDC, oldBrush);
    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldFont);
    SelectObject(memDC, oldBMP);

    SelectObject(memDCOutlined, oldBrushOutlined);
    SelectObject(memDCOutlined, oldPenOutlined);
    SelectObject(memDCOutlined, oldFontOutlined);
    SelectObject(memDCOutlined, oldBMPOutlined);

    DeleteObject(brush);
    DeleteObject(pen);

    ReleaseDC(hwnd, memDC);
    ReleaseDC(hwnd, memDCOutlined);

    return true;
}


void FontPacker::sort(GlyphInfo** info)
{
    for(int i=0; i<NumCode; ++i){
        for(int j=i+1; j<NumCode; ++j){
            if(info[i]->fitHeight_ < info[j]->fitHeight_){
                std::swap(info[i], info[j]);
            }
        }
    }
}


void FontPacker::save(const char* bmppath, const char* infopath)
{
    saveBMP(bmppath);

    HANDLE file = CreateFile(infopath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(NULL != file){
        PackHeader header;
        header.textHeight_ = textHeight_;
        header.spaceWidth_ = spaceWidth_;
        header.startCode_ = StartCode;
        header.endCode_ = EndCode;
        header.resolutionX_ = info_.width_;
        header.resolutionY_ = info_.height_;
        header.distanceField_ = (info_.distanceField_)? 1 : 0;

        DWORD written = 0;
        WriteFile(file, &header, sizeof(PackHeader), &written, NULL);
        WriteFile(file, glyphInfo_, sizeof(GlyphInfo)*NumCode, &written, NULL);
        CloseHandle(file);
    }
}

void FontPacker::saveBMP(const char* bmppath)
{
    BITMAPINFOHEADER bmpih;
    memset(&bmpih, 0, sizeof(BITMAPINFOHEADER));

	bmpih.biSize = sizeof(BITMAPINFOHEADER);
	bmpih.biWidth  = info_.width_;
	bmpih.biHeight = -info_.height_;
    bmpih.biBitCount = 24;
	bmpih.biPlanes = 1;
	bmpih.biCompression = BI_RGB;
    bmpih.biSizeImage = 3*info_.width_*info_.height_;
	bmpih.biXPelsPerMeter = 0;
	bmpih.biYPelsPerMeter = 0;
	bmpih.biClrUsed = 0;
	bmpih.biClrImportant = 0;

    BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42U;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;

	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fileHeader.bfSize = bmpih.biSizeImage + fileHeader.bfOffBits;


    unsigned char* tmp = new unsigned char[bmpih.biSizeImage];

    for(int j=0; j<info_.height_; ++j){
        int y = j*info_.width_*3;
        unsigned char* v = tmp + y;
        for(int i=0; i<info_.width_; ++i){
            int index = j*info_.width_ + i;
            v[0] = static_cast<unsigned char>(dataOutlined_[index] & 0xFFU);
            v[1] = static_cast<unsigned char>(dataBmp_[index] & 0xFFU);
            v[2] = 0;
            v += 3;
        }
    }

    DWORD written = 0;

	HANDLE file = CreateFile(bmppath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(NULL != file){
        WriteFile(file, &fileHeader, sizeof(BITMAPFILEHEADER), &written, NULL);
        WriteFile(file, &bmpih, sizeof(BITMAPINFOHEADER), &written, NULL);

        WriteFile(file, tmp, bmpih.biSizeImage, &written, NULL);
        CloseHandle(file);
    }

    delete[] tmp;
}


