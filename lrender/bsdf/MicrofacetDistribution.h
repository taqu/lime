#ifndef INC_LRENDER_MICROFACETDISTRIBUTION_H__
#define INC_LRENDER_MICROFACETDISTRIBUTION_H__
/**
@file MicrofacetDistribution.h
@author t-sakai
@date 2015/09/26 create
*/
#include "../lrender.h"
#include "../core/Coordinate.h"

namespace lrender
{
//#define LRENDER_MICROFACETDISTRIBUTION_SAMPLE_VISIBILITY (1)

    class MicrofacetDistribution
    {
    public:
        //inline MicrofacetDistribution(f32 alphaU, f32 alphaV);
        inline MicrofacetDistribution(f32 alpha);

        //inline f32 getAlphaU() const;
        //inline void setAlphaU(f32 alphaU);
        //inline f32 getAlphaV() const;
        //inline void setAlphaV(f32 alphaV);

        inline f32 getAlpha() const;
        inline void setAlpha(f32 alpha);

        inline f32 D(const Vector3& wh) const;
        inline f32 DNoCheck(const Vector3& wh) const;
        inline f32 G(const Vector3& wo, const Vector3& wi, const Vector3& wh) const;

        inline Vector3 sample_f(const Vector3& wo, f32 u0, f32 u1) const;
        inline f32 getPdf(const Vector3& wo, const Vector3& wi) const;

        inline f32 smithG1(const Vector3& v, const Vector3& wh) const;
    private:
        //inline f32 getRoughness() const;

        //inline f32 getProjectedRoughness2(const Vector3& v) const;
        inline static void sample11GGX(f32 theta_i, f32 u0, f32 u1, f32& slope_x, f32& slope_y);

        f32 alpha_;
        //f32 alphaU_;
        //f32 alphaV_;
    };

    //inline MicrofacetDistribution::MicrofacetDistribution(f32 alphaU, f32 alphaV)
    //    :alphaU_(alphaU)
    //    ,alphaV_(alphaV)
    //{
    //    alphaU_ = lcore::maximum(alphaU_, 1.0e-4f);
    //    alphaV_ = lcore::maximum(alphaV_, 1.0e-4f);
    //}

    //inline f32 MicrofacetDistribution::getAlphaU() const
    //{
    //    return alphaU_;
    //}

    //inline void MicrofacetDistribution::setAlphaU(f32 alphaU)
    //{
    //    alphaU_ = lcore::maximum(alphaU, 1.0e-4f);
    //}

    //inline f32 MicrofacetDistribution::getAlphaV() const
    //{
    //    return alphaV_;
    //}

    //inline void MicrofacetDistribution::setAlphaV(f32 alphaV)
    //{
    //    alphaV_ = lcore::maximum(alphaV, 1.0e-4f);
    //}

    inline MicrofacetDistribution::MicrofacetDistribution(f32 alpha)
        :alpha_(lcore::maximum(alpha, 1.0e-3f))
    {
    }

    inline f32 MicrofacetDistribution::getAlpha() const
    {
        return alpha_;
    }

    inline void MicrofacetDistribution::setAlpha(f32 alpha)
    {
        alpha_ = lcore::maximum(alpha, 1.0e-3f);
    }

    inline f32 MicrofacetDistribution::D(const Vector3& wh) const
    {
        f32 cosTheta = LocalCoordinate::cosTheta(wh);
        if(cosTheta<=0.0f){
            return 0.0f;
        }
        f32 ret = DNoCheck(wh);
        return (ret*cosTheta<1.0e-6f)? 0.0f : ret;
    }

    inline f32 MicrofacetDistribution::DNoCheck(const Vector3& wh) const
    {
        //GGX Distribution
        f32 cosTheta2 = LocalCoordinate::cosTheta2(wh);
#if 0
        f32 t = (wh.x_*wh.x_)/(alphaU_*alphaU_) + (wh.y_*wh.y_)/(alphaV_*alphaV_) + cosTheta2;
        f32 ret = 1.0f/(PI*alphaU_*alphaV_ * t*t);
#else
        f32 alpha2 = alpha_*alpha_;
        f32 denom = cosTheta2*(alpha2 - 1.0f) + 1.0f;
        f32 ret = alpha2 / (PI*denom*denom);
#endif
        return ret;
    }

    inline f32 MicrofacetDistribution::G(const Vector3& wo, const Vector3& wi, const Vector3& wh) const
    {
        return smithG1(wo, wh)*smithG1(wi, wh);
    }

#if LRENDER_MICROFACETDISTRIBUTION_SAMPLE_VISIBILITY
    /*
    Importance Sampling Microfacet-Based BSDFs with the Distribution of Visible Normals
    https://hal.inria.fr/hal-00996995v2/file/supplemental1.pdf
    */
    inline Vector3 MicrofacetDistribution::sample_f(const Vector3& wo, f32 u0, f32 u1) const
    {
        //1. stretch wo
        //Vector3 two(alphaU_*wo.x_, alphaV_*wo.y_, wo.z_);
        Vector3 two(alpha_*wo.x_, alpha_*wo.y_, wo.z_);
        two.normalize();
        //get polar coordinates of two
        f32 theta = 0.0f;
        f32 phi = 0.0f;
        if(two.z_<0.99999f){
            theta = lmath::acos(two.z_);
            phi = lmath::atan2(two.y_, two.x_);
        }

        //2. sample P22_{two}(slopex, slopey, 1, 1)
        f32 sx, sy;
        sample11GGX(theta, u0, u1, sx, sy);
        
        //3. rotate
        f32 cosPhi, sinPhi;
        lmath::sincos(cosPhi, sinPhi, phi);
        f32 slope_x = cosPhi * sx - sinPhi * sy;
        f32 slope_y = sinPhi * sx + cosPhi * sy;

        //4. unstretch
        //slope_x *= alphaU_;
        //slope_y *= alphaV_;
        slope_x *= alpha_;
        slope_y *= alpha_;

        //5. compute normal
        f32 invWm = 1.0f/lmath::sqrt(slope_x*slope_x + slope_y*slope_y + 1.0f);
        return Vector3(-slope_x*invWm, -slope_y*invWm, invWm); 
    }

#else
    inline Vector3 MicrofacetDistribution::sample_f(const Vector3& wo, f32 u0, f32 u1) const
    {
        f32 alpha2 = alpha_*alpha_;
        f32 tanThetaM2 = alpha2*u0/(1.0f-u0);
        f32 cosThetaM = 1.0f/lmath::sqrt(1.0f+tanThetaM2);
        f32 phiM = PI2 * u1;
        Vector3 m;
        m.z_ = cosThetaM;
        f32 r = lmath::sqrt(1.0f-m.z_*m.z_);

        f32 cosPhi, sinPhi;
        lmath::sincos(sinPhi, cosPhi, phiM);
        m.x_ = r*cosPhi;
        m.y_ = r*sinPhi;

        Vector3 wi;
        lmath::reflect(wi, wo, m);
        return wi;
    }
#endif

