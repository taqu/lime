/**
@file lrender.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include <stdio.h>

#include "lrender.h"
#include "core/AABB.h"
#include "core/Spectrum.h"

namespace lrender
{
    Vector3 normalize(const Vector3& v)
    {
        f32 il = 1.0f/v.length();
        return Vector3(v.x_*il, v.y_*il, v.z_*il);
    }

    Vector3 normalizeChecked(const Vector3& v)
    {
        f32 l = v.lengthSqr();
        if(l<DOT_EPSILON){
            return Vector3(0.0f);
        }
        f32 il = 1.0f/lmath::sqrt(l);
        return Vector3(v.x_*il, v.y_*il, v.z_*il);
    }

    f32 dot(const Vector2& v0, const Vector2& v1)
    {
        return v0.x_*v1.x_ + v0.y_*v1.y_;
    }

    f32 dot(const Vector3& v0, const Vector3& v1)
    {
        return v0.x_*v1.x_ + v0.y_*v1.y_ + v0.z_*v1.z_;
    }

    Vector3 cross(const Vector3& v0, const Vector3& v1)
    {
        f32 x = v0.y_*v1.z_ - v0.z_*v1.y_;
        f32 y = v0.z_*v1.x_ - v0.x_*v1.z_;
        f32 z = v0.x_*v1.y_ - v0.y_*v1.x_;
        return Vector3(x, y, z);
    }

    Vector4 cross(const Vector4& v0, const Vector4& v1)
    {
        f32 x = v0.y_*v1.z_ - v0.z_*v1.y_;
        f32 y = v0.z_*v1.x_ - v0.x_*v1.z_;
        f32 z = v0.x_*v1.y_ - v0.y_*v1.x_;
        return Vector4(x, y, z, 0.0f);
    }

    void getBBox(AABB& bbox, const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        bbox.bmin_ = p0;
        bbox.bmax_ = p0;
        bbox.bmin_.minimum(bbox.bmin_, p1);
        bbox.bmax_.maximum(bbox.bmax_, p1);
        bbox.bmin_.minimum(bbox.bmin_, p2);
        bbox.bmax_.maximum(bbox.bmax_, p2);
    }

    void getNormal(Vector3& normal, const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        Vector3 d0, d1;
        d0.sub(p1, p0);
        d1.sub(p2, p0);
        d0.normalizeChecked();
        d1.normalizeChecked();

        normal.cross(d0, d1);
        normal.normalizeChecked();
    }

    void orthonormalBasis(Vector3& binormal0, Vector3& binormal1, const Vector3& normal)
    {
        if(lcore::absolute(normal.y_)<lcore::absolute(normal.x_)){
            f32 invLen = 1.0f/lmath::sqrt(normal.x_*normal.x_ + normal.z_*normal.z_);
            binormal1.set(normal.z_*invLen, 0.0f, -normal.x_*invLen);

        }else{
            f32 invLen = 1.0f/lmath::sqrt(normal.y_*normal.y_ + normal.z_*normal.z_);
            binormal1.set(0.0f, normal.z_*invLen, -normal.y_*invLen);
        }
        binormal0.cross(binormal1, normal);
    }

    void orthonormalBasis(Vector4& binormal0, Vector4& binormal1, const Vector4& normal)
    {
        if(lcore::absolute(normal.y_)<lcore::absolute(normal.x_)){
            f32 invLen = 1.0f/lmath::sqrt(normal.x_*normal.x_ + normal.z_*normal.z_);
            binormal1.set(normal.z_*invLen, 0.0f, -normal.x_*invLen, 0.0f);

        }else{
            f32 invLen = 1.0f/lmath::sqrt(normal.z_*normal.z_ + normal.z_*normal.z_);
            binormal1.set(0.0f, normal.z_*invLen, -normal.y_*invLen, 0.0f);
        }
        binormal0 = cross(binormal1, normal);
    }

    Vector2 weightAverage(f32 w0, f32 w1, f32 w2, const Vector2& v0, const Vector2& v1, const Vector2& v2)
    {
        f32 x = w0*v0.x_ + w1*v1.x_ + w2*v2.x_;
        f32 y = w0*v0.y_ + w1*v1.y_ + w2*v2.y_;
        return Vector2(x, y);
    }

    Vector3 weightAverage(f32 w0, f32 w1, f32 w2, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        f32 x = w0*v0.x_ + w1*v1.x_ + w2*v2.x_;
        f32 y = w0*v0.y_ + w1*v1.y_ + w2*v2.y_;
        f32 z = w0*v0.z_ + w1*v1.z_ + w2*v2.z_;
        return Vector3(x, y, z);
    }

    //fresnel
    //------------------------------------------------------------------------------
    f32 fresnelForDielectic(f32 csI, f32 csT, f32 ri_int, f32 ri_ext)
    {
        if(lmath::isEqual(ri_int, ri_ext)){
            return 0.0f;
        }
        f32 ri_int_csI = ri_int*csI;
        f32 ri_ext_csT = ri_ext*csT;
        f32 ri_int_csT = ri_int*csT;
        f32 ri_ext_csI = ri_ext*csI;

        f32 rp = (ri_int_csI - ri_ext_csT)/(ri_int_csI + ri_ext_csT);
        f32 rs = (ri_ext_csI - ri_int_csT)/(ri_ext_csI + ri_int_csT);

        return 0.5f*(rs*rs + rp*rp);
    }

    f32 fresnelForDielecticRefract(f32& csOut, f32 cs, f32 ri_int, f32 ri_ext)
    {
        if(lmath::isEqual(ri_int, ri_ext)){
            csOut = -cs;
            return 0.0f;
        }
        f32 relativeRefractive = (0.0f<cs)? ri_int/ri_ext : ri_ext/ri_int;
        f32 sqrCsOut = 1.0f - (1.0f-cs*cs)*relativeRefractive*relativeRefractive;
        if(sqrCsOut<=0.0f){
            csOut = 0.0f;
            return 1.0f;
        }

        f32 csI = lcore::absolute(cs);
        f32 csT = lmath::sqrt(sqrCsOut);

        f32 ri_int_csI = ri_int*csI;
        f32 ri_ext_csT = ri_ext*csT;
        f32 ri_int_csT = ri_int*csT;
        f32 ri_ext_csI = ri_ext*csI;

        f32 rp = (ri_int_csI - ri_ext_csT)/(ri_int_csI + ri_ext_csT);
        f32 rs = (ri_ext_csI - ri_int_csT)/(ri_ext_csI + ri_int_csT);

        csOut = (0.0f<cs)? -csT : csT;
        return 0.5f*(rs*rs + rp*rp);
    }

    void directionToLightProbeCoord(f32& u, f32& v, f32 x, f32 y, f32 z)
    {
        f32 t = lmath::sqrt(x*x + y*y);
        f32 r = (lmath::isZeroPositive(t))? 0.0f : (1.0f/PI)*lmath::acos(z)/t;

        u = x * r * 0.5f + 0.5f;
        v = 0.5f - y * r * 0.5f;
    }

    void lightProbeUVToAngle(f32& theta, f32& phi, f32 u, f32 v)
    {
        u = 2.0f*u - 1.0f;
        v = 2.0f*v - 1.0f;

        f32 l = lmath::sqrt(u*u + v*v);
        phi = PI * l;
        theta = (lmath::isZeroPositive(l))? 0.0f : lmath::atan2(v, u);
    }

namespace
{
    static const f32 RGB2Lum[] = {0.299f, 0.587f, 0.114f, 0.0f};
    static const f32 RGB2Cb[] = {-0.16874f, -0.33126f, 0.5f, 0.0f};
    static const f32 RGB2Cr[] = {0.5f, -0.41869f, -0.08131f, 0.0f};

    static const f32 YCbCr2R[] = {1.0f, 0.0f, 1.402f, 0.0f};
    static const f32 YCbCr2G[] = {1.0f, -0.34414f, -0.71414f, 0.0f};
    static const f32 YCbCr2B[] = {1.0f, 1.772f, 0.0f, 0.0f};
}

    f32 calcLuminance(f32 r, f32 g, f32 b)
    {
        return r * RGB2Lum[0] + g * RGB2Lum[1] + b * RGB2Lum[2];
    }

    f32 calcCb(f32 r, f32 g, f32 b)
    {
        return r * RGB2Cb[0] + g * RGB2Cb[1] + b * RGB2Cb[2];
    }

    f32 calcCr(f32 r, f32 g, f32 b)
    {
        return r * RGB2Cr[0] + g * RGB2Cr[1] + b * RGB2Cr[2];
    }

    f32 calcR(f32 y, f32 cb, f32 cr)
    {
        return y + cb * YCbCr2R[1] + cr * YCbCr2R[2];
    }

    f32 calcG(f32 y, f32 cb, f32 cr)
    {
        return y + cb * YCbCr2G[1] + cr * YCbCr2G[2];
    }

    f32 calcB(f32 y, f32 cb, f32 cr)
    {
        return y + cb * YCbCr2B[1] + cr * YCbCr2B[2];
    }

    //--------------------------------------------------
    // Linear RGB to standard RGB
    void linearToStandardRGB(s32 width, s32 height, Color3* rgbs)
    {
        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                rgbs[index] = rgbs[index].linearToStandard();
            }
        }
    }

    void linearToStandardRGB(s32 width, s32 height, Color4* rgbs)
    {
        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                rgbs[index] = rgbs[index].linearToStandard();
            }
        }
    }

    // Standard RGB to linear RGB
    void standardToLinearRGB(s32 width, s32 height, Color3* rgbs)
    {
        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                rgbs[index] = rgbs[index].standardToLinear();
            }
        }
    }

    void standardToLinearRGB(s32 width, s32 height, Color4* rgbs)
    {
        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                rgbs[index] = rgbs[index].standardToLinear();
            }
        }
    }


    //--------------------------------------------------
    // tone-mapping 
    void toneMapping(s32 width, s32 height, Color3* rgbs, f32 scale)
    {
        f32 logAvg = 0.0f;
        f32 lumMax = TONEMAPPING_DELTA;
        for(s32 i=0; i<height; ++i){
            f32 sum = 0.0f;
            for(s32 j=0; j<width; ++j){
                const Color3& rgb = rgbs[i*width + j];

                f32 lum = calcLuminance(rgb[0], rgb[1], rgb[2]);

                lumMax = lcore::maximum(lumMax, lum);
                sum += lmath::log(TONEMAPPING_DELTA + lum);
            }

            logAvg += sum/width;
        }
        logAvg /= height;

        f32 coeff = scale * lmath::exp(-logAvg);
        lumMax *= coeff;
        f32 invLumMax2 = 1.0f/(lumMax*lumMax);

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                Color3& rgb = rgbs[i*width + j];

                f32 lum = calcLuminance(rgb[0], rgb[1], rgb[2]);
                f32 cb = calcCb(rgb[0], rgb[1], rgb[2]);
                f32 cr = calcCr(rgb[0], rgb[1], rgb[2]);

                lum *= coeff;
                lum = lum * (1.0f + lum*invLumMax2)/(1.0f+lum);

                rgb[0] = calcR(lum, cb, cr);
                rgb[1] = calcG(lum, cb, cr);
                rgb[2] = calcB(lum, cb, cr);
            }
        }
    }

    //--------------------------------------------------
    // tone-mapping 
    void toneMapping(s32 width, s32 height, Color4* rgbs, f32 scale)
    {
        f32 logAvg = 0.0f;
        f32 lumMax = TONEMAPPING_DELTA;
        for(s32 i=0; i<height; ++i){
            f32 sum = 0.0f;
            for(s32 j=0; j<width; ++j){
                const Color4& rgb = rgbs[i*width + j];

                f32 lum = calcLuminance(rgb[0], rgb[1], rgb[2]);

                lumMax = lcore::maximum(lumMax, lum);
                sum += lmath::log(TONEMAPPING_DELTA + lum);
            }

            logAvg += sum/width;
        }
        logAvg /= height;

        f32 coeff = scale * lmath::exp(-logAvg);
        lumMax *= coeff;
        f32 invLumMax2 = 1.0f/(lumMax*lumMax);

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                Color4& rgb = rgbs[i*width + j];

                f32 lum = calcLuminance(rgb[0], rgb[1], rgb[2]);
                f32 cb = calcCb(rgb[0], rgb[1], rgb[2]);
                f32 cr = calcCr(rgb[0], rgb[1], rgb[2]);

                lum *= coeff;
                lum = lum * (1.0f + lum*invLumMax2)/(1.0f+lum);

                rgb[0] = calcR(lum, cb, cr);
                rgb[1] = calcG(lum, cb, cr);
                rgb[2] = calcB(lum, cb, cr);
            }
        }
    }

    // tone-mapping
    void toneMapping(s32 width, s32 height, lmath::Vector4* data, f32 scale)
    {
        LIME_ALIGN16 f32 buff[4];

        lmath::lm128 rgb2Lum = _mm_loadu_ps(RGB2Lum);
        lmath::lm128 pixel;

        f32 logAvg = 0.0f;
        f32 lumMax = TONEMAPPING_DELTA;
        for(s32 i=0; i<height; ++i){
            f32 sum = 0.0f;
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;
                pixel = _mm_loadu_ps((const f32*)(data+index));
                _mm_store_ps(buff, _mm_mul_ps(pixel, rgb2Lum));

                f32 lum = buff[0] + buff[1] + buff[2];

                lumMax = lcore::maximum(lumMax, lum);
                sum += lmath::log(TONEMAPPING_DELTA + lum);
            }

            logAvg += sum/width;
        }
        logAvg /= height;

        f32 coeff = scale * lmath::exp(-logAvg);
        lumMax *= coeff;
        f32 invLumMax2 = 1.0f/(lumMax*lumMax);

        lmath::lm128 rgb2Cb = _mm_loadu_ps(RGB2Cb);
        lmath::lm128 rgb2Cr = _mm_loadu_ps(RGB2Cr);

        lmath::lm128 yCbCr2R = _mm_loadu_ps(YCbCr2R);
        lmath::lm128 yCbCr2G = _mm_loadu_ps(YCbCr2G);
        lmath::lm128 yCbCr2B = _mm_loadu_ps(YCbCr2B);

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                s32 index = i*width + j;

                pixel = _mm_loadu_ps((const f32*)(data+index));

                _mm_store_ps(buff, _mm_mul_ps(pixel, rgb2Lum));
                f32 lum = buff[0] + buff[1] + buff[2];

                _mm_store_ps(buff, _mm_mul_ps(pixel, rgb2Cb));
                f32 cb = buff[0] + buff[1] + buff[2];

                _mm_store_ps(buff, _mm_mul_ps(pixel, rgb2Cr));
                f32 cr = buff[0] + buff[1] + buff[2];

                lum *= coeff;
                lum = lum * (1.0f + lum*invLumMax2)/(1.0f+lum);

                buff[0] = lum;
                buff[1] = cb;
                buff[2] = cr;
                pixel = _mm_loadu_ps(buff);


                _mm_store_ps(buff, _mm_mul_ps(pixel, yCbCr2R));
                data[index].x_ = buff[0] + buff[1] + buff[2];

                _mm_store_ps(buff, _mm_mul_ps(pixel, yCbCr2G));
                data[index].y_ = buff[0] + buff[1] + buff[2];

                _mm_store_ps(buff, _mm_mul_ps(pixel, yCbCr2B));
                data[index].z_ = buff[0] + buff[1] + buff[2];
            }
        }
    }


namespace sh
{

namespace
{
    const f32 CartesianCoefficients[] =
    {
        lmath::sqrt(INV_PI) * 0.5f,

        lmath::sqrt(3.0f*INV_PI) * 0.5f,
        lmath::sqrt(3.0f*INV_PI) * 0.5f,
        lmath::sqrt(3.0f*INV_PI) * 0.5f,

        lmath::sqrt(15.0f*INV_PI) * 0.5f,
        lmath::sqrt(15.0f*INV_PI) * 0.5f,
        lmath::sqrt(5.0f*INV_PI) * 0.25f,
        lmath::sqrt(15.0f*INV_PI) * 0.5f,
        lmath::sqrt(15.0f*INV_PI) * 0.25f,

        0.590044f,
        2.89061f,
        0.457046f,
        0.373176f,
        0.457046f,
        1.44531f,
        0.590044f,
    };

    s32 fractional(s32 x)
    {
        s32 ret = 1;
        for(;1<x; --x){
            ret *= x;
        }
        return ret;
    }

    f64 calcK(s32 l, s32 m)
    {
        f64 t = ((2.0*l + 1.0) * fractional(l-m)) / (4.0*PI*fractional(l+m));
        return lmath::sqrt(t);
    }

    f64 calcP(s32 l, s32 m, f64 x)
    {
        f64 pmm = 1.0;
        if(0<m){
            f64 somx2 = lmath::sqrt(1.0-x*x);
            f64 fact = 1.0;
            for(s32 i=l; i<=m; ++i){
                pmm *= -fact*somx2;
                fact += 2.0;
            }
        }

        if(l == m){
            return pmm;
        }

        f64 pmmp1 = x*(2.0*m+1.0)*pmm;
        if(l == (m+1)){
            return pmmp1;
        }

        f64 pll = 0.0;
        for(s32 ll=m+2; ll<=l; ++ll){
            pll = ((2.0*ll - 1.0)*x*pmmp1 - (ll+m-1.0)*pmm)/(ll-m);
            pmm = pmmp1;
            pmmp1 = pll;
        }
        return pll;
    }
}


    f64 SH(s32 l, s32 m, f64 theta, f64 phi)
    {
        const f64 sqrt2 = lmath::sqrt(2.0);

        if(0 == m){
            return calcK(l, 0) * calcP(l, m, lmath::cos(theta));

        }else if(0<m){
            return sqrt2 * calcK(l,m) * lmath::cos(m*phi) * calcP(l, m, lmath::cos(theta));

        }else{
            m = -m;
            return sqrt2 * calcK(l,m) * lmath::sin(m*phi) * calcP(l, m, lmath::cos(theta));
        }
    }


    f32 SHCartesianCoefficient(s32 index)
    {
        return CartesianCoefficients[index];
    }

    void calcSHMatrix(lmath::Matrix44& mat, const f32* coefficient)
    {
        const f32 c1 = 0.429043f;
        const f32 c2 = 0.511664f;
        const f32 c3 = 0.743125f;
        const f32 c4 = 0.886227f;
        const f32 c5 = 0.247708f;
        mat.m_[0][0] = c1 * coefficient[8];
        mat.m_[0][1] = c1 * coefficient[4];
        mat.m_[0][2] = c1 * coefficient[7];
        mat.m_[0][3] = c2 * coefficient[3];

        mat.m_[1][0] = c1 * coefficient[4];
        mat.m_[1][1] = -c1 * coefficient[8];
        mat.m_[1][2] = c1 * coefficient[5];
        mat.m_[1][3] = c2 * coefficient[1];

        mat.m_[2][0] = c1 * coefficient[7];
        mat.m_[2][1] = c1 * coefficient[5];
        mat.m_[2][2] = c3 * coefficient[6];
        mat.m_[2][3] = c2 * coefficient[2];

        mat.m_[3][0] = c2 * coefficient[3];
        mat.m_[3][1] = c2 * coefficient[1];
        mat.m_[3][2] = c2 * coefficient[2];
        mat.m_[3][3] = c4 * coefficient[0] - c5 * coefficient[6];
    }
}

    bool savePPM(const char* filename, const Color3* rgb, s32 width, s32 height)
    {
        LASSERT(NULL != filename);
        LASSERT(NULL != rgb);

        FILE* file = NULL;
        fopen_s(&file, filename, "wb");
        if(NULL == file){
            return false;
        }

        fprintf(file, "P3\n");
        fprintf(file, "%d %d\n", width, height);
        fprintf(file, "255\n");

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                const Color3& pixel = rgb[width*i + j];
                u8 r,g,b;
                pixel.getRGB(r, g, b);
                fprintf(file, "%d %d %d ", r, g, b);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        return true;
    }

    bool savePPM(const char* filename, const Color4* rgba, s32 width, s32 height)
    {
        LASSERT(NULL != filename);
        LASSERT(NULL != rgba);

        FILE* file = NULL;
        fopen_s(&file, filename, "wb");
        if(NULL == file){
            return false;
        }

        fprintf(file, "P3\n");
        fprintf(file, "%d %d\n", width, height);
        fprintf(file, "255\n");

        for(s32 i=0; i<height; ++i){
            for(s32 j=0; j<width; ++j){
                const Color4& pixel = rgba[width*i + j];
                u8 r,g,b,a;
                pixel.getRGBA(r, g, b, a);
                fprintf(file, "%d %d %d ", r, g, b);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        return true;
    }
}
