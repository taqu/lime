/**
@file IOPNG.cpp
@author t-sakai
@date 2015/09/23 create
*/
#include <png.h>
#include <lcore/Array.h>

#include "loader/IOPNG.h"
#include "core/Spectrum.h"

namespace lrender
{
    namespace
    {
        void read_callback(png_structp png_ptr, png_bytep data, png_size_t size)
        {
            lcore::istream* is = static_cast<lcore::istream*>(png_get_io_ptr(png_ptr));
            is->read(reinterpret_cast<char*>(data), size);
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

    bool IOPNG::read(lcore::istream& is, Color4* buffer, s32& width, s32& height, s32& rowBytes, SwapRGB swap)
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
            reinterpret_cast<png_uint_32*>(&width),
            reinterpret_cast<png_uint_32*>(&height),
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
        bool swapBytes = false;
        switch(color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            png_set_gray_to_rgb(png_ptr); //グレースケールをRGBに変換
            png_set_add_alpha(png_ptr, 0xFFU, PNG_FILLER_AFTER);
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(png_ptr); //グレースケールをRGBに変換
            break;

        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            png_set_add_alpha(png_ptr, 0xFFU, PNG_FILLER_AFTER);
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_TYPE_RGB:
            png_set_add_alpha(png_ptr, 0xFFU, PNG_FILLER_AFTER);
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            swapBytes = (swap == Swap_RGB);
            break;

        case PNG_COLOR_MASK_PALETTE:
        default:
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        };
        //png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        rowBytes = png_get_rowbytes(png_ptr, info_ptr);
        if(NULL == buffer){
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            is.seekg(startPos, lcore::ios::beg);
            return true;
        }

        // バッファ確保。各行ごとの先頭ポインタ配列作成
        u8* tmpBuffer = LNEW u8[width*height*4];
        rowPointers = LNEW u8*[height];

        for(s32 i=0; i<height; ++i){
            rowPointers[i] = tmpBuffer + rowBytes * (height-i-1);
        }

        png_read_image(png_ptr, rowPointers);

        if(swapBytes){
            for(s32 i=0; i<height; ++i){
                for(s32 j=0; j<rowBytes; j+=4){
                    lcore::swap(rowPointers[i][j+0], rowPointers[i][j+2]);
                    //lcore::swap(rowPointers[i][j+3], rowPointers[i][j+1]);
                }
            }
        }

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                Color4& rgba = buffer[index];
                index *= 4;
                rgba.setRGBA(tmpBuffer[index+0], tmpBuffer[index+1], tmpBuffer[index+2], tmpBuffer[index+3]);
            }
        }

        LDELETE_ARRAY(rowPointers);
        LDELETE_ARRAY(tmpBuffer);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return true;
    }
}
