/**
@file ETC1.cpp
@author t-sakai
@date 2011/05/14
*/
#include "ETC1.h"
#include <lmath/lmathcore.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/io/IOTextureUtil.h>


namespace lgraphics
{

namespace
{
    class ModeBase
    {
    public:
        static void clearBlock(u8* block)
        {
            for(u32 i=0; i<8; ++i){
                block[i] = 0;
            }
        }

        static u8 getFlipBit(const u8* block)
        {
            return (block[3] & 0x01U);
        }

        static void setFlipBit(u8* block, u8 bit)
        {
            LASSERT(bit == 0 || bit == 1);
            block[3] |= bit;
        }


        static u8 getDiffBit(const u8* block)
        {
            return (block[3] & 0x02U);
        }

        static void setDiffBit(u8* block, u8 bit)
        {
            LASSERT(bit == 0 || bit == 1);
            block[3] |= (bit<<1);
        }


        static u8 getTableCodeWord1(const u8* block)
        {
            return (block[3] >> 2) & 0x07U;
        }

        static void setTableCodeWord1(u8* block, u8 cw)
        {
            LASSERT(cw<8);
            block[3] &= 0xE3U;
            block[3] |= (cw<<2);
        }

        static u8 getTableCodeWord2(const u8* block)
        {
            return block[3] >> 5;
        }

        static void setTableCodeWord2(u8* block, u8 cw)
        {
            LASSERT(cw<8);
            block[3] &= 0x1FU;
            block[3] |= (cw<<5);
        }
#if 0
        static void setMSB(u8* msb, u32 pixelNo, u8 index)
        {
            LASSERT(pixelNo<16);
            LASSERT(0<=index && index<4);

            index >>= 1;
            u32 n = pixelNo >> 3;
            u32 p = pixelNo - (n<<3);
            n = 1-n;
            u8 mask = index << p;
            msb[n] |= mask;

        }

        static void setLSB(u8* lsb, u32 pixelNo, u8 index)
        {
            LASSERT(pixelNo<16);
            LASSERT(0<=index && index<4);

            index &= 0x01U;
            u32 n = pixelNo >> 3;
            u32 p = pixelNo - (n<<3);
            n = 1-n;
            u8 mask = index << p;
            lsb[n] |= mask;
        }

#else
        static void setBit(u8* msb, u8* lsb, u8 x, u8 y, u8 index)
        {
            LASSERT(x<4);
            LASSERT(y<4);
            LASSERT(0<=index && index<4);

            u8 li = index & 0x01U;
            index >>= 1;
            u8 mi = index;

            y += x<<2;
            x >>= 1;
            u8 p = y - (x<<3);
            u8 n = 1 - x;

            msb[n] |= mi << p;
            lsb[n] |= li << p;
        }
#endif

    };

    const s16 ModifierTable[8][4] =
    {
#if 0
        {-8, -2, 2, 8}, //0
        {-17, -5, 5, 17}, //1
        {-29, -9, 9, 29}, //2
        {-42, -13, 13, 42}, //3
        {-60, -18, 18, 60}, //4
        {-80, -24, 24, 80}, //5
        {-106, -33, 33, 106}, //6
        {-183, -47, 47, 183}, //7
#else
        {2, 8, -2, -8}, //0
        {5, 17, -5, -17}, //1
        {9, 29, -9, -29}, //2
        {13, 42, -13, -42}, //3
        {18, 60, -18, -60}, //4
        {24, 80, -24, -80}, //5
        {33, 106, -33, -106}, //6
        {47, 183, -47, -183}, //7
#endif
    };

    /// 16:16固定少数版テーブル
    const u32 ModifierTableFixed[8][4] =
    {
        {0x020000U, 0x080000U, 0xFFFE0000U, 0xFFF80000U, },
        {0x050000U, 0x110000U, 0xFFFB0000U, 0xFFEF0000U, },
        {0x090000U, 0x1D0000U, 0xFFF70000U, 0xFFE30000U, },
        {0x0D0000U, 0x2A0000U, 0xFFF30000U, 0xFFD60000U, },
        {0x120000U, 0x3C0000U, 0xFFEE0000U, 0xFFC40000U, },
        {0x180000U, 0x500000U, 0xFFE80000U, 0xFFB00000U, },
        {0x210000U, 0x6A0000U, 0xFFDF0000U, 0xFF960000U, },
        {0x2F0000U, 0xB70000U, 0xFFD10000U, 0xFF490000U, },
    };

    class IndividualMode
    {
    public:
        static void getBaseColor1(u8& r, u8& g, u8& b, const u8* block)
        {
            r = block[0] & 0xF0U;
            r += (r>>4);

            g = block[1] & 0xF0U;
            g += (g>>4);

            b = block[2] & 0xF0U;
            b += (b>>4);
        }

        static void getBaseColor2(u8& r, u8& g, u8& b, const u8* block)
        {
            r = block[0] & 0x0FU;
            r += (r<<4);

            g = block[1] & 0x0FU;
            g += (g<<4);

            b = block[2] & 0x0FU;
            b += (b<<4);
        }

        static void setBaseColor1(u8* block, u8 r, u8 g, u8 b)
        {
            block[0] &= 0x0FU;
            block[0] |= r << 4;

            block[1] &= 0x0FU;
            block[1] |= g << 4;

            block[2] &= 0x0FU;
            block[2] |= b << 4;
        }

        static void setBaseColor2(u8* block, u8 r, u8 g, u8 b)
        {
            block[0] &= 0xF0U;
            block[0] |= (r & 0x0FU);

            block[1] &= 0xF0U;
            block[1] |= (g & 0x0FU);

            block[2] &= 0xF0U;
            block[2] |= (b & 0x0FU);
        }
    };


    // TODO:
    class DifferentialMode
    {
    public:
        static void getBaseColor1(u8& r, u8& g, u8& b, const u8* block)
        {
            r = block[0] & 0xF8U;
            r += (r>>5);

            g = block[1] & 0xF8U;
            g += (g>>5);

            b = block[2] & 0xF8U;
            b += (b>>5);
        }

        static void getBaseColor2(s8& r, s8& g, s8& b, const u8* block)
        {
            r = block[0] & 0x07U;
            if((r&0x04U) != 0){
                r = r ^ 0x07U + 1;
                r = -r;
            }

            g = block[1] & 0x07U;
            if((g&0x04U) != 0){
                g = g ^ 0x07U + 1;
                g = -g;
            }

            b = block[2] & 0x07U;
            if((b&0x04U) != 0){
                b = b ^ 0x07U + 1;
                b = -b;
            }
        }

        static void setBaseColor1(u8* block, u8 r, u8 g, u8 b)
        {
            block[0] &= 0x07U;
            block[0] |= r << 3;

            block[1] &= 0x07U;
            block[1] |= g << 3;

            block[2] &= 0x07U;
            block[2] |= b << 3;
        }

        static void setBaseColor2(u8* block, s8 r, s8 g, s8 b)
        {
            block[0] &= 0xF8U;
            block[0] |= (r & 0x07U);

            block[1] &= 0xF8U;
            block[1] |= (g & 0x07U);

            block[2] &= 0xF8U;
            block[2] |= (b & 0x07U);
        }
    };


    // Le Gall (CDF) 5/3 Analisys Filter
    static const f32 CDF53LowPass[5] =
    {
        -(1.0f/8.0f),
         (2.0f/8.0f),
         (6.0f/8.0f),
         (2.0f/8.0f),
        -(1.0f/8.0f),
    };

    static const f32 CDF53HighPass[3] =
    {
         (1.0f/2.0f),
         1.0f,
         (1.0f/2.0f),
    };


    u32 avgCDF53Convolusion(u32 d0, u32 d1, u32 d2, u32 d3)
    {
        u32 ret = d0 + d1;

        ret += (d0>>1) + d1 + (d2>>1);
        ret += (d1>>1) + d2 + (d3>>1);
        ret += d2 + d3;

        return ret;
     }


