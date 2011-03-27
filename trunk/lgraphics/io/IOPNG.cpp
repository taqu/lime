/**
@file IOPNG.cpp
@author t-sakai
@date 2011/01/08
*/
#include <png.h>
#include <lcore/vector.h>

#include "../lgraphicsAPIInclude.h"

#include "IOPNG.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        void read_callback(png_structp png_ptr, png_bytep data, png_size_t size)
        {
            lcore::istream* is = reinterpret_cast<lcore::istream*>(png_get_io_ptr(png_ptr));
            is->read(reinterpret_cast<char*>(data), size);
        }
    }

    IOPNG::IOPNG()
    {
    }

    IOPNG::~IOPNG()
    {
    }

    bool IOPNG::read(lcore::istream& is, TextureRef& texture)
    {
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
        u8* buffer = NULL;
        u8** rowPointers = NULL;

        // エラー時のジャンプ設定
        if(setjmp(png_jmpbuf(png_ptr))){
            LIME_DELETE_ARRAY(buffer);
            LIME_DELETE_ARRAY(rowPointers);

            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        }

        // 最初にロードしたサイズをセット
        png_set_sig_bytes(png_ptr, SigSize);

        // readコールバック
        png_set_read_fn(png_ptr, &is, read_callback);

        u32 result = 0;

        png_read_info(png_ptr, info_ptr);
        png_uint_32 width, height;
        int bit_depth, color_type;
        int interlace, compression, filter;
        result = png_get_IHDR(
            png_ptr,
            info_ptr,
            &width, &height,
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
        png_set_swap_alpha(png_ptr); //BGRAをARGBに
        f64 gamma = 0.0;
        if(png_get_gAMA(png_ptr, info_ptr, &gamma)){
            png_set_gamma(png_ptr, 2.2, gamma);
        }

        //カラータイプチェック。テクスチャフォーマット
        BufferFormat format = Buffer_Unknown;
        switch(color_type)
        {
        case PNG_COLOR_TYPE_GRAY:
            format= Buffer_L8;
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(png_ptr); //グレースケールをRGBに変換
#if defined(LIME_GL)
            png_set_bgr(png_ptr); //RGBをBGRに
            format= Buffer_A8B8G8R8;
#else
            format= Buffer_A8R8G8B8;
#endif
            break;

        case PNG_COLOR_TYPE_PALETTE:
            format= Buffer_R8G8B8;
            break;

        case PNG_COLOR_TYPE_RGB:
            format= Buffer_R8G8B8;
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
#if defined(LIME_GL)
            png_set_bgr(png_ptr); //RGBをBGRに
            format= Buffer_A8B8G8R8;
#else
            format= Buffer_A8R8G8B8;
#endif
            break;

        case PNG_COLOR_MASK_PALETTE:
        default:
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            return false;
        };


        // バッファ確保。各行ごとの先頭ポインタ配列作成
        png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        buffer = LIME_NEW u8[rowbytes*height];
        rowPointers = LIME_NEW u8*[height];

        for(u32 i=0; i<height; ++i){
            rowPointers[i] = buffer + rowbytes * i;
        }

        png_read_image(png_ptr, rowPointers);


        texture = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
        texture.blit(0, buffer);

        LIME_DELETE_ARRAY(buffer);
        LIME_DELETE_ARRAY(rowPointers);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return true;
    }

}
}
