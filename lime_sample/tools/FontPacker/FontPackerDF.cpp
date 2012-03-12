/**
@file FontPackerDF.cpp
@author t-sakai
@date 2011/11/30 create
*/
#include "stdafx.h"

#include "FontPackerDF.h"

#include "DistanceField.h"

namespace
{
    int set(FontPackerDF::GlyphInfoDF& info, int spread)
    {
        int dx = 0;
        if(spread>info.offsetX_){
            dx = spread - info.offsetX_;
            info.offsetX_ = spread;
        }

        int dy = 0;
        if(spread>info.offsetY_){
            dy = spread - info.offsetY_;
            info.offsetY_ = spread;
        }

        info.dx_ = static_cast<short>(dx);
        info.dy_ = static_cast<short>(dy);

        int dw = info.offsetX_ + info.fitWidth_ + spread;
        if(dw>info.width_){
            info.width_ = dw;
        }

        int dh = info.offsetY_ + info.fitHeight_ + spread;
        return dh;
    }

    void set(GlyphInfo& info, FontPackerDF::GlyphInfoDF& infoDF, float invScale)
    {
        info.offsetX_ = 0;//static_cast<char>(infoDF.offsetX_ * invScale);
        info.offsetY_ = 0;//static_cast<char>(infoDF.offsetY_ * invScale);
        //info.fitWidth_ = static_cast<short>(infoDF.fitWidth_ * invScale);
        //info.fitHeight_ = static_cast<short>(infoDF.fitHeight_ * invScale);

        info.width_ = static_cast<short>( floor(infoDF.width_ * invScale) );
    }
}

FontPackerDF::FontPackerDF()
:font_(NULL)
,bmp_(NULL)
,dataBmp_(NULL)
,textHeight_(0)
,spaceWidth_(0)
{
}

FontPackerDF::~FontPackerDF()
{
    release();
}

bool FontPackerDF::create(HWND hwnd, const FontInfo& info)
{
    release();

    info_ = info;

    bool ret = false;
    ret = createGlyphInfos(hwnd);
        ret = drawScaled(hwnd);
    return ret;
}

void FontPackerDF::release()
{
    if(bmp_ != NULL){
        DeleteObject(bmp_);
        bmp_ = NULL;
        dataBmp_ = NULL;
    }else if(dataBmp_ != NULL){
        delete[] dataBmp_;
        dataBmp_ = NULL;
    }

    if(font_ != NULL){
        DeleteObject(font_);
        font_ = NULL;
    }
}


bool FontPackerDF::createGlyphInfos(HWND hwnd)
{
    int fontWeight = (info_.bold_)? FW_BOLD : FW_NORMAL;
    font_ = CreateFontA(
        info_.size_ * info_.distanceScale_,
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
        NONANTIALIASED_QUALITY,
        FF_DONTCARE|DEFAULT_PITCH,
        info_.face_.c_str());


    HDC memDC = NULL;
    {
        HDC hdc = GetDC(hwnd);
        memDC = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd, hdc);
    }

    HFONT oldFont = (HFONT)SelectObject(memDC, font_);

    SetBkMode(memDC, TRANSPARENT);
    TEXTMETRICA  metrics;
    GetTextMetricsA(memDC, &metrics);

    textHeight_ = metrics.tmHeight;

    GLYPHMETRICS glyphMetrics;

    //2x2変換行列
    MAT2 mat=
    {
        {0,1}, {0,0},
        {0,0}, {0,1}
    };

    //空白サイズ
    GetGlyphOutlineA(memDC, 32, GGO_METRICS, &glyphMetrics, 0, NULL, &mat);
    spaceWidth_ = static_cast<short>( floor(glyphMetrics.gmCellIncX * (1.0f/info_.distanceScale_)) );

    int spread = (info_.distanceScale_<info_.distanceSpread_)? info_.distanceSpread_ : info_.distanceScale_;

    for(unsigned int i=0; i<NumCode; ++i){
        unsigned int code = i + StartCode;
        GetGlyphOutlineA(memDC, code, GGO_METRICS, &glyphMetrics, 0, NULL, &mat);

        glyphInfoDF_[i].code_ = code;

        glyphInfoDF_[i].width_ = glyphMetrics.gmCellIncX;
        glyphInfoDF_[i].fitWidth_ = glyphMetrics.gmBlackBoxX;
        glyphInfoDF_[i].fitHeight_ = glyphMetrics.gmBlackBoxY;
        glyphInfoDF_[i].offsetX_ = glyphMetrics.gmptGlyphOrigin.x;
        glyphInfoDF_[i].offsetY_ = metrics.tmAscent - glyphMetrics.gmptGlyphOrigin.y;

        int h = set(glyphInfoDF_[i], spread);

        if(h>textHeight_){
            textHeight_ = h;
        }
    }

    SelectObject(memDC, oldFont);

    ReleaseDC(hwnd, memDC);

    return true;
}


