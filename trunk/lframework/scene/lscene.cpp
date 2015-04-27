/**
@file lscene.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "lscene.h"
#include "SystemBase.h"
#include <lmath/lmathcore.h>

namespace lscene
{
    lcore::AllocFunc SceneAllocator::malloc_ = NULL;
    lcore::AllocFuncDebug SceneAllocator::mallocDebug_ = NULL;
    lcore::FreeFunc SceneAllocator::free_ = NULL;

    lcore::AlignedAllocFunc SceneAllocator::alignedMalloc_ = NULL;
    lcore::AlignedAllocFuncDebug SceneAllocator::alignedMallocDebug_ = NULL;
    lcore::AlignedFreeFunc SceneAllocator::alignedFree_ = NULL;

    void copySize16Times(void* dst, const void* src, s32 size)
    {
#ifdef LMATH_USE_SSE
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
#else
        lcore::memcpy(dst, src, size);
#endif
    }

    void copyAlign16DstOnlySize16Times(void* dst, const void* src, s32 size)
    {
#ifdef LMATH_USE_SSE
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
#else
        lcore::memcpy(dst, src, size);
#endif
    }

    void copyAlign16Size16Times(void* dst, const void* src, s32 size)
    {
#ifdef LMATH_USE_SSE
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
#else
        lcore::memcpy(dst, src, size);
#endif
    }
}
