#ifndef INC_LRENDERCORE_H__
#define INC_LRENDERCORE_H__
/**
@file lrendercore.h
@author t-sakai
@date 2009/05/01 create
*/
#include <lcore/lcore.h>
#include <lmath/lmath.h>

namespace lrender
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    using lmath::Vector2;
    using lmath::Vector3;
    using lmath::Vector4;
    using lmath::Matrix43;
    using lmath::Matrix44;

    extern const f32 RAY_EPSILON;//=1.0e-5f;

    extern const f32 FLOAT_INFINITY;//

    enum LightType
    {
        Light_Direction =0,
        Light_Num,
    };

    enum CameraType
    {
        Camera_Normal =0,
        Camera_Num,
    };

    enum SamplerType
    {
        Sampler_Grid =0,
        Sampler_Num,
    };

    enum BufferFormat
    {
        Buffer_D32 =0,
        Buffer_A32B32G32R32F,
        Buffer_R8G8B8,
        Buffer_A8R8G8B8,
        BufferFormat_Num,
    };


    s32 toGraphicsFormat(BufferFormat format);
}

#endif //INC_LRENDERCORE_H__