    inline f32 MicrofacetDistribution::getPdf(const Vector3& wo, const Vector3& wi) const
    {
        f32 cosThetaO = LocalCoordinate::cosTheta(wo);
        f32 cosThetaI = LocalCoordinate::cosTheta(wi);
        if(cosThetaO<=DOT_EPSILON || cosThetaI<=DOT_EPSILON){
            return 0.0f;
        }
        Vector3 wh = normalize(wo+wi);

        f32 costWiWh = lcore::absolute(dot(wi,wh));

#if LRENDER_MICROFACETDISTRIBUTION_SAMPLE_VISIBILITY
        return smithG1(wo, wh) * DNoCheck(wh) / (4.0f*lcore::absolute(cosThetaO));
#else
        return DNoCheck(wh) * LocalCoordinate::absCosTheta(wh) * 0.25f/costWiWh;
#endif
    }

    inline f32 MicrofacetDistribution::smithG1(const Vector3& v, const Vector3& wh) const
    {
        f32 cosTheta = LocalCoordinate::cosTheta(v);
        if(dot(v, wh) * cosTheta <= 0.0f){
            return 0.0f;
        }
        cosTheta = lcore::absolute(cosTheta);
        f32 k = alpha_*0.5f;
        return 1.0f/(cosTheta*(1.0f-k)+k);
    }

#if 1
    //inline f32 MicrofacetDistribution::getRoughness() const
    //{
    //    return (alphaU_*alphaU_ + alphaV_*alphaV_)*0.5f;
    //}

    //inline f32 MicrofacetDistribution::getProjectedRoughness2(const Vector3& v) const
    //{
    //    f32 invSinTheta2 = 1.0f/LocalCoordinate::sinTheta2(v);
    //    if(lmath::isEqual(alphaU_, alphaV_) || invSinTheta2<=0.0f){
    //        return alphaU_*alphaU_;
    //    }

    //    f32 cosPhi2 = v.x_*v.x_*invSinTheta2;
    //    f32 sinPhi2 = v.y_*v.y_*invSinTheta2;

    //    return (cosPhi2*alphaU_*alphaU_ + sinPhi2*alphaV_*alphaV_);
    //}

    inline void MicrofacetDistribution::sample11GGX(
        f32 theta_i,
        f32 u0, f32 u1,
        f32& slope_x, f32& slope_y)
    {
        //special case (normal incidence)
        if(theta_i < 0.0001f){
            f32 r = lmath::sqrt(u0 / (1.0f - u0));
            f32 phi = PI2 * u1;
            f32 sinPhi, cosPhi;
            lmath::sincos(sinPhi, cosPhi, phi);
            slope_x = r * cosPhi;
            slope_y = r * sinPhi;
            return;
        }

        //precomputations
        f32 tan_theta_i = lmath::tan(theta_i);
        f32 a = 1.0f / (tan_theta_i);
        f32 G1 = 2.0f / (1.0f + lmath::sqrt(1.0f + 1.0f / (a*a)));

        //sample slope_x
        f32 A = 2.0f*u0 / G1 - 1.0f;
        f32 A2 = A*A;
        f32 A22 = A2 - 1.0f;
        //if(lmath::isEqual(A22, 0.0f, 1.0e-5f)){
        //    A22 = (A22<0.0f)? -1.0e-5f : 1.0e-5f;
        //}
        f32 tmp = 1.0f / A22;
        f32 B = tan_theta_i;
        f32 D = lmath::sqrt(B*B*tmp*tmp - (A2 - B*B)*tmp);
        f32 slope_x_1 = B*tmp - D;
        f32 slope_x_2 = B*tmp + D;
        slope_x = (A < 0 || slope_x_2 > 1.0f / tan_theta_i) ? slope_x_1 : slope_x_2;

        //sample slope_y
        f32 S;
        if(u1 > 0.5f){
            S = 1.0f;
            u1 = 2.0f*(u1 - 0.5f);
        } else{
            S = -1.0f;
            u1 = 2.0f*(0.5f - u1);
        }
        f32 z = (u1*(u1*(u1*0.27385f - 0.73369f) + 0.46341f)) / (u1*(u1*(u1*0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
        slope_y = S * z * lmath::sqrt(1.0f + slope_x*slope_x);
    }
#endif
}
#endif //INC_LRENDER_MICROFACETDISTRIBUTION_H__
