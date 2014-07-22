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
    void set(FontPackerDF::GlyphInfoDF& info, int spread)
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
    }

    void set(GlyphInfo& info, FontPackerDF::GlyphInfoDF& infoDF, float invScale)
    {
        info.offsetX_ = 0;//static_cast<char>(infoDF.offsetX_ * invScale);
        info.offsetY_ = 0;//static_cast<char>(infoDF.offsetY_ * invScale);
        //info.fitWidth_ = static_cast<short>(infoDF.fitWidth_ * invScale);
        //info.fitHeight_ = static_cast<short>(infoDF.fitHeight_ * invScale);

        info.width_ = static_cast<short>( floor(infoDF.width_ * invScale) );
    }

    DWORD getGlyphMetrics(GLYPHMETRICS& metrics, HDC hdc, unsigned short code)
    {
        //2x2変換行列
        MAT2 mat=
        {
            {0,1}, {0,0},
            {0,0}, {0,1}
        };
        return GetGlyphOutline(hdc, code, GGO_METRICS, &metrics, 0, NULL, &mat);
    }

    HBITMAP createBMP(int size, DWORD** data)
    {
        BITMAPINFO bmi;
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = size;
        bmi.bmiHeader.biHeight = -size;
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
            (VOID**)data,
            NULL,
            0);
        return bmp;
    }
}

FontPackerDF::FontPackerDF()
:font_(NULL)
,bmp_(NULL)
,dataBmp_(NULL)
,numCodes_(0)
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
    numCodes_ = (info_.asciiOnly_)? NumASCII : NumTableEntries;

    bool ret = false;
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

bool FontPackerDF::drawScaled(HWND hwnd)
{
    dataBmp_ = new DWORD[info_.width_*info_.height_];
    memset(dataBmp_, 0, sizeof(DWORD)*info_.width_*info_.height_);

    int fontWeight = (info_.bold_)? FW_BOLD : FW_NORMAL;
    //スケールしたフォント作成
    if(NULL == font_){
        font_ = CreateFont(
            info_.size_*info_.distanceScale_,
            0,
            0,
            0,
            fontWeight,
            FALSE,
            FALSE,
            FALSE,
            SHIFTJIS_CHARSET,
            OUT_DEFAULT_PRECIS,//OUT_TT_ONLY_PRECIS,
            CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY,//NONANTIALIASED_QUALITY, //距離場の場合はアンチエイリアスなし
            DEFAULT_PITCH | FF_DONTCARE, //FIXED_PITCH|FF_MODERN,
            info_.face_.c_str());
    }

    HDC memDC = NULL;
    {
        HDC hdc = GetDC(hwnd);
        memDC = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd, hdc);
    }

    HFONT oldFont = (HFONT)SelectObject(memDC, font_);

    SetBkMode(memDC, TRANSPARENT);

    TEXTMETRIC  metrics;
    GetTextMetrics(memDC, &metrics);

    int spread = (info_.distanceScale_<info_.distanceSpread_)? info_.distanceSpread_ : info_.distanceScale_;
    textHeight_ =  metrics.tmHeight + spread;

    int tmpHeight = textHeight_*2;
    DWORD* tmpBmp = NULL;
    HBITMAP bmp = createBMP(tmpHeight, &tmpBmp);
    if(NULL == bmp){
        return false;
    }


    HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));

    HPEN pen = (HPEN)CreatePen(PS_SOLID, info_.outline_, RGB(255,255,255));

    HBITMAP oldBMP = (HBITMAP)SelectObject(memDC, bmp);

    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);

    HPEN oldPen = (HPEN)SelectObject(memDC, pen);

    GLYPHMETRICS glyphMetrics;

    //空白サイズ
    getGlyphMetrics(glyphMetrics, memDC, ' ');
    spaceWidth_ = static_cast<short>( floor(glyphMetrics.gmCellIncX * (1.0f/info_.distanceScale_)) );

    for(unsigned int i=0; i<numCodes_; ++i){
        
        unsigned short code;
        U16ToSTR(code, charcode::SJIS_LEVEL1[i].code_);
        if(GDI_ERROR == getGlyphMetrics(glyphMetrics, memDC, charcode::SJIS_LEVEL1[i].code_)){
            glyphInfoDF_[i].code_ = 0;
            continue;
        }

        glyphInfoDF_[i].code_ = code;

        glyphInfoDF_[i].width_ = glyphMetrics.gmCellIncX;
        glyphInfoDF_[i].fitWidth_ = glyphMetrics.gmBlackBoxX;
        glyphInfoDF_[i].fitHeight_ = glyphMetrics.gmBlackBoxY;
        glyphInfoDF_[i].offsetX_ = glyphMetrics.gmptGlyphOrigin.x;
        glyphInfoDF_[i].offsetY_ = metrics.tmAscent - glyphMetrics.gmptGlyphOrigin.y;

        set(glyphInfoDF_[i], spread);
    }

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

    for(unsigned int i=0; i<numCodes_; ++i){
        GlyphInfo& info = glyphInfo_[i];
        GlyphInfoDF& infoDF = glyphInfoDF_[i];

        //一時バッファに文字を描画
        {
            memset(tmpBmp, 0, sizeof(DWORD)*tmpHeight*tmpHeight);

            BeginPath(memDC);

            str[0] = infoDF.code_ & 0xFFU;
            str[1] = (infoDF.code_>>8) & 0xFFU;

            if(isTwoBytes(str)){
                TextOut(memDC, infoDF.dx_, infoDF.dy_, str, 2);
            }else{
                TextOut(memDC, infoDF.dx_, infoDF.dy_, str, 1);
            }

            EndPath(memDC);
            FillPath(memDC);
        }


        set(info, infoDF, invScale);
        info.code_ = infoDF.code_;
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
    for(int i=0; i<numCodes_; ++i){
        for(int j=i+1; j<numCodes_; ++j){
            if(info[i]->fitHeight_ < info[j]->fitHeight_){
                std::swap(info[i], info[j]);
            }
        }
    }
}


void FontPackerDF::save(const TCHAR* bmppath, const TCHAR* infopath)
{
    saveDistanceFieldBMP(bmppath);

    HANDLE file = CreateFile(infopath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(NULL != file){
        PackHeader header;
        header.textHeight_ = textHeight_;
        header.spaceWidth_ = spaceWidth_;
        //header.startCode_ = StartCode;
        //header.endCode_ = EndCode;
        header.numCodes_ = numCodes_;
        header.resolutionX_ = info_.width_;
        header.resolutionY_ = info_.height_;
        header.distanceField_ = (info_.distanceField_)? 1 : 0;

        DWORD written = 0;
        WriteFile(file, &header, sizeof(PackHeader), &written, NULL);
        WriteFile(file, glyphInfo_, sizeof(GlyphInfo)*numCodes_, &written, NULL);
        CloseHandle(file);
    }
}


void FontPackerDF::saveDistanceFieldBMP(const TCHAR* bmppath)
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