    bool isHigherFrequencyVertical(etc::Texel* color)
    {
        u32 h = 0;
        u32 d0, d1, d2, d3;
        for(u32 i=0; i<4; ++i){
            u32 j = i<<2;

            d0 = color[j+0].getGrayFixed();
            d1 = color[j+1].getGrayFixed();
            d2 = color[j+2].getGrayFixed();
            d3 = color[j+3].getGrayFixed();

            h += avgCDF53Convolusion(d0, d1, d2, d3);
        }

        u32 v = 0;
        for(u32 i=0; i<4; ++i){
            d0 = color[i+0].getGrayFixed();
            d1 = color[i+1].getGrayFixed();
            d2 = color[i+2].getGrayFixed();
            d3 = color[i+3].getGrayFixed();

            v += avgCDF53Convolusion(d0, d1, d2, d3);
        }
        return (h<v);
    }

}

namespace etc
{
    const f32 GrayFixedPoint::WeightR = 0.299f * 65536.0f;
    const f32 GrayFixedPoint::WeightG = 0.587f * 65536.0f;
    const f32 GrayFixedPoint::WeightB = 0.114f * 65536.0f;

    u32 ETC1Codec::calcEncodedSize(u32 width, u32 height)
    {
        width += 0x03U;
        width >>= 2;

        height += 0x03U;
        height >>= 2;

        return (width * height * 8);
    }


#if 1
    const f32 WeightR = 0.299f;
    const f32 WeightG = 0.587f;
    const f32 WeightB = 0.114f;

    //-----------------------------------------------------------------------------
    // ミップマップ作成
    void ETC1Codec::encodeMipMap(TextureRef& dst, u8* src, u32 width, u32 height, u32 levels)
    {
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        //io::convertToPow2Image(&src, width, height, lgraphics::Buffer_B8G8R8);
        u32 encodedSize = calcEncodedSize(width, height);

        u32 dw, dh;
        io::calcHalfSize(dw, dh, width, height);

        u32 size = dw*dh*3;

        u8* buffer = LIME_NEW u8[size + encodedSize];
        u8* half = buffer + encodedSize;

        dst.attach(); //bind

        //0番目圧縮
        encode(buffer, src, width, height);

        dst.blit(0, width, height, buffer);

        u8* tmp0 = src;
        u8* tmp1 = half;

        for(u32 i=1; i<=levels; ++i){
            io::createHalfSizeImage(tmp1, dw, dh, tmp0, width, height, 3);
            encode(buffer, tmp1, dw, dh);
            dst.blit(i, dw, dh, buffer); //テクスチャデータ転送

            lcore::swap(tmp1, tmp0);
            width = dw;
            height = dh;
            io::calcHalfSize(dw, dh, width, height);
        }

        LIME_DELETE_ARRAY(buffer);
        dst.detach(); //unbind
    }

    //-----------------------------------------------------------------------------
    bool ETC1Codec::encode(u8* dst, const u8* src, u32 width, u32 height)
    {
        LASSERT(dst != NULL);
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        width_ = width;
        height_ = height;

        u32 ublocks = (width + 0x03U)>>2;
        u32 vblocks = (height + 0x03U)>>2;

        u32 nv = (vblocks-1);
        u32 nu = (ublocks-1);

        for(u32 i=0; i<nv; ++i){
            u32 sy = i<<2;
            u32 ey = sy + 4;

            for(u32 j=0; j<nu; ++j){
                u32 sx = j<<2;
                getTexel(src, sx, sy);
                encodeBlock(dst);
            }

            for(u32 j=nu; j<ublocks; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, width, sy, ey);
                encodeBlock(dst);
            }
        }

        for(u32 i=nv; i<vblocks; ++i){
            u32 sy = i<<2;
            u32 ey = height;

            for(u32 j=0; j<nu; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, sx+4, sy, ey);
                encodeBlock(dst);
            }

            for(u32 j=nu; j<ublocks; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, width, sy, ey);
                encodeBlock(dst);
            }
        }
        return true;
    }

    inline void ETC1Codec::getTexel(const u8* src, u32 sx, u32 sy)
    {
        for(u32 i=0; i<4; ++i){
            const u8* row = src + 3*((sy+i)*width_ + sx);
            Texel *trow = texels_ + (i<<2);

            for(u32 j=0; j<4; ++j){
                trow->r_ = row[0];
                trow->g_ = row[1];
                trow->b_ = row[2];

                ++trow;
                row += 3;
            }
        }

    }

    inline void ETC1Codec::getTexelPartial(const u8* src, u32 sx, u32 ex, u32 sy, u32 ey)
    {
        for(u32 i=0; i<16; ++i){
            texels_[i].r_ = 0;
            texels_[i].g_ = 0;
            texels_[i].b_ = 0;
        }

        for(u32 i=sy; i<ey; ++i){
            const u8* row = src + 3*(sy*width_ + sx);
            Texel *trow = texels_ + ((i-sy) << 2);

            for(u32 j=sx; j<ex; ++j){
                trow->r_ = row[0];
                trow->g_ = row[1];
                trow->b_ = row[2];

                ++trow;
                row += 3;
            }
        }
    }


    //---------------------------------------------------------------------
    void ETC1Codec::encodeBlock(u8*& dst)
    {
#if 0
        u8 vblock[8];
        u8 hblock[8];

        ModeBase::clearBlock(vblock);
        ModeBase::clearBlock(hblock);

        f32 verror = encodeBlock4x2( vblock );
        f32 herror = encodeBlock2x4( hblock );
        if(verror < herror){
            lcore::memcpy(dst, vblock, 8);
        }else{
            lcore::memcpy(dst, hblock, 8);
        }
#else
        ModeBase::clearBlock(dst);

        if(isHigherFrequencyVertical(texels_)){
            encodeBlock2x4avg( dst );

        }else{
            encodeBlock4x2avg( dst );
        }
#endif

        //１ブロック終了したので、8バイト進める
        dst += 8;
    }

#if 0
    //---------------------------------------------------------------------
    u32 ETC1Codec::encodeBlock2x4(u8* dst)
    {
        ModeBase::setFlipBit(dst, 1);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        {
            TexelFixedPoint texel1, texel2;
            u8 rgb4bit1[3];
            u8 rgb4bit2[3];
            u8 rgb5bit1[3];
            u8 rgb5bit2[3];

            calcAvg2x4(texel1, 0, 8);
            calcAvg2x4(texel2, 8, 16);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit1, texel1);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit2, texel2);


            //サブブロック１との差をとる
            s32 diff[3];
            for(s32 i=0; i<3; ++i){
                diff[i] = rgb5bit2[i];
                diff[i] -= rgb5bit1[i];
            }


            // 差が4ビット以内ならDifferentialモード
            if((-4<=diff[0]) && (diff[0]<=3)
                && (-4<=diff[1]) && (diff[1]<=3)
                && (-4<=diff[2]) && (diff[2]<=3))
            {
                ModeBase::setDiffBit(dst, 1);

                DifferentialMode::setBaseColor1(dst, rgb5bit1[0], rgb5bit1[1], rgb5bit1[2]);
                DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

                s32 tmp;
                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb5bit1[i]<<3) | (rgb5bit1[i]>>2);

                    tmp = rgb5bit1[i];
                    tmp += diff[i];
                    tmp = lmath::clamp(tmp, 0, 255);

                    rgb2[i] = tmp;
                    rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
                }

            }else{
                quantizeTo444(rgb4bit1, texel1);
                quantizeTo444(rgb4bit2, texel2);

                ModeBase::setDiffBit(dst, 0);
                IndividualMode::setBaseColor1(dst, rgb4bit1[0], rgb4bit1[1], rgb4bit1[2]);
                IndividualMode::setBaseColor2(dst, rgb4bit2[0], rgb4bit2[1], rgb4bit2[2]);

                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb4bit1[i]<<4) | rgb4bit1[i];
                    rgb2[i] = (rgb4bit2[i]<<4) | rgb4bit2[i];
                }
            }

        }


        u8 min_table;
        u32 error = encode2x4(dst, min_table, rgb1, 0, 8);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode2x4(dst, min_table, rgb2, 8, 16);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }
#endif

    //---------------------------------------------------------------------
    u32 ETC1Codec::encodeBlock2x4avg(u8* dst)
    {
        ModeBase::setFlipBit(dst, 1);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        TexelFixedPoint texel1, texel2;
        s32 diff[3];

        calcAvg2x4(texel1, 0, 8);
        calcAvg2x4(texel2, 8, 16);


        //256*256*31/255 = 7967.1215686274509803921568627451
        static const u32 ratio = 7967;
        //texel1.mul(ratio);
        texel1.rshift(3);
        texel1.get(rgb1[0], rgb1[1], rgb1[2]);

        //texel2.mul(ratio);
        texel2.rshift(3);
        texel2.get(rgb2[0], rgb2[1], rgb2[2]);

        //サブブロック１との差をとる
        for(s32 i=0; i<3; ++i){
            diff[i] = rgb2[i];
            diff[i] -= rgb1[i];
        }

        // 差が4ビット以内ならDifferentialモード
        if((-4<=diff[0]) && (diff[0]<=3)
            && (-4<=diff[1]) && (diff[1]<=3)
            && (-4<=diff[2]) && (diff[2]<=3))
        {
            ModeBase::setDiffBit(dst, 1);

            DifferentialMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

            s32 tmp;
            for(s32 i=0; i<3; ++i){
                tmp = rgb1[i];
                tmp += diff[i];
                tmp = lmath::clamp(tmp, 0, 255);

                rgb1[i] = (rgb1[i]<<3) | (rgb1[i]>>2);

                rgb2[i] = tmp;
                rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
            }

        }else{
            //256*256*4*0.501 = 7182.336
            //256*256/17 = 3855.0588235294117647058823529412
            static const u32 ratio2 = 3855;
            static const u32 offset = 7182;

            //texel1.mul(ratio2);
            texel1.rshift(1);
            //texel1.add(offset);
            texel1.get(rgb1[0], rgb1[1], rgb1[2]);

            //texel2.mul(ratio2);
            texel2.rshift(1);
            //texel2.add(offset);
            texel2.get(rgb2[0], rgb2[1], rgb2[2]);

            ModeBase::setDiffBit(dst, 0);
            IndividualMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            IndividualMode::setBaseColor2(dst, rgb2[0], rgb2[1], rgb2[2]);

            for(s32 i=0; i<3; ++i){
                rgb1[i] = (rgb1[i]<<4) | rgb1[i];
                rgb2[i] = (rgb2[i]<<4) | rgb2[i];
            }
        }


        u8 min_table;
        u32 error = encode2x4(dst, min_table, rgb1, 0, 8);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode2x4(dst, min_table, rgb2, 8, 16);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }

#if 0
    //---------------------------------------------------------------------
    u32 ETC1Codec::encodeBlock4x2(u8* dst)
    {
        ModeBase::setFlipBit(dst, 0);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        {
            TexelFixedPoint texel1, texel2;
            u8 rgb4bit1[3];
            u8 rgb4bit2[3];
            u8 rgb5bit1[3];
            u8 rgb5bit2[3];

            calcAvg4x2(texel1, true);
            calcAvg4x2(texel2, false);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit1, texel1);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit2, texel2);


            //サブブロック１との差をとる
            s32 diff[3];
            for(s32 i=0; i<3; ++i){
                diff[i] = rgb5bit2[i];
                diff[i] -= rgb5bit1[i];
            }


            // 差が4ビット以内ならDifferentialモード
            if((-4<=diff[0]) && (diff[0]<=3)
                && (-4<=diff[1]) && (diff[1]<=3)
                && (-4<=diff[2]) && (diff[2]<=3))
            {
                ModeBase::setDiffBit(dst, 1);

                DifferentialMode::setBaseColor1(dst, rgb5bit1[0], rgb5bit1[1], rgb5bit1[2]);
                DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

                s32 tmp;
                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb5bit1[i]<<3) | (rgb5bit1[i]>>2);
                    tmp = rgb5bit1[i];
                    tmp += diff[i];
                    tmp = lmath::clamp(tmp, 0, 255);

                    rgb2[i] = tmp;
                    rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
                }

            }else{
                quantizeTo444(rgb4bit1, texel1);
                quantizeTo444(rgb4bit2, texel2);

                ModeBase::setDiffBit(dst, 0);
                IndividualMode::setBaseColor1(dst, rgb4bit1[0], rgb4bit1[1], rgb4bit1[2]);
                IndividualMode::setBaseColor2(dst, rgb4bit2[0], rgb4bit2[1], rgb4bit2[2]);

                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb4bit1[i]<<4) | rgb4bit1[i];
                    rgb2[i] = (rgb4bit2[i]<<4) | rgb4bit2[i];
                }
            }

        }

        u8 min_table;
        u32 error = encode4x2(dst, min_table, rgb1, true);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode4x2(dst, min_table, rgb2, false);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }
#endif

    //---------------------------------------------------------------------
    u32 ETC1Codec::encodeBlock4x2avg(u8* dst)
    {
        ModeBase::setFlipBit(dst, 0);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        TexelFixedPoint texel1, texel2;

        calcAvg4x2(texel1, 0);
        calcAvg4x2(texel2, 2);


        //256*256*31/255 = 7967.1215686274509803921568627451
        static const u32 ratio = 7967;
        //texel1.mul(ratio);
        texel1.rshift(3);
        texel1.get(rgb1[0], rgb1[1], rgb1[2]);

        //texel2.mul(ratio);
        texel2.rshift(3);
        texel2.get(rgb2[0], rgb2[1], rgb2[2]);


        //サブブロック１との差をとる
        s32 diff[3];
        for(s32 i=0; i<3; ++i){
            diff[i] = rgb2[i];
            diff[i] -= rgb1[i];
        }


        // 差が4ビット以内ならDifferentialモード
        if((-4<=diff[0]) && (diff[0]<=3)
            && (-4<=diff[1]) && (diff[1]<=3)
            && (-4<=diff[2]) && (diff[2]<=3))
        {
            ModeBase::setDiffBit(dst, 1);

            DifferentialMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

            s32 tmp;
            for(s32 i=0; i<3; ++i){
                tmp = rgb1[i];
                tmp += diff[i];
                tmp = lmath::clamp(tmp, 0, 255);

                rgb1[i] = (rgb1[i]<<3) | (rgb1[i]>>2);

                rgb2[i] = tmp;
                rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
            }

        }else{
            //256*256*4*0.501 = 7182.336
            //256*256/17 = 3855.0588235294117647058823529412
            static const u32 ratio2 = 3855;
            static const u32 offset = 7182;

            texel1.rshift(1);
            //texel1.add(offset);
            texel1.get(rgb1[0], rgb1[1], rgb1[2]);

            texel2.rshift(1);
            //texel2.add(offset);
            texel2.get(rgb2[0], rgb2[1], rgb2[2]);

            ModeBase::setDiffBit(dst, 0);
            IndividualMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            IndividualMode::setBaseColor2(dst, rgb2[0], rgb2[1], rgb2[2]);

            for(s32 i=0; i<3; ++i){
                rgb1[i] = (rgb1[i]<<4) | rgb1[i];
                rgb2[i] = (rgb2[i]<<4) | rgb2[i];
            }
        }

        u8 min_table;
        u32 error = encode4x2(dst, min_table, rgb1, 0);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode4x2(dst, min_table, rgb2, 2);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }

    //---------------------------------------------------------------------
    void ETC1Codec::calcAvg2x4(TexelFixedPoint& texel, u32 sx, u32 ex)
    {
        u32 r=0, g=0, b=0;
        for(u32 i=sx; i<ex; ++i){
            r += texels_[i].r_;
            g += texels_[i].g_;
            b += texels_[i].b_;
        }

        texel.setShift(r, g, b, TexelFixedPoint::NumBitShift-3);
    }


    //---------------------------------------------------------------------
    void ETC1Codec::calcAvg4x2(TexelFixedPoint& texel, u32 sx)
    {
        u32 r=0, g=0, b=0;
        u32 k = 0;

        for(u32 j=0; j<4; ++j){
            k = (j<<2) + sx;
            r += texels_[k].r_;
            g += texels_[k].g_;
            b += texels_[k].b_;

            ++k;
            r += texels_[k].r_;
            g += texels_[k].g_;
            b += texels_[k].b_;
        }

        texel.setShift(r, g, b, TexelFixedPoint::NumBitShift-3);
    }

