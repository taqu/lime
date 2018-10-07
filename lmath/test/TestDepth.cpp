#include <catch_wrap.hpp>
#include <lcore/Random.h>
#include <lcore/File.h>
#include "lmath.h"
#include "Vector4.h"
#include "Matrix44.h"

namespace lmath
{
    TEST_CASE("TestDepth::TestReverseZ")
    {
        const f32 fovy = DEG_TO_RAD*60.0f;
        const f32 znear = 0.1f;
        const f32 zfar = 1.0f;
        const f32 aspect = 4.0f/3.0f;
        const s32 numSamples = 1024;
        Matrix44 projZ;
        projZ.perspectiveFov(fovy, aspect, znear, zfar);
        Matrix44 projRZ;
        projRZ.perspectiveFovReverseZ(fovy, aspect, znear, zfar);

        const f32 zrange = zfar-znear;
        for(s32 i=0; i<numSamples; ++i){
            f32 z = zrange/numSamples*i + znear;
            Vector4 p = Vector4::construct(0.0f, 0.0f, z, 1.0f);
            Vector4 pz = Vector4::construct(mul(projZ, p));
            pz /= pz.w_;
            Vector4 prz = Vector4::construct(mul(projRZ, p));
            prz /= prz.w_;

            prz.z_ = lcore::clamp01(prz.z_);
            CHECK((0.0f<=pz.z_ && pz.z_<=1.0f));
            CHECK((0.0f<=prz.z_ && prz.z_<=1.0f));

            s32* psz = reinterpret_cast<s32*>(&pz.z_);
            s32* psrz = reinterpret_cast<s32*>(&prz.z_);
            s32 sz =  static_cast<s32>(0x7FFFFF & (*psz));
            s32 srz = static_cast<s32>(0x7FFFFF & (*psrz));
            LOG_INFO(z << ", " << sz << ", " << srz);
        }
    }

    TEST_CASE("TestDepth::TestLinearZ")
    {
        const f32 fovy = DEG_TO_RAD*60.0f;
        const f32 znear = 0.1f;
        const f32 zfar = 1000.0f;
        const f32 aspect = 4.0f/3.0f;
        const s32 numSamples = 1024;
        Matrix44 projRZ;
        projRZ.perspectiveFovReverseZ(fovy, aspect, znear, zfar);
        Vector4 linearZParam = getLinearZParameterReverseZ(znear, zfar);

        const f32 zrange = zfar-znear;
        for(s32 i=0; i<numSamples; ++i){
            f32 z = zrange/numSamples*i + znear;
            Vector4 p = Vector4::construct(0.0f, 0.0f, z, 1.0f);
            Vector4 prz = Vector4::construct(mul(projRZ, p));
            prz /= prz.w_;

            prz.z_ = lcore::clamp01(prz.z_);
            CHECK((0.0f<=prz.z_ && prz.z_<=1.0f));
            f32 linearZ = toLinearZ(prz.z_, linearZParam);
            LOG_INFO(z << ", " << prz.z_ << ", " << linearZ);
        }
    }
}
