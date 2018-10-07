#ifndef INC_LMATH_CURLNOISE_H_
#define INC_LMATH_CURLNOISE_H_
/**
@file CurlNoise.h
@author t-sakai
@date 2017/08/20 create

"Curl noise for procedural fluid flow", R. Bridson, J. Hourihan, and M. Nordenstam, Proc. ACM SIGGRAPH 2007
*/
#include "lmath.h"

namespace lmath
{
    //--------------------------------------------------------------------
    //---
    //--- CurlNoise2
    //---
    //--------------------------------------------------------------------
    class CurlNoise2
    {
    public:
        CurlNoise2();
        virtual ~CurlNoise2();
        virtual bool seed(f32 dt) const;
        virtual f32 potential(f32 x, f32 y) const;
        virtual void advance(f32 dt);
        virtual void getVelocity(Vector2& v, const Vector2& x) const;

    protected:
        f32 time_;
        f32 dx_;
    };

    //--------------------------------------------------------------------
    //---
    //--- CurlNoise3
    //---
    //--------------------------------------------------------------------
    class CurlNoise3
    {
    public:
        CurlNoise3();
        virtual ~CurlNoise3();
        virtual bool seed(f32 dt) const;
        virtual Vector3 potential(f32 x, f32 y, f32 z) const;
        virtual void advance(f32 dt);
        virtual void getVelocity(Vector3& v, const Vector3& x) const;

    protected:
        f32 time_;
        f32 dx_;
    };


    //--------------------------------------------------------------------
    //---
    //--- Example3DPlume
    //---
    //--------------------------------------------------------------------
#if 0
    class Example3DPlume : public CurlNoise3
    {
    public:
        Example3DPlume();
        virtual ~Example3DPlume();
        virtual bool seed(f32 dt) const;
        virtual Vector3 potential(f32 x, f32 y, f32 z) const;
        virtual void advance(f32 dt);
        virtual void getVelocity(Vector3& v, const Vector3& x) const;

        f32 noise0(f32 x, f32 y, f32 z) const;
        f32 noise1(f32 x, f32 y, f32 z) const;
        f32 noise2(f32 x, f32 y, f32 z) const;

        f32 distanceAndNormal(f32 x, f32 y, f32 z, Vector3& normal) const;
        void matchBoundary(Vector3& psi, f32 d, f32 lengthscale, const Vector3& normal) const;
    protected:
    };
#endif
}
#endif //INC_LMATH_CURLNOISE_H_