#if 0
    //---------------------------------------------------------------------
    void ETC1Codec::quantizeTo444(u8* encoded, const TexelFixedPoint& avg)
    {
        u32 low4bit[3];
        u32 high4bit[3];
        u32 low[3];
        u32 high[3];

        low4bit[0] = avg.r_ >> 4;
        low4bit[1] = avg.g_ >> 4;
        low4bit[2] = avg.b_ >> 4;
        for(s32 i=0; i<3; ++i){
            high4bit[i] = (low4bit[i]>15)? 15 : (low4bit[i] + 1);

            low[i] = (low4bit[i] << 4) | low4bit[i];
            high[i] = (high4bit[i] << 4) | high4bit[i];
        }

        quantize(encoded, avg, low, high, low4bit, high4bit);
    }

    //---------------------------------------------------------------------
    void ETC1Codec::quantizeTo555(u8* encoded, const TexelFixedPoint& avg)
    {
        u32 low5bit[3];
        u32 high5bit[3];
        u32 low[3];
        u32 high[3];

        low5bit[0] = avg.r_ >> 3;
        low5bit[1] = avg.g_ >> 3;
        low5bit[2] = avg.b_ >> 3;
        for(s32 i=0; i<3; ++i){
            high5bit[i] = (low5bit[i]>31)? 31 : (low5bit[i] + 1);

            low[i] = (low5bit[i] << 3) | (low5bit[i]>>2);
            high[i] = (high5bit[i] << 3) | (high5bit[i]>>2);
        }

        quantize(encoded, avg, low, high, low5bit, high5bit);
    }

    //---------------------------------------------------------------------
    void ETC1Codec::quantize(u8* encoded, const TexelFixedPoint& avg, const u32* low, const u32* high, const u32* lbit, const u32* hbit)
    {
        f32 k[3];
        f32 d[3];

        k[0] = static_cast<f32>(high[0] - low[0]);
        k[1] = static_cast<f32>(high[1] - low[1]);
        k[2] = static_cast<f32>(high[2] - low[2]);

        d[0] = static_cast<f32>(low[0]) - avg.r_;
        d[1] = static_cast<f32>(low[1]) - avg.g_;
        d[2] = static_cast<f32>(low[2]) - avg.b_;

        const f32 RG = WeightR * WeightG;
        const f32 GB = WeightG * WeightB;
        const f32 BR = WeightB * WeightR;

        f32 error[8];
        error[0] = RG*sqr((d[0] + 0.0f) - (d[1] + 0.0f)) + BR*sqr((d[2] + 0.0f) - (d[0] + 0.0f)) + GB*sqr((d[1] + 0.0f) - (d[2] + 0.0f));
        error[1] = RG*sqr((d[0] + k[0]) - (d[1] + 0.0f)) + BR*sqr((d[2] + 0.0f) - (d[0] + k[0])) + GB*sqr((d[1] + 0.0f) - (d[2] + 0.0f));
        error[2] = RG*sqr((d[0] + 0.0f) - (d[1] + k[1])) + BR*sqr((d[2] + 0.0f) - (d[0] + 0.0f)) + GB*sqr((d[1] + k[1]) - (d[2] + 0.0f));
        error[3] = RG*sqr((d[0] + 0.0f) - (d[1] + 0.0f)) + BR*sqr((d[2] + k[2]) - (d[0] + 0.0f)) + GB*sqr((d[1] + 0.0f) - (d[2] + k[2]));
        error[4] = RG*sqr((d[0] + k[0]) - (d[1] + k[1])) + BR*sqr((d[2] + 0.0f) - (d[0] + k[0])) + GB*sqr((d[1] + k[1]) - (d[2] + 0.0f));
        error[5] = RG*sqr((d[0] + k[0]) - (d[1] + 0.0f)) + BR*sqr((d[2] + k[2]) - (d[0] + k[0])) + GB*sqr((d[1] + 0.0f) - (d[2] + k[2]));
        error[6] = RG*sqr((d[0] + 0.0f) - (d[1] + k[1])) + BR*sqr((d[2] + k[2]) - (d[0] + 0.0f)) + GB*sqr((d[1] + k[1]) - (d[2] + k[2]));
        error[7] = RG*sqr((d[0] + k[0]) - (d[1] + k[1])) + BR*sqr((d[2] + k[2]) - (d[0] + k[0])) + GB*sqr((d[1] + k[1]) - (d[2] + k[2]));

        s32 min_index = 0;
        f32 min_error = error[0];
        for(s32 i=1; i<8; ++i){
            if(error[i]<min_error){
                min_error = error[i];
                min_index = i;
            }
        }

        switch(min_index)
        {
        case 0:
            encoded[0] = lbit[0];
            encoded[1] = lbit[1];
            encoded[2] = lbit[2];
            break;

        case 1:
            encoded[0] = hbit[0];
            encoded[1] = lbit[1];
            encoded[2] = lbit[2];
            break;

        case 2:	
            encoded[0] = lbit[0];
            encoded[1] = hbit[1];
            encoded[2] = lbit[2];
            break;

        case 3:	
            encoded[0] = lbit[0];
            encoded[1] = lbit[1];
            encoded[2] = hbit[2];
            break;

        case 4:	
            encoded[0] = hbit[0];
            encoded[1] = hbit[1];
            encoded[2] = lbit[2];
            break;

        case 5:	
            encoded[0] = hbit[0];
            encoded[1] = lbit[1];
            encoded[2] = hbit[2];
            break;

        case 6:	
            encoded[0] = lbit[0];
            encoded[1] = hbit[1];
            encoded[2] = hbit[2];
            break;

        case 7:	
            encoded[0] = hbit[0];
            encoded[1] = hbit[1];
            encoded[2] = hbit[2];
            break;

        default:
            break;
        }
    }
