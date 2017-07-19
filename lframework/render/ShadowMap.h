#ifndef INC_LFRAMEWORK_SHADOWMAP_H__
#define INC_LFRAMEWORK_SHADOWMAP_H__
/**
@file ShadowMap.h
@author t-sakai
@date 2016/12/03 create
*/
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>
#include <lgraphics/ViewRef.h>
#include "../lframework.h"

namespace lmath
{
    class Sphere;
}

namespace lfw
{
    class Camera;
    class Light;

    class ShadowMap
    {
    public:
        static const s32 NumCascades = LFW_CONFIG_SHADOW_NUMCASCADES;
        static const u8 FitType_ToCascades = 0;
        static const u8 FitType_ToScene = 1;

        static const u8 FitNearFar_None = 0;
        static const u8 FitNearFar_AABB = 1;
        static const u8 FitNearFar_SceneAABB = 2;

        ShadowMap();
        ~ShadowMap();

        void initialize(s32 cascadeLevels, s32 resolution, f32 znear, f32 zfar, u32 format, f32 logRatio=0.5f);
        void update(const Camera& camera, const Light& light);

        inline s32 getCascadeLevels() const;

        inline s32 getResolution() const;
        inline f32 getInvResolution() const;

        inline f32 getPCFBlurSize() const;
        inline void setPCFBlurSize(f32 size);

        inline f32 getDepthBias() const;
        inline void setDepthBias(f32 depthBias);

        inline f32 getSlopeScaledDepthBias() const;
        inline void setSlopeScaledDepthBias(f32 slopeScaledDepthBias);

        inline u8 getFitType() const;
        inline void setFitType(u8 type);

        inline u8 getMoveLightTexelSize() const;
        inline void setMoveLightTexelSize(u8 flag);

        inline s8 getFitNearFar();
        inline void setFitNearFar(u8 type);

        inline bool isAutoupdateAABB() const;
        inline void setAutoupdateAABB(bool autoupdate);

        inline void setSceneAABB(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax);
        inline lmath::Vector4& getSceneAABBMin();
        inline lmath::Vector4& getSceneAABBMax();

        inline const lmath::Matrix44& getLightProjection(s32 cascade) const;

        inline const lmath::Matrix44& getLightViewProjection(s32 cascade) const;

        inline f32 getCascadePartition(s32 cascade) const;

        void calcCascadePartitions(f32 logRatio);

        inline void getLightProjectionAlign16(lmath::Matrix44* dstAligned) const;
        inline void getLightViewProjectionAlign16(lmath::Matrix44* dstAligned) const;
        inline void getCascadeScalesAlign16(f32* cascades) const;
        inline const lmath::Vector4& getLightDirection() const;

        bool contains(const lmath::Sphere& sphere) const;

        inline lgfx::DepthStencilViewRef& getDSV();
        inline lgfx::ShaderResourceViewRef& getSRV();
    private:
        lmath::Vector4 sceneAABBMin_;
        lmath::Vector4 sceneAABBMax_;

        lmath::Matrix44* lightProjection_;
        lmath::Matrix44* lightViewProjection_;
        u8 bufferMatrices_[sizeof(lmath::Matrix44)*NumCascades*2 + 16];

        s32 cascadeLevels_;
        s32 resolution_;
        f32 invResolution_;
        f32 pcfBlurSize_;
        f32 znear_;
        f32 zfar_;
        f32 depthBias_;
        f32 slopeScaledDepthBias_;
        u8 fitType_;
        u8 moveLightTexelSize_;
        u8 fitNearFar_;
        u8 autoupdateAABB_;
        lmath::Vector4 lightDirection_;

        void createSceneAABBPoints(lmath::Vector4 dst[8]);
        //void getNearFar(f32& nearPlane, f32& farPlane, lmath::Vector4& lightViewOrthoMin, lmath::Vector4& lightViewOrthoMax, lmath::Vector4* viewPoints);

        f32 cascadePartitions_[NumCascades+1];
        f32 cascadeScales_[LFW_CONFIG_SHADOW_MAXCASCADES];

        lgfx::DepthStencilViewRef dsvShadowMap_;
        lgfx::ShaderResourceViewRef srvShadowMap_;
    };

    inline s32 ShadowMap::getCascadeLevels() const
    {
        return cascadeLevels_;
    }