bool FontPackerDF::drawScaled(HWND hwnd)
{
    int tmpHeight = textHeight_ * 2;

    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = tmpHeight;
    bmi.bmiHeader.biHeight = -tmpHeight;
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

    DWORD* tmpBmp = NULL;

    HBITMAP bmp = CreateDIBSection(
        NULL,
        &bmi,
        DIB_RGB_COLORS,
        (VOID**)&tmpBmp,
        NULL,
        0);

    if(NULL == bmp){
        return false;
    }

    dataBmp_ = new DWORD[info_.width_*info_.height_];
    memset(dataBmp_, 0, sizeof(DWORD)*info_.width_*info_.height_);

    int fontWeight = (info_.bold_)? FW_BOLD : FW_NORMAL;
    //スケールしたフォント作成
    if(NULL == font_){
        font_ = CreateFontA(
            info_.size_*info_.distanceScale_,
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
            NONANTIALIASED_QUALITY, //距離場の場合はアンチエイリアスなし
            FF_DONTCARE|DEFAULT_PITCH,
            info_.face_.c_str());
    }

    HDC memDC = NULL;
    {
        HDC hdc = GetDC(hwnd);
        memDC = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd, hdc);
    }

    HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));

    HPEN pen = (HPEN)CreatePen(PS_SOLID, info_.outline_, RGB(255,255,255));


    HBITMAP oldBMP = (HBITMAP)SelectObject(memDC, bmp);

    HFONT oldFont = (HFONT)SelectObject(memDC, font_);

    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);

    HPEN oldPen = (HPEN)SelectObject(memDC, pen);

    SetBkMode(memDC, TRANSPARENT);

    TEXTMETRICA  metrics;
    GetTextMetricsA(memDC, &metrics);

    char str[2] = {'\0', '\0'};

    float ratioX = 32767.0f/info_.width_;
    float ratioY = 32767.0f/info_.height_;
    
    int padding = 1;
    int x = padding;
    int y = padding;
    int offset = info_.distanceScale_ / 2;
    float invScale = 1.0f/info_.distanceScale_;
    float source_spread = static_cast<float>(info_.distanceSpread_ * info_.distanceScale_);

    textHeight_ = static_cast<short>( floor(textHeight_ * invScale) );

    for(unsigned int i=0; i<NumCode; ++i){
        GlyphInfo& info = glyphInfo_[i];
        GlyphInfoDF& infoDF = glyphInfoDF_[i];

        //一時バッファに文字を描画
        {
            memset(tmpBmp, 0, sizeof(DWORD)*tmpHeight*tmpHeight);

            str[0] = i + StartCode;

            BeginPath(memDC);
            TextOutA(memDC, infoDF.dx_, infoDF.dy_, str, 1);
            EndPath(memDC);
            FillPath(memDC);
        }


        set(info, infoDF, invScale);
        info.fitWidth_ = info.width_;
        info.fitHeight_ = textHeight_;

        if((x+info.width_)>info_.width_){
            x = padding;
            y += padding + textHeight_;
            if((y+textHeight_)>info_.height_){
                break;
            }
        }

        info.x_ = x;
        info.y_ = y;
        info.fitWCoded_ = static_cast<short>(ratioX * info.fitWidth_);
        info.fitHCoded_ = static_cast<short>(ratioY * info.fitHeight_);

        for(int j=0; j<textHeight_; ++j){
            for(int k=0; k<info.width_; ++k){
                int tx = k*info_.distanceScale_ + offset;
                int ty = j*info_.distanceScale_ + offset;

                float sd = signedDistance(tx, ty, source_spread, tmpHeight, tmpHeight, tmpBmp);
                unsigned int enc = static_cast<unsigned int>(255.0f*(sd + source_spread)/(source_spread*2.0f));

                tx = info.x_ + k;
                ty = info.y_ + j;
                unsigned int index = ty * info_.width_ + tx;
                dataBmp_[index] = min(enc, 255);
            }
        }

        x += info.width_ + padding;
    }


    textHeight_ = static_cast<short>( floor(metrics.tmHeight * invScale) );

    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldBrush);
    SelectObject(memDC, oldFont);
    SelectObject(memDC, oldBMP);

    DeleteObject(brush);
    DeleteObject(pen);

    if(bmp != NULL){
        DeleteObject(bmp);
        bmp = NULL;
        tmpBmp = NULL;
    }
    ReleaseDC(hwnd, memDC);

    return true;
}



void FontPackerDF::sort(GlyphInfo** info)
{
    for(int i=0; i<NumCode; ++i){
        for(int j=i+1; j<NumCode; ++j){
            if(info[i]->fitHeight_ < info[j]->fitHeight_){
                std::swap(info[i], info[j]);
            }
        }
    }
}


void FontPackerDF::save(const char* bmppath, const char* infopath)
{
    saveDistanceFieldBMP(bmppath);

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


void FontPackerDF::saveDistanceFieldBMP(const char* bmppath)
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


    float source_spread = static_cast<float>(info_.distanceSpread_ * info_.distanceScale_);
    int offset = info_.distanceScale_ / 2;
    int scaleW = info_.width_ * info_.distanceScale_;
    int scaleH = info_.height_ * info_.distanceScale_;

    for(int j=0; j<info_.height_; ++j){
        int y = j*info_.height_*3;
        unsigned char* v = tmp + y;

        for(int i=0; i<info_.width_; ++i){

            v[0] = static_cast<unsigned char>(dataBmp_[j*info_.width_ + i] & 0xFFU);
            v[1] = v[0];
            v[2] = v[0];
            v+=3;
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