#endif

    //---------------------------------------------------------------------
    u32 ETC1Codec::encode2x4(u8* dst, u8& min_table, const u8* rgb, u32 sx, u32 ex)
    {
        u32 min_error = 0xFFFFFFFFU;
        u32 error;

        min_table = 0;

        u8 msb[2];
        u8 lsb[2];
        u8 best_msb[2];
        u8 best_lsb[2];

        for(u8 i=0; i<8; ++i){

            msb[0] = dst[4];
            msb[1] = dst[5];
            lsb[0] = dst[6];
            lsb[1] = dst[7];

            error = encode2x4(msb, lsb, i, rgb, sx, ex);
            if(error < min_error){
                min_error = error;
                min_table = i;
                best_msb[0] = msb[0]; best_msb[1] = msb[1];
                best_lsb[0] = lsb[0]; best_lsb[1] = lsb[1];
            }
        }

        dst[4] = best_msb[0];
        dst[5] = best_msb[1];
        dst[6] = best_lsb[0];
        dst[7] = best_lsb[1];

        return min_error;
    }

    //---------------------------------------------------------------------
    u32 ETC1Codec::encode2x4(u8* msb, u8* lsb, u8 table, const u8* rgb, u32 sx, u32 ex)
    {
        u32 min_error;
        u32 error;
        u32 sum_error = 0;
        s32 pix;
        s16 r, g, b;

        for(u32 i=sx; i<ex; ++i){
            const Texel& texel = texels_[i];

            min_error = 0xFFFFFFFFU;
            pix = 0;

            for(s32 j=0; j<4; ++j){
                r = lmath::clamp<s16>(ModifierTable[table][j] + rgb[0], 0, 255) - texel.r_;
                g = lmath::clamp<s16>(ModifierTable[table][j] + rgb[1], 0, 255) - texel.g_;
                b = lmath::clamp<s16>(ModifierTable[table][j] + rgb[2], 0, 255) - texel.b_;

                r *= r;
                g *= g;
                b *= b;

                error = WeightRError*r + WeightGError*g + WeightBError*b;

                if(error<min_error){
                    min_error = error;
                    pix = j;
                }
            }
#if 0
            u8 y = i >> 2;
            u8 x = i - (y<<2);
            u8 p = (x << 2) + y;
            ModeBase::setMSB(msb, p, pix);
            ModeBase::setLSB(lsb, p, pix);
#else
            u8 y = i >> 2;
            u8 x = i - (y<<2);
            ModeBase::setBit(msb, lsb, x, y, pix);
#endif

            sum_error += min_error;
        }
        return sum_error;
    }

    //---------------------------------------------------------------------
    u32 ETC1Codec::encode4x2(u8* dst, u8& min_table, const u8* rgb, u32 sx)
    {
        u32 min_error = 0xFFFFFFFFU;
        u32 error;

        min_table = 0;

        u8 msb[2];
        u8 lsb[2];
        u8 best_msb[2];
        u8 best_lsb[2];

        for(u8 i=0; i<8; ++i){

            msb[0] = dst[4];
            msb[1] = dst[5];
            lsb[0] = dst[6];
            lsb[1] = dst[7];

            error = encode4x2(msb, lsb, i, rgb, sx);
            if(error < min_error){
                min_error = error;
                min_table = i;
                best_msb[0] = msb[0]; best_msb[1] = msb[1];
                best_lsb[0] = lsb[0]; best_lsb[1] = lsb[1];
            }
        }

        dst[4] = best_msb[0];
        dst[5] = best_msb[1];
        dst[6] = best_lsb[0];
        dst[7] = best_lsb[1];

        return min_error;
    }

    //---------------------------------------------------------------------
    u32 ETC1Codec::encode4x2(u8* msb, u8* lsb, u8 table, const u8* rgb, u32 sx)
    {
        u32 min_error;
        u32 error;
        u32 sum_error = 0;
        s32 pix;
        s16 r, g, b;

        u32 index = 0;
        for(u32 j=0; j<4; ++j){
            index = (j<<2) + sx;
            for(u32 i=0; i<2; ++i){
                const Texel& texel = texels_[index];

                min_error = 0xFFFFFFFFU;
                pix = 0;

                for(s32 k=0; k<4; ++k){
                    r = lmath::clamp<s32>(ModifierTable[table][k] + rgb[0], 0, 255) - texel.r_;
                    g = lmath::clamp<s32>(ModifierTable[table][k] + rgb[1], 0, 255) - texel.g_;
                    b = lmath::clamp<s32>(ModifierTable[table][k] + rgb[2], 0, 255) - texel.b_;

                    r *= r;
                    g *= g;
                    b *= b;

                    error = WeightRError*r + WeightGError*g + WeightBError*b;

                    if(error<min_error){
                        min_error = error;
                        pix = k;
                    }
                }
#if 0
                u8 x = i+sx;
                u8 p = (x << 2) + j;
                ModeBase::setMSB(msb, p, pix);
                ModeBase::setLSB(lsb, p, pix);
#else
                ModeBase::setBit(msb, lsb, i+sx, j, pix);
#endif
                sum_error += min_error;
                ++index;
            }
        }
        return sum_error;
    }

