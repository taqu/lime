#ifndef INC_LFRAMEWORK_H__
#define INC_LFRAMEWORK_H__
/**
@file lframework.h
@author t-sakai
@date 2016/05/08 create
*/
#include <lcore/lcore.h>
#include "Config.h"

namespace lmath
{
    class Vector4;
    class Matrix44;
}

namespace lfw
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::s64;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::u64;
    using lcore::f32;
    using lcore::f64;
    using lcore::Char;

    enum Layer
    {
        Layer_Default = 0x01U<<0,
    };

    enum ResourceType
    {
        ResourceType_Bytes,
        ResourceType_Texture2D,
        ResourceType_Model,
        ResourceType_Animation,
    };

    enum LightType
    {
        LightType_Direction,
        LightType_Point,
        LightType_Spot,
        LightType_Area,
    };

    enum RenderPath
    {
        RenderPath_Shadow =0,
        RenderPath_Opaque,
        RenderPath_Transparent,
        RenderPath_Num,
    };

    enum ClearType
    {
        ClearType_None =0,
        ClearType_Depth,
        ClearType_Color,
        ClearType_SkyBox,
    };

    enum RenderType
    {
        RenderType_Forward,
        RenderType_Deferred,
    };

    enum ShadowType
    {
        ShadowType_None =0,
        ShadowType_ShadowMap,
    };

    extern const Char* ImageExtension_DDS;
    extern const Char* ImageExtension_PNG;
    extern const Char* ImageExtension_BMP;
    extern const Char* ImageExtension_TGA;

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 
    @param src ... 
    @param size ... 16の倍数
    */
    void copySize16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 16バイトアライン
    @param src ... 
    @param size ... 16の倍数
    */
    void copyAlignedDst16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 
    @param src ... 16バイトアライン
    @param size ... 16の倍数
    */
    void copyAlignedSrc16(void* dst, const void* src, s32 size);

    /**
    @brief 16の倍数のサイズのデータコピー
    @param dst ... 16バイトアライン
    @param src ... 16バイトアライン
    @param size ... 16の倍数
    */
    void copyAlignedDstAlignedSrc16(void* dst, const void* src, s32 size);

    /**
    @brief
    @param dst ... 16バイトアライン
    @param src ... 
    */
    void copyAlignedDst16(lmath::Vector4& dst, const lmath::Vector4& src);

    /**
    @brief マトリックスコピー
    @param dst ... 16バイトアライン
    @param src ... 
    */
    void copyAlignedDst16(lmath::Matrix44& dst, const lmath::Matrix44& src);

    u64 calcHash(const Char* path);
}
#endif //INC_LFRAMEWORK_H__
