﻿/**
@file IOPNG.cpp
@author t-sakai
@date 2013/07/24 create
*/
#include <png.h>
#include <lcore/Array.h>

#include "io/IOPNG.h"
#include "Enumerations.h"
#include "TextureRef.h"

namespace lgfx
{
namespace io
{
    namespace
    {
        void read_callback(png_structp png_ptr, png_bytep data, png_size_t size)
        {
            lcore::istream* is = static_cast<lcore::istream*>(png_get_io_ptr(png_ptr));
            is->read(reinterpret_cast<char*>(data), static_cast<s32>(size));
        }
    }

    bool IOPNG::checkSignature(lcore::istream& is)
    {
        s32 pos = is.tellg();
        static const u32 SigSize = 8;
        u8 data[SigSize];
        u32 count = lcore::io::read(is, data, SigSize);
        is.seekg(pos, lcore::ios::beg);
        if(count<1){
            return false;
        }
        return (0 == png_sig_cmp(data, 0, SigSize));
    }

    bool IOPNG::read(
        lcore::istream& is,
        u8* buffer,
        bool sRGB,
        SwapRGB swap,
        u32& width, u32& height, lsize_t& rowBytes, DataFormat& format)
    {
        s32 startPos = is.tellg();

        //シグネチャチェック
        static const u32 SigSize = 8;
        u8 signature[SigSize];
        lcore::memset(signature, 0, SigSize);
        lcore::io::read(is, signature, SigSize);

        if(0 != png_sig_cmp(signature, 0, SigSize)){
            return false;
        }

        // 構造体初期化。
        //TODO:各コールバック設定
        png_structp png_ptr = png_create_read_struct(
            PNG_LIBPNG_VER_STRING,
            NULL,
            NULL,
            NULL);
        if(NULL == png_ptr){
            return false;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if(NULL == info_ptr){
            png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
            return false;
        }


        //後で使うデータ
        u8** rowPointers = NULL;

        // エラー時のジャンプ設定
        if(setjmp(png_jmpbuf(png_ptr))){
            LDELETE_ARRAY(rowPointers);

            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        }

        // 最初にロードしたサイズをセット
        png_set_sig_bytes(png_ptr, SigSize);

        // readコールバック
        png_set_read_fn(png_ptr, &is, read_callback);

        u32 result = 0;

        png_read_info(png_ptr, info_ptr);
        int bit_depth, color_type;
        int interlace, compression, filter;
        result = png_get_IHDR(
            png_ptr,
            info_ptr,
            static_cast<png_uint_32*>(&width),
            static_cast<png_uint_32*>(&height),
            &bit_depth,
            &color_type,
            &interlace, &compression, &filter);

        if(!result){
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        }

        png_set_strip_16(png_ptr);  //１６ビットを８ビットに変換
        png_set_palette_to_rgb(png_ptr); //パレットをRGBに変換
        png_set_packing(png_ptr); //1,2,4ビットの深度の場合、8bit/pixelに
        //png_set_swap_alpha(png_ptr); //BGRAをARGBに
        f64 gamma = 0.0;
        if(png_get_gAMA(png_ptr, info_ptr, &gamma)){
            png_set_gamma(png_ptr, 2.2, gamma);
        }

        //カラータイプチェック。テクスチャフォーマット
        format = Data_Unknown;
        bool swapBytes = false;
        switch(color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            format= Data_R8_UNorm;
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(png_ptr); //グレースケールをRGBに変換
            format= (swap == Swap_RGB)? Data_B8G8R8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm_SRGB;
            break;

        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            png_set_add_alpha(png_ptr, 0xFFU, PNG_FILLER_AFTER);
            format= (swap == Swap_RGB)? Data_B8G8R8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm_SRGB;
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_TYPE_RGB:
            png_set_add_alpha(png_ptr, 0xFFU, PNG_FILLER_AFTER);
            format= (swap == Swap_RGB)? Data_B8G8R8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm_SRGB;
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            format= (swap == Swap_BGR)? Data_B8G8R8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm_SRGB;
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_MASK_PALETTE:
        default:
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        };

        if(!sRGB){
            switch(format)
            {
            case Data_B8G8R8A8_UNorm_SRGB:
                format = Data_B8G8R8A8_UNorm;
                break;
            case Data_R8G8B8A8_UNorm_SRGB:
                format = Data_R8G8B8A8_UNorm;
                break;
            }
        }

        //png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        rowBytes = png_get_rowbytes(png_ptr, info_ptr);
        if(NULL == buffer){
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            is.seekg(startPos, lcore::ios::beg);
            return true;
        }

        // バッファ確保。各行ごとの先頭ポインタ配列作成
        rowPointers = LNEW u8*[height];

        for(u32 i=0; i<height; ++i){
            rowPointers[i] = buffer + rowBytes * i;
        }

        png_read_image(png_ptr, rowPointers);

        if(swapBytes){
            for(u32 i=0; i<height; ++i){
                for(u32 j=0; j<rowBytes; j+=4){
                    lcore::swap(rowPointers[i][j+0], rowPointers[i][j+2]);
                    //lcore::swap(rowPointers[i][j+3], rowPointers[i][j+1]);
                }
            }
        }

        LDELETE_ARRAY(rowPointers);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return true;
    }

    //---------------------------------------------------------------------------------------------------
    bool IOPNG::read(Texture2DRef& texture,
        lcore::istream& is,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        bool sRGB,
        SwapRGB swap,
        u32& width, u32& height, lsize_t& rowBytes, DataFormat& format)
    {
        if(false == read(is, NULL, sRGB, swap, width, height, rowBytes, format)){
            return false;
        }

        lsize_t size = rowBytes * height;
        u8* buffer = LNEW u8[size];

        if(false == read(is, buffer, sRGB, swap, width, height, rowBytes, format)){
            LDELETE_ARRAY(buffer);
            return false;
        }

        SubResourceData initData;
        initData.pitch_ = static_cast<u32>(rowBytes);
        initData.slicePitch_ = 0;
        initData.mem_ = buffer;

        texture = lgfx::Texture::create2D(
                width,
                height,
                1,
                1,
                format,
                usage,
                bindFlag,
                access,
                misc,
                &initData);

        LDELETE_ARRAY(buffer);
        return texture.valid();
    }
}
}