    inline s32 ShadowMap::getResolution() const
    {
        return resolution_;
    }

    inline f32 ShadowMap::getInvResolution() const
    {
        return invResolution_;
    }

    inline f32 ShadowMap::getPCFBlurSize() const
    {
        return pcfBlurSize_;
    }

    inline void ShadowMap::setPCFBlurSize(f32 size)
    {
        LASSERT(0.0f<=size);
        pcfBlurSize_ = size;
    }

    inline f32 ShadowMap::getDepthBias() const
    {
        return depthBias_;
    }

    inline void ShadowMap::setDepthBias(f32 depthBias)
    {
        depthBias_ = depthBias;
    }

    inline f32 ShadowMap::getSlopeScaledDepthBias() const
    {
        return slopeScaledDepthBias_;
    }

    inline void ShadowMap::setSlopeScaledDepthBias(f32 slopeScaledDepthBias)
    {
        slopeScaledDepthBias_ = slopeScaledDepthBias;
    }

    inline u8 ShadowMap::getFitType() const
    {
        return fitType_;
    }

    inline void ShadowMap::setFitType(u8 type)
    {
        fitType_ = type;
    }

    inline u8 ShadowMap::getMoveLightTexelSize() const
    {
        return moveLightTexelSize_;
    }

    inline void ShadowMap::setMoveLightTexelSize(u8 flag)
    {
        moveLightTexelSize_ = flag;
    }

    inline s8 ShadowMap::getFitNearFar()
    {
        return fitNearFar_;
    }

    inline void ShadowMap::setFitNearFar(u8 type)
    {
        fitNearFar_ = type;
    }

    inline bool ShadowMap::isAutoupdateAABB() const
    {
        return 0 != autoupdateAABB_;
    }

    inline void ShadowMap::setAutoupdateAABB(bool autoupdate)
    {
        autoupdateAABB_ = (autoupdate)? 1 : 0;
    }

    inline void ShadowMap::setSceneAABB(const lmath::Vector4& aabbMin, const lmath::Vector4& aabbMax)
    {
        sceneAABBMin_ = aabbMin;
        sceneAABBMax_ = aabbMax;
    }

    inline lmath::Vector4& ShadowMap::getSceneAABBMin()
    {
        return sceneAABBMin_;
    }

    inline lmath::Vector4& ShadowMap::getSceneAABBMax()
    {
        return sceneAABBMax_;
    }

    inline const lmath::Matrix44& ShadowMap::getLightProjection(s32 cascade) const
    {
        LASSERT(0<=cascade && cascade<cascadeLevels_);
        return lightProjection_[cascade];
    }

    inline const lmath::Matrix44& ShadowMap::getLightViewProjection(s32 cascade) const
    {
        LASSERT(0<=cascade && cascade<cascadeLevels_);
        return lightViewProjection_[cascade];
    }

    inline f32 ShadowMap::getCascadePartition(s32 cascade) const
    {
        LASSERT(0<=cascade && cascade<cascadeLevels_);
        //return cascadePartitionsFrustum_[cascade];
        return cascadePartitions_[cascade+1];
    }

    inline void ShadowMap::getLightProjectionAlign16(lmath::Matrix44* dstAligned) const
    {
        copyAlignedDstAlignedSrc16(dstAligned, lightProjection_, sizeof(lmath::Matrix44)*NumCascades);
    }

    inline void ShadowMap::getLightViewProjectionAlign16(lmath::Matrix44* dstAligned) const
    {
        copyAlignedDstAlignedSrc16(dstAligned, lightViewProjection_, sizeof(lmath::Matrix44)*NumCascades);
    }

    inline void ShadowMap::getCascadeScalesAlign16(f32* cascadeScales) const
    {
        _mm_store_ps(cascadeScales, _mm_loadu_ps(cascadeScales_));
    }

    inline const lmath::Vector4& ShadowMap::getLightDirection() const
    {
        return lightDirection_;
    }

    inline lgfx::DepthStencilViewRef& ShadowMap::getDSV()
    {
        return dsvShadowMap_;
    }

    inline lgfx::ShaderResourceViewRef& ShadowMap::getSRV()
    {
        return srvShadowMap_;
    }
}

#endif //INC_LFRAMEWORK_SHADOWMAP_H__
