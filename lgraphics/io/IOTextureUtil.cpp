/**
@file IOTextureUtil.cpp
@author t-sakai
@date 2011/04/30
*/
#include "IOTextureUtil.h"
#include <lmath/lmathcore.h>
#include <lgraphics/api/TextureRef.h>

namespace lgraphics
{
namespace io
{
    namespace
    {
        static const f32 InvLog2 = (1.44269504f);

        /**
        @brief 縮小する
        @param dst
        @param src
        @param fW
        @param fH
        @param fNW
        @param fNH
        @param bpp

        拡大率は1/2より小さくはない
        */
        void scaleDownToHalfSize(u8* dst, const u8* src, f32 fW, f32 fH, f32 fNW, f32 fNH, u32 bpp)
        {
            LASSERT(0<bpp && bpp<=4);
            u32 w = static_cast<u32>(fW);
            //u32 h = static_cast<u32>(fH);
            u32 nw = static_cast<u32>(fNW);
            u32 nh = static_cast<u32>(fNH);


            f32 ratioW = fW/fNW;
            f32 ratioH = fH/fNH;
            f32 invRatioW = fNW/fW;
            f32 invRatioH = fNH/fH;


            f32 fx, fy;
            f32 y0, y1, x0, x1;
            s32 sy0, sy1, sx0, sx1;
            f32 value[4];
            f32 r[4];
            for(u32 j=0; j<nh; ++j){
                fy = ratioH * j;            //元画像座標へ変換
                sy0 = static_cast<s32>(fy); //画素１
                sy1 = sy0+1;                //画素２
                y0 = (1.0f - fy + sy0) * invRatioH; //混合比率１
                y1 = 1.0f - y0;             //混合比率２

                for(u32 i=0; i<nw; ++i){
                    fx = ratioW * i;            //元画像座標へ変換
                    sx0 = static_cast<s32>(fx); //画素１
                    sx1 = sx0+1;                //画素２
                    x0 = (1.0f - fx + sx0) * invRatioW; //混合比率１
                    x1 = 1.0f - x0;             //混合比率２

                    //比率をかけて４ピクセルを混合
                    //------------------------------------
                    u32 p0 = (sy0 * w + sx0) * bpp;
                    u32 p1 = p0 + bpp;

                    r[0] = x0*y0;
                    r[1] = x1*y0;

                    for(u32 k=0; k<bpp; ++k){
                        value[k] = src[p0 + k] * r[0];
                        value[k] += src[p1 + k] * r[1];
                    }

                    p0 += w * bpp;
                    p1 = p0 + bpp;

                    r[0] = x0*y1;
                    r[1] = x1*y1;
                    for(u32 k=0; k<bpp; ++k){
                        value[k] += src[p0 + k] * r[0];
                        value[k] += src[p1 + k] * r[1];
                    }

                    //出力
                    //-------------------------------------
                    u32 dp = (j*nw + i)*bpp;
                    for(u32 k=0; k<bpp; ++k){
                        dst[dp + k] = static_cast<u8>( value[k] );
                    }

                }// for(u32 i=0;
            }// for(u32 j=0;

        }// scaleDownToHalfSize
    }

    //-----------------------------------------------------
    // 2のべき乗のサイズに変換
    void convertToPow2Image(u8** ppBuffer, u32& width, u32& height, BufferFormat format)
    {
        static const f32 Epsilon = 0.0001f;

        LASSERT(width>0);
        LASSERT(height>0);

        f32 w = static_cast<f32>(width);
        f32 h = static_cast<f32>(height);
        f32 log2_w = lmath::log( w ) * InvLog2;
        f32 log2_h = lmath::log( h ) * InvLog2;

        log2_w = lmath::floor(log2_w);
        log2_h = lmath::floor(log2_h);

        f32 nw = lmath::pow(2.0f, log2_w);
        f32 nh = lmath::pow(2.0f, log2_h);

        if(lmath::isEqual(nw, w)
            && lmath::isEqual(nh, h))
        {
            return;
        }
        u32 bpp = calcBpp(format);

        width = static_cast<u32>(nw);
        height = static_cast<u32>(nh);
        u8* newBuffer = LIME_NEW u8[width*height*bpp];

        scaleDownToHalfSize(newBuffer, *ppBuffer, w, h, nw, nh, bpp);
        LIME_DELETE_ARRAY(*ppBuffer);
        *ppBuffer = newBuffer;
    }