#else
    /***********************************************************************/
    /***                                                                   */
    /*** Old                                                               */
    /***                                                                   */
    /***********************************************************************/
    const f32 WeightR = 0.299f;
    const f32 WeightG = 0.587f;
    const f32 WeightB = 0.114f;

    const f32 ETC1Codec::OneEight = (1.0f/8.0f);

    //-----------------------------------------------------------------------------
    // ミップマップ作成
    void ETC1Codec::encodeMipMap(TextureRef& dst, u8* src, u32 width, u32 height, u32 levels)
    {
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        io::convertToPow2Image(&src, width, height, lgraphics::Buffer_B8G8R8);

        u32 dw, dh;
        io::calcHalfSize(dw, dh, width, height);

        u32 size = dw*dh*3;

        u8* buffer = LIME_NEW u8[size];

        dst.attach(); //bind

        //0番目圧縮
        encode(buffer, src, width, height);

        dst.blit(0, width, height, buffer);

        u8* tmp0 = src;
        u8* tmp1 = buffer;

        for(u32 i=1; i<=levels; ++i){
            break;
            io::createHalfSizeImage(tmp1, dw, dh, tmp0, width, height, 3);
            encode(tmp0, tmp1, dw, dh);
            dst.blit(i, dw, dh, tmp0); //テクスチャデータ転送

            lcore::swap(tmp1, tmp0);
            width = dw;
            height = dh;
            io::calcHalfSize(dw, dh, width, height);
        }

        LIME_DELETE_ARRAY(buffer);
        dst.detach(); //unbind
    }

    //-----------------------------------------------------------------------------
    bool ETC1Codec::encode(u8* dst, const u8* src, u32 width, u32 height)
    {
        LASSERT(dst != NULL);
        LASSERT(src != NULL);
        LASSERT(width>0);
        LASSERT(height>0);

        width_ = width;
        height_ = height;

        u32 ublocks = (width + 0x03U)>>2;
        u32 vblocks = (height + 0x03U)>>2;

        u32 nv = (vblocks-1);
        u32 nu = (ublocks-1);

        for(u32 i=0; i<nv; ++i){
            u32 sy = i<<2;
            u32 ey = sy + 4;

            for(u32 j=0; j<nu; ++j){
                u32 sx = j<<2;
                getTexel(src, sx, sy);
                encodeBlock(dst);
            }

            for(u32 j=nu; j<ublocks; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, width, sy, ey);
                encodeBlock(dst);
            }
        }

        for(u32 i=nv; i<vblocks; ++i){
            u32 sy = i<<2;
            u32 ey = height;

            for(u32 j=0; j<nu; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, sx+4, sy, ey);
                encodeBlock(dst);
            }

            for(u32 j=nu; j<ublocks; ++j){
                u32 sx = j<<2;
                getTexelPartial(src, sx, width, sy, ey);
                encodeBlock(dst);
            }
        }
        return true;
    }

    inline void ETC1Codec::getTexel(const u8* src, u32 sx, u32 sy)
    {
        for(u32 i=0; i<4; ++i){
            const u8* row = src + 3*(sy+i)*width_ + 3*sx;
            u32 offset = i<<2;
            Texel *trow = texels_ + offset;
            GrayStaticPoint *gray = grays_ + offset;

            for(u32 j=0; j<4; ++j){
                trow->r_ = row[0];
                trow->g_ = row[1];
                trow->b_ = row[2];
                gray->set(*trow);

                trow += 1;
                gray += 1;
                row += 3;
            }
        }
    }

    inline void ETC1Codec::getTexelPartial(const u8* src, u32 sx, u32 ex, u32 sy, u32 ey)
    {
        for(u32 i=0; i<16; ++i){
            texels_[i].r_ = 0;
            texels_[i].g_ = 0;
            texels_[i].b_ = 0;
            grays_[i].v_ = 0;
        }

        for(u32 i=sy; i<ey; ++i){
            const u8* row = src + 3*sy*width_ + 3*sx;
            u32 offset = ((i-sy) << 2);
            Texel *trow = texels_ + offset;
            GrayStaticPoint *gray = grays_ + offset;

            for(u32 j=sx; j<ex; ++j){
                trow->r_ = row[0];
                trow->g_ = row[1];
                trow->b_ = row[2];
                gray->set(*trow);

                trow += 1;
                gray += 1;
                row += 3;
            }
        }
    }


    //---------------------------------------------------------------------
    void ETC1Codec::encodeBlock(u8*& dst)
    {
#if 0
        u8 vblock[8];
        u8 hblock[8];

        ModeBase::clearBlock(vblock);
        ModeBase::clearBlock(hblock);

        f32 verror = encodeBlock4x2( vblock );
        f32 herror = encodeBlock2x4( hblock );
        if(verror < herror){
            lcore::memcpy(dst, vblock, 8);
        }else{
            lcore::memcpy(dst, hblock, 8);
        }
#else
        u8 block[8];
        u8 block_avg[8];
        ModeBase::clearBlock(block);
        ModeBase::clearBlock(block_avg);

        f32 error, erroravg;

        if(isHigherFrequencyVertical(grays_)){
            error = encodeBlock2x4( block );
            erroravg = encodeBlock2x4avg( block_avg );

        }else{
            error = encodeBlock4x2( block );
            erroravg = encodeBlock4x2avg( block_avg );
        }

        if(error < erroravg){
            lcore::memcpy(dst, block, 8);
        }else{
            lcore::memcpy(dst, block_avg, 8);
        }
#endif

        //１ブロック終了したので、8バイト進める
        dst += 8;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encodeBlock2x4(u8* dst)
    {
        ModeBase::setFlipBit(dst, 1);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        {
            TexelF32 texel1, texel2;
            u8 rgb4bit1[3];
            u8 rgb4bit2[3];
            u8 rgb5bit1[3];
            u8 rgb5bit2[3];

            calcAvg2x4(texel1, 0, 8);
            calcAvg2x4(texel2, 8, 16);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit1, texel1);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit2, texel2);


            //サブブロック１との差をとる
            s32 diff[3];
            for(s32 i=0; i<3; ++i){
                diff[i] = rgb5bit2[i];
                diff[i] -= rgb5bit1[i];
            }


            // 差が4ビット以内ならDifferentialモード
            if((-4<=diff[0]) && (diff[0]<=3)
                && (-4<=diff[1]) && (diff[1]<=3)
                && (-4<=diff[2]) && (diff[2]<=3))
            {
                ModeBase::setDiffBit(dst, 1);

                DifferentialMode::setBaseColor1(dst, rgb5bit1[0], rgb5bit1[1], rgb5bit1[2]);
                DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

                s32 tmp;
                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb5bit1[i]<<3) | (rgb5bit1[i]>>2);

                    tmp = rgb5bit1[i];
                    tmp += diff[i];
                    tmp = lmath::clamp(tmp, 0, 255);

                    rgb2[i] = tmp;
                    rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
                }

            }else{
                quantizeTo444(rgb4bit1, texel1);
                quantizeTo444(rgb4bit2, texel2);

                ModeBase::setDiffBit(dst, 0);
                IndividualMode::setBaseColor1(dst, rgb4bit1[0], rgb4bit1[1], rgb4bit1[2]);
                IndividualMode::setBaseColor2(dst, rgb4bit2[0], rgb4bit2[1], rgb4bit2[2]);

                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb4bit1[i]<<4) | rgb4bit1[i];
                    rgb2[i] = (rgb4bit2[i]<<4) | rgb4bit2[i];
                }
            }

        }

        f32 error = 0.0f;
        {
            u8 min_table;
            error = encode2x4(dst, min_table, rgb1, 0, 8);
            ModeBase::setTableCodeWord1(dst, min_table);

            error += encode2x4(dst, min_table, rgb2, 8, 16);
            ModeBase::setTableCodeWord2(dst, min_table);
        }
        return error;
    }


    //---------------------------------------------------------------------
    f32 ETC1Codec::encodeBlock2x4avg(u8* dst)
    {
        ModeBase::setFlipBit(dst, 1);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        TexelF32 texel1, texel2;
        s32 diff[3];

        calcAvg2x4(texel1, 0, 8);
        calcAvg2x4(texel2, 8, 16);

        rgb1[0] = static_cast<u8>( texel1.r_*(31.0f/255.0f) );
        rgb1[1] = static_cast<u8>( texel1.g_*(31.0f/255.0f) );
        rgb1[2] = static_cast<u8>( texel1.b_*(31.0f/255.0f) );

        rgb2[0] = static_cast<u8>( texel2.r_*(31.0f/255.0f) );
        rgb2[1] = static_cast<u8>( texel2.g_*(31.0f/255.0f) );
        rgb2[2] = static_cast<u8>( texel2.b_*(31.0f/255.0f) );

        //サブブロック１との差をとる
        for(s32 i=0; i<3; ++i){
            diff[i] = rgb2[i];
            diff[i] -= rgb1[i];
        }

        // 差が4ビット以内ならDifferentialモード
        if((-4<=diff[0]) && (diff[0]<=3)
            && (-4<=diff[1]) && (diff[1]<=3)
            && (-4<=diff[2]) && (diff[2]<=3))
        {
            ModeBase::setDiffBit(dst, 1);

            DifferentialMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

            s32 tmp;
            for(s32 i=0; i<3; ++i){
                tmp = rgb1[i];
                tmp += diff[i];
                tmp = lmath::clamp(tmp, 0, 255);

                rgb1[i] = (rgb1[i]<<3) | (rgb1[i]>>2);

                rgb2[i] = tmp;
                rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
            }

        }else{
            rgb1[0] = static_cast<u8>( texel1.r_*(1.0f/17.0f) + 0.5001f);
            rgb1[1] = static_cast<u8>( texel1.g_*(1.0f/17.0f) + 0.5001f);
            rgb1[2] = static_cast<u8>( texel1.b_*(1.0f/17.0f) + 0.5001f);

            rgb2[0] = static_cast<u8>( texel2.r_*(1.0f/17.0f) + 0.5001f);
            rgb2[1] = static_cast<u8>( texel2.g_*(1.0f/17.0f) + 0.5001f);
            rgb2[2] = static_cast<u8>( texel2.b_*(1.0f/17.0f) + 0.5001f);

            ModeBase::setDiffBit(dst, 0);
            IndividualMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            IndividualMode::setBaseColor2(dst, rgb2[0], rgb2[1], rgb2[2]);

            for(s32 i=0; i<3; ++i){
                rgb1[i] = (rgb1[i]<<4) | rgb1[i];
                rgb2[i] = (rgb2[i]<<4) | rgb2[i];
            }
        }

        f32 error = 0.0f;

        u8 min_table;
        error = encode2x4(dst, min_table, rgb1, 0, 8);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode2x4(dst, min_table, rgb2, 8, 16);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encodeBlock4x2(u8* dst)
    {
        ModeBase::setFlipBit(dst, 0);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        {
            TexelF32 texel1, texel2;
            u8 rgb4bit1[3];
            u8 rgb4bit2[3];
            u8 rgb5bit1[3];
            u8 rgb5bit2[3];

            calcAvg4x2(texel1, 0, 2);
            calcAvg4x2(texel2, 2, 4);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit1, texel1);

            //-------------------------------------------------------
            quantizeTo555(rgb5bit2, texel2);


            //サブブロック１との差をとる
            s32 diff[3];
            for(s32 i=0; i<3; ++i){
                diff[i] = rgb5bit2[i];
                diff[i] -= rgb5bit1[i];
            }


            // 差が4ビット以内ならDifferentialモード
            if((-4<=diff[0]) && (diff[0]<=3)
                && (-4<=diff[1]) && (diff[1]<=3)
                && (-4<=diff[2]) && (diff[2]<=3))
            {
                ModeBase::setDiffBit(dst, 1);

                DifferentialMode::setBaseColor1(dst, rgb5bit1[0], rgb5bit1[1], rgb5bit1[2]);
                DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

                s32 tmp;
                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb5bit1[i]<<3) | (rgb5bit1[i]>>2);
                    tmp = rgb5bit1[i];
                    tmp += diff[i];
                    tmp = lmath::clamp(tmp, 0, 255);

                    rgb2[i] = tmp;
                    rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
                }

            }else{
                quantizeTo444(rgb4bit1, texel1);
                quantizeTo444(rgb4bit2, texel2);

                ModeBase::setDiffBit(dst, 0);
                IndividualMode::setBaseColor1(dst, rgb4bit1[0], rgb4bit1[1], rgb4bit1[2]);
                IndividualMode::setBaseColor2(dst, rgb4bit2[0], rgb4bit2[1], rgb4bit2[2]);

                for(s32 i=0; i<3; ++i){
                    rgb1[i] = (rgb4bit1[i]<<4) | rgb4bit1[i];
                    rgb2[i] = (rgb4bit2[i]<<4) | rgb4bit2[i];
                }
            }

        }

        f32 error = 0.0f;
        {
            u8 min_table;
            error = encode4x2(dst, min_table, rgb1, 0, 2);
            ModeBase::setTableCodeWord1(dst, min_table);

            error += encode4x2(dst, min_table, rgb2, 2, 4);
            ModeBase::setTableCodeWord2(dst, min_table);
        }
        return error;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encodeBlock4x2avg(u8* dst)
    {
        ModeBase::setFlipBit(dst, 0);

        //エンコード後にデコードしたRGB
        u8 rgb1[3];
        u8 rgb2[3];

        TexelF32 texel1, texel2;

        calcAvg4x2(texel1, 0, 2);
        calcAvg4x2(texel2, 2, 4);

        rgb1[0] = static_cast<u8>( texel1.r_*(31.0f/255.0f) );
        rgb1[1] = static_cast<u8>( texel1.g_*(31.0f/255.0f) );
        rgb1[2] = static_cast<u8>( texel1.b_*(31.0f/255.0f) );

        rgb2[0] = static_cast<u8>( texel2.r_*(31.0f/255.0f) );
        rgb2[1] = static_cast<u8>( texel2.g_*(31.0f/255.0f) );
        rgb2[2] = static_cast<u8>( texel2.b_*(31.0f/255.0f) );


        //サブブロック１との差をとる
        s32 diff[3];
        for(s32 i=0; i<3; ++i){
            diff[i] = rgb2[i];
            diff[i] -= rgb1[i];
        }


        // 差が4ビット以内ならDifferentialモード
        if((-4<=diff[0]) && (diff[0]<=3)
            && (-4<=diff[1]) && (diff[1]<=3)
            && (-4<=diff[2]) && (diff[2]<=3))
        {
            ModeBase::setDiffBit(dst, 1);

            DifferentialMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            DifferentialMode::setBaseColor2(dst, diff[0], diff[1], diff[2]);

            s32 tmp;
            for(s32 i=0; i<3; ++i){
                tmp = rgb1[i];
                tmp += diff[i];
                tmp = lmath::clamp(tmp, 0, 255);

                rgb1[i] = (rgb1[i]<<3) | (rgb1[i]>>2);

                rgb2[i] = tmp;
                rgb2[i] = (rgb2[i]<<3) | (rgb2[i]>>2);
            }

        }else{
            rgb1[0] = static_cast<u8>( texel1.r_*(1.0f/17.0f) + 0.5001f);
            rgb1[1] = static_cast<u8>( texel1.g_*(1.0f/17.0f) + 0.5001f);
            rgb1[2] = static_cast<u8>( texel1.b_*(1.0f/17.0f) + 0.5001f);

            rgb2[0] = static_cast<u8>( texel2.r_*(1.0f/17.0f) + 0.5001f);
            rgb2[1] = static_cast<u8>( texel2.g_*(1.0f/17.0f) + 0.5001f);
            rgb2[2] = static_cast<u8>( texel2.b_*(1.0f/17.0f) + 0.5001f);


            ModeBase::setDiffBit(dst, 0);
            IndividualMode::setBaseColor1(dst, rgb1[0], rgb1[1], rgb1[2]);
            IndividualMode::setBaseColor2(dst, rgb2[0], rgb2[1], rgb2[2]);

            for(s32 i=0; i<3; ++i){
                rgb1[i] = (rgb1[i]<<4) | rgb1[i];
                rgb2[i] = (rgb2[i]<<4) | rgb2[i];
            }
        }


        f32 error = 0.0f;

        u8 min_table;
        error = encode4x2(dst, min_table, rgb1, 0, 2);
        ModeBase::setTableCodeWord1(dst, min_table);

        error += encode4x2(dst, min_table, rgb2, 2, 4);
        ModeBase::setTableCodeWord2(dst, min_table);

        return error;
    }

    //---------------------------------------------------------------------
    void ETC1Codec::calcAvg2x4(TexelF32& texel, u32 sx, u32 ex)
    {
        texel.set(0.0f, 0.0f, 0.0f);
        for(u32 i=sx; i<ex; ++i){
            texel.r_ += static_cast<f32>( texels_[i].r_ );
            texel.g_ += static_cast<f32>( texels_[i].g_ );
            texel.b_ += static_cast<f32>( texels_[i].b_ );
        }

        texel.mul(OneEight);
    }


    //---------------------------------------------------------------------
    void ETC1Codec::calcAvg4x2(TexelF32& texel, u32 sx, u32 ex)
    {
        texel.set(0.0f, 0.0f, 0.0f);
        u32 k = 0;
        for(u32 i=sx; i<ex; ++i){
            for(u32 j=0; j<4; ++j){
                k = i + (j<<2);

                texel.r_ += static_cast<f32>( texels_[k].r_ );
                texel.g_ += static_cast<f32>( texels_[k].g_ );
                texel.b_ += static_cast<f32>( texels_[k].b_ );
            }
        }

        texel.mul(OneEight);
    }

    //---------------------------------------------------------------------
    void ETC1Codec::quantizeTo444(u8* encoded, const TexelF32& avg)
    {
        static const f32 ratio = (15.0f/255.0f);
        s32 low4bit[3];
        s32 high4bit[3];
        s32 low[3];
        s32 high[3];


        low4bit[0] = static_cast<s32>(avg.r_ * ratio);
        low4bit[1] = static_cast<s32>(avg.g_ * ratio);
        low4bit[2] = static_cast<s32>(avg.b_ * ratio);
        for(s32 i=0; i<3; ++i){
            high4bit[i] = (low4bit[i]>15)? 15 : (low4bit[i] + 1);

            low[i] = (low4bit[i] << 4) | low4bit[i];
            high[i] = (high4bit[i] << 4) | high4bit[i];
        }

        quantize(encoded, avg, low, high, low4bit, high4bit);
    }

    //---------------------------------------------------------------------
    void ETC1Codec::quantizeTo555(u8* encoded, const TexelF32& avg)
    {
        static const f32 ratio = (31.0f/255.0f);
        s32 low5bit[3];
        s32 high5bit[3];
        s32 low[3];
        s32 high[3];


        low5bit[0] = static_cast<s32>(avg.r_ * ratio);
        low5bit[1] = static_cast<s32>(avg.g_ * ratio);
        low5bit[2] = static_cast<s32>(avg.b_ * ratio);
        for(s32 i=0; i<3; ++i){
            high5bit[i] = (low5bit[i]>31)? 31 : (low5bit[i] + 1);

            low[i] = (low5bit[i] << 3) | (low5bit[i]>>2);
            high[i] = (high5bit[i] << 3) | (high5bit[i]>>2);
        }

        quantize(encoded, avg, low, high, low5bit, high5bit);
    }

    //---------------------------------------------------------------------
    void ETC1Codec::quantize(u8* encoded, const TexelF32& avg, const s32* low, const s32* high, const s32* lbit, const s32* hbit)
    {
        f32 k[3];
        f32 d[3];

        k[0] = static_cast<f32>(high[0] - low[0]);
        k[1] = static_cast<f32>(high[1] - low[1]);
        k[2] = static_cast<f32>(high[2] - low[2]);

        d[0] = low[0] - avg.r_;
        d[1] = low[1] - avg.g_;
        d[2] = low[2] - avg.b_;

        const f32 RG = WeightR * WeightG;
        const f32 GB = WeightG * WeightB;
        const f32 BR = WeightB * WeightR;

        f32 error[8];
        error[0] = RG*sqr((d[0] + 0.0f) - (d[1] + 0.0f)) + BR*sqr((d[2] + 0.0f) - (d[0] + 0.0f)) + GB*sqr((d[1] + 0.0f) - (d[2] + 0.0f));
        error[1] = RG*sqr((d[0] + k[0]) - (d[1] + 0.0f)) + BR*sqr((d[2] + 0.0f) - (d[0] + k[0])) + GB*sqr((d[1] + 0.0f) - (d[2] + 0.0f));
        error[2] = RG*sqr((d[0] + 0.0f) - (d[1] + k[1])) + BR*sqr((d[2] + 0.0f) - (d[0] + 0.0f)) + GB*sqr((d[1] + k[1]) - (d[2] + 0.0f));
        error[3] = RG*sqr((d[0] + 0.0f) - (d[1] + 0.0f)) + BR*sqr((d[2] + k[2]) - (d[0] + 0.0f)) + GB*sqr((d[1] + 0.0f) - (d[2] + k[2]));
        error[4] = RG*sqr((d[0] + k[0]) - (d[1] + k[1])) + BR*sqr((d[2] + 0.0f) - (d[0] + k[0])) + GB*sqr((d[1] + k[1]) - (d[2] + 0.0f));
        error[5] = RG*sqr((d[0] + k[0]) - (d[1] + 0.0f)) + BR*sqr((d[2] + k[2]) - (d[0] + k[0])) + GB*sqr((d[1] + 0.0f) - (d[2] + k[2]));
        error[6] = RG*sqr((d[0] + 0.0f) - (d[1] + k[1])) + BR*sqr((d[2] + k[2]) - (d[0] + 0.0f)) + GB*sqr((d[1] + k[1]) - (d[2] + k[2]));
        error[7] = RG*sqr((d[0] + k[0]) - (d[1] + k[1])) + BR*sqr((d[2] + k[2]) - (d[0] + k[0])) + GB*sqr((d[1] + k[1]) - (d[2] + k[2]));

        s32 min_index = 0;
        f32 min_error = error[0];
        for(s32 i=1; i<8; ++i){
            if(error[i]<min_error){
                min_error = error[i];
                min_index = i;
            }
        }

        switch(min_index)
        {
        case 0:
            encoded[0] = lbit[0];
            encoded[1] = lbit[1];
            encoded[2] = lbit[2];
            break;

        case 1:
            encoded[0] = hbit[0];
            encoded[1] = lbit[1];
            encoded[2] = lbit[2];
            break;

        case 2:	
            encoded[0] = lbit[0];
            encoded[1] = hbit[1];
            encoded[2] = lbit[2];
            break;

        case 3:	
            encoded[0] = lbit[0];
            encoded[1] = lbit[1];
            encoded[2] = hbit[2];
            break;

        case 4:	
            encoded[0] = hbit[0];
            encoded[1] = hbit[1];
            encoded[2] = lbit[2];
            break;

        case 5:	
            encoded[0] = hbit[0];
            encoded[1] = lbit[1];
            encoded[2] = hbit[2];
            break;

        case 6:	
            encoded[0] = lbit[0];
            encoded[1] = hbit[1];
            encoded[2] = hbit[2];
            break;

        case 7:	
            encoded[0] = hbit[0];
            encoded[1] = hbit[1];
            encoded[2] = hbit[2];
            break;

        default:
            break;
        }
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encode2x4(u8* dst, u8& min_table, const u8* rgb, u32 sx, u32 ex)
    {
        f32 min_error = 255*255*3*16;
        min_table = 0;

        u8 msb[2];
        u8 lsb[2];
        u8 best_msb[2];
        u8 best_lsb[2];

        for(u8 i=0; i<8; ++i){

#if 1
            msb[0] = dst[4];
            msb[1] = dst[5];
            lsb[0] = dst[6];
            lsb[1] = dst[7];
#else
            msb[1] = dst[4];
            msb[0] = dst[5];
            lsb[1] = dst[6];
            lsb[0] = dst[7];
#endif

            f32 error = encode2x4(msb, lsb, i, rgb, sx, ex);
            if(error < min_error){
                min_error = error;
                min_table = i;
                best_msb[0] = msb[0]; best_msb[1] = msb[1];
                best_lsb[0] = lsb[0]; best_lsb[1] = lsb[1];
            }
        }

#if 1
        dst[4] = best_msb[0];
        dst[5] = best_msb[1];
        dst[6] = best_lsb[0];
        dst[7] = best_lsb[1];
#else
        dst[4] = best_msb[1];
        dst[5] = best_msb[0];
        dst[6] = best_lsb[1];
        dst[7] = best_lsb[1];
#endif
        return min_error;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encode2x4(u8* msb, u8* lsb, u8 table, const u8* rgb, u32 sx, u32 ex)
    {
        s32 candidate[3];
        f32 min_error;
        f32 sum_error = 0.0f;
        s32 pix;
        for(u32 i=sx; i<ex; ++i){
            const Texel& texel = texels_[i];

            min_error = 255.0f*255.0f*3.0f*16.0f;
            pix = 0;

            for(s32 j=0; j<4; ++j){
                candidate[0] = lmath::clamp( ModifierTable[table][j] + rgb[0], 0, 255 );
                candidate[1] = lmath::clamp( ModifierTable[table][j] + rgb[1], 0, 255 );
                candidate[2] = lmath::clamp( ModifierTable[table][j] + rgb[2], 0, 255 );
                f32 err = WeightR* sqr( candidate[0] - texel.r_ );
                err += WeightG* sqr(candidate[1] - texel.g_);
                err += WeightB* sqr(candidate[2] - texel.b_);

                if(err<min_error){
                    min_error = err;
                    pix = j;
                }
            }

            u8 y = i >> 2;
            u8 x = i - (y<<2);
            u8 p = (x << 2) + y;
            ModeBase::setMSB(msb, p, pix);
            ModeBase::setLSB(lsb, p, pix);

            sum_error += min_error;
        }
        return sum_error;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encode4x2(u8* dst, u8& min_table, const u8* rgb, u32 sx, u32 ex)
    {
        f32 min_error = 255*255*3*16;
        min_table = 0;

        u8 msb[2];
        u8 lsb[2];
        u8 best_msb[2];
        u8 best_lsb[2];

        for(u8 i=0; i<8; ++i){
#if 1
            msb[0] = dst[4];
            msb[1] = dst[5];
            lsb[0] = dst[6];
            lsb[1] = dst[7];
#else
            msb[1] = dst[4];
            msb[0] = dst[5];
            lsb[1] = dst[6];
            lsb[0] = dst[7];
#endif
            f32 error = encode4x2(msb, lsb, i, rgb, sx, ex);
            if(error < min_error){
                min_error = error;
                min_table = i;
                best_msb[0] = msb[0]; best_msb[1] = msb[1];
                best_lsb[0] = lsb[0]; best_lsb[1] = lsb[1];
            }
        }
#if 1
        dst[4] = best_msb[0];
        dst[5] = best_msb[1];
        dst[6] = best_lsb[0];
        dst[7] = best_lsb[1];
#else
        dst[4] = best_msb[1];
        dst[5] = best_msb[0];
        dst[6] = best_lsb[1];
        dst[7] = best_lsb[1];
#endif

        return min_error;
    }

    //---------------------------------------------------------------------
    f32 ETC1Codec::encode4x2(u8* msb, u8* lsb, u8 table, const u8* rgb, u32 sx, u32 ex)
    {
        s32 candidate[3];
        f32 min_error;
        f32 sum_error = 0.0f;
        s32 pix;
        u32 index = 0;
        for(u32 i=sx; i<ex; ++i){
            for(u32 j=0; j<4; ++j){
                index = i + (j<<2);

                const Texel& texel = texels_[index];

                min_error = 255.0f*255.0f*3.0f*16.0f;
                pix = 0;

                for(s32 k=0; k<4; ++k){
                    candidate[0] = lmath::clamp( ModifierTable[table][k] + rgb[0], 0, 255 );
                    candidate[1] = lmath::clamp( ModifierTable[table][k] + rgb[1], 0, 255 );
                    candidate[2] = lmath::clamp( ModifierTable[table][k] + rgb[2], 0, 255 );
                    f32 err = WeightR* sqr(candidate[0] - texel.r_);
                    err += WeightG* sqr(candidate[1] - texel.g_);
                    err += WeightB* sqr(candidate[2] - texel.b_);

                    if(err<min_error){
                        min_error = err;
                        pix = k;
                    }
                }

                u8 y = index >> 2;
                u8 x = index - (y<<2);
                u8 p = (x << 2) + y;
                ModeBase::setMSB(msb, p, pix);
                ModeBase::setLSB(lsb, p, pix);

                sum_error += min_error;
            }
        }
        return sum_error;
    }
#endif
}
}
