/**
@file lframework.cpp
@author t-sakai
@date 2016/11/22 create
*/
#include "lframework.h"
#include <lmath/lmath.h>
#include <lcore/xxHash.h>

namespace lfw
{
    const Char* ImageExtension_DDS = "dds";
    const Char* ImageExtension_PNG = "png";
    const Char* ImageExtension_BMP = "bmp";
    const Char* ImageExtension_TGA = "tga";

    void copySize16(void* dst, const void* src, s32 size)
    {
        f32* d = reinterpret_cast<f32*>(dst);
        const f32* s = reinterpret_cast<const f32*>(src);

        s32 count = size>>4;
        LASSERT((size-(count<<4))==0);

        for(s32 i=0; i<count; ++i){
            lmath::lm128 v0 = _mm_loadu_ps(s);
            _mm_storeu_ps(d, v0);
            s+=4;
            d+=4;
        }
    }

    void copyAlignedDst16(void* dst, const void* src, s32 size)
    {
        f32* d = reinterpret_cast<f32*>(dst);
        const f32* s = reinterpret_cast<const f32*>(src);

        s32 count = size>>4;
        LASSERT((size-(count<<4))==0);

        for(s32 i=0; i<count; ++i){
            lmath::lm128 v0 = _mm_loadu_ps(s);
            _mm_store_ps(d, v0);
            s+=4;
            d+=4;
        }
    }

    void copyAlignedSrc16(void* dst, const void* src, s32 size)
    {
        f32* d = reinterpret_cast<f32*>(dst);
        const f32* s = reinterpret_cast<const f32*>(src);

        s32 count = size>>4;
        LASSERT((size-(count<<4))==0);

        for(s32 i=0; i<count; ++i){
            lmath::lm128 v0 = _mm_load_ps(s);
            _mm_storeu_ps(d, v0);
            s+=4;
            d+=4;
        }
    }

    void copyAlignedDstAlignedSrc16(void* dst, const void* src, s32 size)
    {
        f32* d = reinterpret_cast<f32*>(dst);
        const f32* s = reinterpret_cast<const f32*>(src);

        s32 count = size>>4;
        LASSERT((size-(count<<4))==0);

        for(s32 i=0; i<count; ++i){
            lmath::lm128 v0 = _mm_load_ps(s);
            _mm_store_ps(d, v0);
            s+=4;
            d+=4;
        }
    }

    //
    void copyAlignedDst16(lmath::Vector4& dst, const lmath::Vector4& src)
    {
        f32* d = reinterpret_cast<f32*>(&dst);
        const f32* s = reinterpret_cast<const f32*>(&src);
        _mm_store_ps(&d[0], _mm_loadu_ps(&s[0]));
    }

    // マトリックスコピー
    void copyAlignedDst16(lmath::Matrix44& dst, const lmath::Matrix44& src)
    {
        f32* d = reinterpret_cast<f32*>(&dst);
        const f32* s = reinterpret_cast<const f32*>(&src);
        _mm_store_ps(&d[0], _mm_loadu_ps(&s[0]));
        _mm_store_ps(&d[4], _mm_loadu_ps(&s[4]));
        _mm_store_ps(&d[8], _mm_loadu_ps(&s[8]));
        _mm_store_ps(&d[12], _mm_loadu_ps(&s[12]));
    }

    u64 calcHash(const Char* path)
    {
        LASSERT(NULL != path);
        return lcore::xxHash64(reinterpret_cast<const u8*>(path), lcore::strlen_s32(path));
    }
}