    //-----------------------------------------------------
    // 半分のサイズのテクスチャ作成
    void createHalfSizeImage(u8* dst, u32 dw, u32 dh, const u8* src, u32 sw, u32 sh, u32 bpp)
    {
        LASSERT(dw>0);
        LASSERT(dh>0);
        LASSERT(sw>0);
        LASSERT(sh>0);
        LASSERT(0<bpp && bpp<=4);

        u32 ratio[2];
        ratio[0] = (sw>=2)? 2 : 1;
        ratio[1] = (sh>=2)? 2 : 1;

        u32 offset[2];
        offset[0] = (sw>=2)? 1 : 0;
        offset[1] = (sh>=2)? 1 : 0;

        u32 pitch = sw * bpp;
        u32 y0, x0;
        u32 value[4];
        for(u32 j=0; j<dh; ++j){
            y0 = j * ratio[1];

            for(u32 i=0; i<dw; ++i){
                x0 = i * ratio[0];

                u32 p0 = y0*pitch + x0*bpp;
                u32 p1 = p0 + offset[0]*bpp;

                for(u32 k=0; k<bpp; ++k){
                    value[k] = src[p0+k];
                    value[k] += src[p1+k];
                }

                p0 += pitch*offset[1];
                p1 = p0 + offset[0]*bpp;
                for(u32 k=0; k<bpp; ++k){
                    value[k] += src[p0+k];
                    value[k] += src[p1+k];
                    value[k] >>= 2;
                }

                //出力
                //-------------------------------------
                u32 dp = (j*dw + i)*bpp;
                for(u32 k=0; k<bpp; ++k){
                    dst[dp + k] = static_cast<u8>( value[k] );
                }

            }
        }
    }

    //-----------------------------------------------------
    void calcHalfSize(u32& dw, u32& dh, u32 sw, u32 sh)
    {
        dw = (sw<=2)? 1 : (sw >> 1);
        dh = (sh<=2)? 1 : (sh >> 1);
    }

    //-----------------------------------------------------
    // フォーマットからピクセル辺りのバイト数計算
    u32 calcBpp(BufferFormat format)
    {
        switch(format)
        {
        case Buffer_A8:
            return 1;
            break;

        case Buffer_R8G8B8:
#if defined(LIME_GL)
        case Buffer_B8G8R8:
#endif
            return 3;
            break;

        case Buffer_A8R8G8B8:
        case Buffer_X8R8G8B8:
        case Buffer_A8B8G8R8:
        case Buffer_X8B8G8R8:
            return 4;
            break;

        case Buffer_L8:
            return 1;
            break;

        default:
            LASSERT(false);
            break;
        };
        return 0;
    }


    //-----------------------------------------------------
    // ミップマップ作成
    void createMipMap(TextureRef& dst, u8* src, u32 width, u32 height, BufferFormat format, u32 levels)
    {
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        //convertToPow2Image(&src, width, height, format);

        dst.attach(); //bind

        dst.blit(0, width, height, src); //0番目

        u32 bpp = calcBpp(format);

        u32 dw, dh;
        calcHalfSize(dw, dh, width, height);

        u8* buffer = LIME_NEW u8[dw*dh*bpp];

        u8* tmp0 = src;
        u8* tmp1 = buffer;

        for(u32 i=1; i<=levels; ++i){
            createHalfSizeImage(tmp1, dw, dh, tmp0, width, height, bpp);
            dst.blit(i, dw, dh, tmp1); //テクスチャデータ転送

            lcore::swap(tmp1, tmp0);
            width = dw;
            height = dh;
            calcHalfSize(dw, dh, width, height);
        }

        LIME_DELETE_ARRAY(buffer);
        dst.detach(); //unbind
    }

    //-----------------------------------------------------
    // ミップマップ作成
    void createMipMapDebug(TextureRef& dst, u8* src, u32 width, u32 height, BufferFormat format, u32 levels)
    {
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        static const u32 NumColors = 10;
        static const u8 Colors[NumColors][4] =
        {
            {0xFFU, 0x00U, 0x00U, 0xFFU},
            {0x80U, 0x00U, 0x00U, 0xFFU},
            {0x00U, 0xFFU, 0x00U, 0xFFU},
            {0x00U, 0x80U, 0x00U, 0xFFU},
            {0x00U, 0x00U, 0xFFU, 0xFFU},

            {0x00U, 0x00U, 0x80U, 0xFFU},
            {0xFFU, 0xFFU, 0x00U, 0xFFU},
            {0x80U, 0x80U, 0x00U, 0xFFU},
            {0x00U, 0xFFU, 0xFFU, 0xFFU},
            {0x00U, 0x80U, 0x80U, 0xFFU},
        };

        convertToPow2Image(&src, width, height, format);

        u32 bpp = calcBpp(format);

        u8* tmp;

        dst.attach(); //bind
        for(u32 i=0; i<=levels; ++i){

            tmp = src;
            u32 index = (NumColors<=i)? NumColors-1 : i;

            for(u32 j=0; j<width*height; ++j){
                for(u32 k=0; k<bpp; ++k){
                    tmp[k] = Colors[index][k];
                }
                tmp += bpp;
            }

            dst.blit(i, width, height, src); //テクスチャデータ転送
            calcHalfSize(width, height, width, height);
        }
        dst.detach(); //unbind
    }


    u32 calcMipMapLevels(u32 width, u32 height)
    {
        u32 log2_w = static_cast<u32>( lmath::log( static_cast<f32>(width) ) * InvLog2 );
        u32 log2_h = static_cast<u32>( lmath::log( static_cast<f32>(height) ) * InvLog2 );
        u32 levels = lcore::maximum(log2_w, log2_h) + 1;
        return levels;
    }
}
}
