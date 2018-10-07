/**
@file CurlNoise.cpp
@author t-sakai
@date 2017/08/20 create

"Curl noise for procedural fluid flow", R. Bridson, J. Hourihan, and M. Nordenstam, Proc. ACM SIGGRAPH 2007
*/
#include "CurlNoise.h"
#include "Vector2.h"
#include "Vector3.h"
#include <lcore/Noise.h>

namespace lmath
{
    //--------------------------------------------------------------------
    //---
    //--- CurlNoise2
    //---
    //--------------------------------------------------------------------
    CurlNoise2::CurlNoise2()
        :time_(0.0f)
        ,dx_(1.0e-5f)
    {}

    CurlNoise2::~CurlNoise2()
    {}

    bool CurlNoise2::seed(f32) const
    {
        return false;
    }

    f32 CurlNoise2::potential(f32, f32) const
    {
        return 0.0f;
    }

    void CurlNoise2::advance(f32 dt)
    {
        time_ += dt;
    }

    void CurlNoise2::getVelocity(Vector2& v, const Vector2& x) const
    {
        f32 inv = 1.0f/(2.0f*dx_);
        v[0]=-(potential(x[0], x[1]+dx_) - potential(x[0], x[1]-dx_))*inv;
        v[1]= (potential(x[0]+dx_, x[1]) - potential(x[0]-dx_, x[1]))*inv;
    }

    //--------------------------------------------------------------------
    //---
    //--- CurlNoise3
    //---
    //--------------------------------------------------------------------
    CurlNoise3::CurlNoise3()
        :time_(0.0f)
        ,dx_(1.0e-5f)
    {}

    CurlNoise3::~CurlNoise3()
    {}

    bool CurlNoise3::seed(f32) const
    {
        return false;
    }

    Vector3 CurlNoise3::potential(f32, f32, f32) const
    {
        return Vector3::construct(0.0f);
    }

    void CurlNoise3::advance(f32 dt)
    {
        time_ += dt;
    }

    void CurlNoise3::getVelocity(Vector3& v, const Vector3& x) const
    {
        f32 inv = 1.0f/(2.0f*dx_);
        v[0]=( (potential(x[0], x[1]+dx_, x[2])[2] - potential(x[0], x[1]-dx_, x[2])[2])
            -(potential(x[0], x[1], x[2]+dx_)[1] - potential(x[0], x[1], x[2]-dx_)[1]) ) *inv;
        v[1]=( (potential(x[0], x[1], x[2]+dx_)[0] - potential(x[0], x[1], x[2]-dx_)[0])
            -(potential(x[0]+dx_, x[1], x[2])[2] - potential(x[0]-dx_, x[1], x[2])[2]) ) *inv;
        v[2]=( (potential(x[0]+dx_, x[1], x[2])[1] - potential(x[0]-dx_, x[1], x[2])[1])
            -(potential(x[0], x[1]+dx_, x[2])[0] - potential(x[0], x[1]-dx_, x[2])[0]) ) *inv;
    }

    //--------------------------------------------------------------------
    //---
    //--- Example3DPlume
    //---
    //--------------------------------------------------------------------
#if 0
    Example3DPlume::Example3DPlume();
    Example3DPlume::~Example3DPlume();
    bool Example3DPlume::seed(f32 dt) const;
    Vector3 Example3DPlume::potential(f32 x, f32 y, f32 z) const;
    void Example3DPlume::advance(f32 dt);
    void Example3DPlume::getVelocity(Vector3& v, const Vector3& x) const;

    f32 Example3DPlume::noise0(f32 x, f32 y, f32 z) const
    {
        return noise(x, y, z);
    }

    f32 Example3DPlume::noise1(f32 x, f32 y, f32 z) const
    {
        return noise(y+31.416f, z-47.853f, x+12.793f);
    }

    f32 Example3DPlume::noise2(f32 x, f32 y, f32 z) const
    {
        return noise(z-233.145f, x-113.408f, y-185.31f);
    }

    f32 Example3DPlume::distanceAndNormal(f32 x, f32 y, f32 z, Vector3& normal) const
    {
    }

    void Example3DPlume::matchBoundary(Vector3& psi, f32 d, f32 lengthscale, const Vector3& normal) const
    {
    }
#endif
}
