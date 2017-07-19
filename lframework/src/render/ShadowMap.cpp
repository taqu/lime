/**
@file ShadowMap.cpp
@author t-sakai
@date 2016/12/03 create
*/
#include "render/ShadowMap.h"
#include "render/Camera.h"
#include "render/Light.h"
#include "render/Frustum.h"
#include <lmath/geometry/PrimitiveTest.h>
#include <lgraphics/TextureRef.h>

namespace lfw
{
    ShadowMap::ShadowMap()
        :cascadeLevels_(1)
        ,resolution_(1)
        ,invResolution_(1.0f)
        ,pcfBlurSize_(0.0f)
        ,znear_(0.0f)
        ,zfar_(1.0f)
        ,depthBias_(1.0e-3f)
        ,slopeScaledDepthBias_(1.0e-2f)
        ,fitType_(FitType_ToCascades)
        ,moveLightTexelSize_(1)
        ,fitNearFar_(FitNearFar_AABB)
        ,autoupdateAABB_(1)
    {
        sceneAABBMin_ = lmath::Vector4::zero();
        sceneAABBMax_ = lmath::Vector4::zero();

        u8* p = lcore::align16(bufferMatrices_);
        lightProjection_ = reinterpret_cast<lmath::Matrix44*>(p);
        lightViewProjection_ = reinterpret_cast<lmath::Matrix44*>(p + sizeof(lmath::Matrix44)*NumCascades);
    }

    ShadowMap::~ShadowMap()
    {
    }

    void ShadowMap::initialize(s32 cascadeLevels, s32 resolution, f32 znear, f32 zfar, u32 format, f32 logRatio)
    {
        cascadeLevels_ = cascadeLevels;
        resolution_ = lcore::maximum(1, resolution);
        invResolution_ = 1.0f/resolution_;
        znear_ = znear;
        zfar_ = zfar;
        calcCascadePartitions(logRatio);

        {
            lgfx::DataFormat dsvFormat = static_cast<lgfx::DataFormat>(format);
            lgfx::DataFormat texFormat;
            lgfx::DataFormat srvFormat;
            switch(dsvFormat)
            {
            case lgfx::Data_D32_Float:
                texFormat = lgfx::Data_R32_TypeLess;
                srvFormat = lgfx::Data_R32_Float;
                break;
            case lgfx::Data_D24_UNorm_S8_UInt:
                texFormat = lgfx::Data_R24G8_TypeLess;
                srvFormat = lgfx::Data_R24_UNorm_X8_TypeLess;
                break;
            case lgfx::Data_D16_UNorm:
                texFormat = lgfx::Data_R16_TypeLess;
                srvFormat = lgfx::Data_R16_UNorm;
                break;
            default:
                texFormat = lgfx::Data_R24G8_TypeLess;
                srvFormat = lgfx::Data_R24_UNorm_X8_TypeLess;
                break;
            }
            lgfx::Texture2DRef depthTexture = lgfx::Texture::create2D(
                resolution_,
                resolution_,
                1,
                cascadeLevels,
                texFormat,
                lgfx::Usage_Default,
                lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);

            lgfx::DSVDesc dsvDesc;
            lgfx::SRVDesc srvDesc;

            dsvDesc.format_ = dsvFormat;
            dsvDesc.dimension_ = lgfx::DSVDimension_Texture2DArray;
            dsvDesc.tex2DArray_.mipSlice_ = 0;
            dsvDesc.tex2DArray_.firstArraySlice_ = 0;
            dsvDesc.tex2DArray_.arraySize_ = cascadeLevels;

            srvDesc.format_ = srvFormat;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2DArray;
            srvDesc.tex2DArray_.mostDetailedMip_ = 0;
            srvDesc.tex2DArray_.mipLevels_ = 1;
            srvDesc.tex2DArray_.firstArraySlice_ = 0;
            srvDesc.tex2DArray_.arraySize_ = cascadeLevels;

            dsvShadowMap_ = depthTexture.createDSView(dsvDesc);
            srvShadowMap_ = depthTexture.createSRView(srvDesc);
        }
    }

    void ShadowMap::update(const Camera& camera, const Light& light)
    {
        //const lmath::Matrix44& view = camera.getViewMatrix();
        const lmath::Matrix44& invView = camera.getInvViewMatrix();
        lightDirection_ = -light.getDirection();

        lmath::Matrix44 lightView;
        light.getLightView(lightView);

        lmath::Vector4 sceneAABBPointsInLightView[8];
        createSceneAABBPoints(sceneAABBPointsInLightView);
        for(s32 i=0; i<8; ++i){
            sceneAABBPointsInLightView[i] = lmath::Vector4::construct(mul(lightView, sceneAABBPointsInLightView[i]));
        }

        f32 frustumIntervalBegin, frustumIntervalEnd;
        lmath::Vector4 lightViewOrthoMin;
        lmath::Vector4 lightViewOrthoMax;

        Frustum frustum;
        frustum.calcFromProjection(camera.getProjMatrix(), camera.getZNear(), camera.getZFar());

        lmath::Vector4 frustumPoints[8];
        f32 offsetForPCFBlur = static_cast<f32>(pcfBlurSize_*2+1)/resolution_;
        lmath::Vector4 scaleForPCFBlur = lmath::Vector4::construct(0.5f*offsetForPCFBlur, 0.5f*offsetForPCFBlur, 0.0f, 0.0f);
        f32 invResolution = 1.0f/resolution_;
        lmath::Vector4 vInvResolution = lmath::Vector4::construct(invResolution, invResolution, 0.0f, 0.0f);

        lmath::lm128 worldUnitsPerTexel;
        for(s32 cascade=0; cascade<cascadeLevels_; ++cascade){
            frustumIntervalBegin = (FitType_ToCascades == fitType_)? cascadePartitions_[cascade] : 0.0f;
            frustumIntervalEnd = cascadePartitions_[cascade+1];


            frustum.getPoints(frustumPoints, frustumIntervalBegin, frustumIntervalEnd);

            lightViewOrthoMin.set(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
            lightViewOrthoMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

            lmath::lm128 worldPoint;
            for(s32 i=0; i<8; ++i){
                //ワールド座標へ変換
                frustumPoints[i] = lmath::Vector4::construct(mul(invView, frustumPoints[i]));
                //ライトの座標へ変換
                worldPoint = mul(lightView, frustumPoints[i]);
                lightViewOrthoMin = lmath::Vector4::construct(minimum(lightViewOrthoMin, worldPoint));
                lightViewOrthoMax = lmath::Vector4::construct(maximum(lightViewOrthoMax, worldPoint));
            }

            switch(fitType_)
            {
            case FitType_ToCascades:
                {
                    lmath::lm128 boarderOffset;
                    boarderOffset = sub(lightViewOrthoMax, lightViewOrthoMin);
                    boarderOffset = boarderOffset * scaleForPCFBlur;
                    lightViewOrthoMin = lmath::Vector4::construct(lightViewOrthoMin - boarderOffset);
                    lightViewOrthoMax = lmath::Vector4::construct(lightViewOrthoMax + boarderOffset);

                    worldUnitsPerTexel = sub(lightViewOrthoMax, lightViewOrthoMin);
                    worldUnitsPerTexel = worldUnitsPerTexel * vInvResolution;
                }
                break;

            case FitType_ToScene:
            default:
                {
                    lmath::Vector4 diagonal = lmath::Vector4::construct(sub(frustumPoints[0], frustumPoints[6]));
                    f32 cascadeBound = diagonal.length();
                    lmath::lm128 tmpBoarderOffset;
                    tmpBoarderOffset = sub(lightViewOrthoMax, lightViewOrthoMin);
                    tmpBoarderOffset = diagonal - tmpBoarderOffset;
                    tmpBoarderOffset = 0.5f * tmpBoarderOffset;
                    lmath::Vector4 boarderOffset = lmath::Vector4::construct(tmpBoarderOffset);
                    boarderOffset.z_ = boarderOffset.w_ = 0.0f;

                    lightViewOrthoMin -= boarderOffset;
                    lightViewOrthoMax += boarderOffset;
                    f32 units = cascadeBound*invResolution;
                    worldUnitsPerTexel = _mm_set_ps(0.0f, 0.0f, units, units);
                }
                break;
            }

            if(moveLightTexelSize_ == 1){
                lmath::lm128 tmpLightViewOrthoMin = lightViewOrthoMin / worldUnitsPerTexel;
                tmpLightViewOrthoMin = floor(tmpLightViewOrthoMin);
                lightViewOrthoMin = lmath::Vector4::construct(tmpLightViewOrthoMin * worldUnitsPerTexel);

                lmath::lm128 tmpLightViewOrthoMax = lightViewOrthoMax / worldUnitsPerTexel;
                tmpLightViewOrthoMax = floor(tmpLightViewOrthoMax);
                lightViewOrthoMax = lmath::Vector4::construct(tmpLightViewOrthoMax * worldUnitsPerTexel);
            }

            f32 nearPlane = 0.0f;
            f32 farPlane = 10000.0f;

            switch(fitNearFar_)
            {
            case FitNearFar_None:
                break;

            case FitNearFar_AABB:
                {
                    nearPlane = FLT_MAX;
                    farPlane = -FLT_MAX;
                    for(s32 i=0; i<8; ++i){
                        nearPlane = lcore::minimum(sceneAABBPointsInLightView[i].z_, nearPlane);
                        farPlane = lcore::maximum(sceneAABBPointsInLightView[i].z_, farPlane);
                    }
                }
                break;

            case FitNearFar_SceneAABB:
                Frustum::calcFitNearFar(nearPlane, farPlane, lightViewOrthoMin, lightViewOrthoMax, camera);
                break;
            }

            lightProjection_[cascade].orthoOffsetCenterReverseZ(
                lightViewOrthoMin.x_,
                lightViewOrthoMax.x_,
                lightViewOrthoMax.y_,
                lightViewOrthoMin.y_,
                nearPlane,
                farPlane);

            lightViewProjection_[cascade].mul(lightProjection_[cascade], lightView);

            //cascadePartitionsFrustum_[cascade] = frustumIntervalEnd;
        }
    }

    void ShadowMap::calcCascadePartitions(f32 logRatio)
    {
        f32 zextent = zfar_ - znear_;
        //f32 invZFar = 1.0f/zfar_;

        f32 farOverNear = zfar_ / znear_;
        f32 invNumCascades = 1.0f/cascadeLevels_;

        cascadePartitions_[0] = 0.0f;

        for(s32 i=1; i<=cascadeLevels_; ++i){
            f32 step = static_cast<f32>(i) * invNumCascades;
            f32 logZ = znear_ * lmath::pow(farOverNear, step);
            f32 uniformZ = zextent * step;

            f32 zfar = lcore::lerp(uniformZ, logZ, logRatio);

            cascadePartitions_[i] = zfar;
            cascadeScales_[i-1] = (zfar-cascadePartitions_[i-1]);
        }
    }

    void ShadowMap::createSceneAABBPoints(lmath::Vector4 dst[8])
    {
        static const lmath::Vector4 map[] = 
        { 
            lmath::Vector4::construct(0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, 0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, -0.5f, -0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, 0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(0.5f, -0.5f, 0.5f, 0.5f),
            lmath::Vector4::construct(-0.5f, -0.5f, 0.5f, 0.5f),
        };

        lmath::return_type_vec4 sceneCenter = add(sceneAABBMin_, sceneAABBMax_);
        sceneCenter = 0.5f * sceneCenter;

        lmath::return_type_vec4 sceneExtent = sub(sceneAABBMax_, sceneAABBMin_);
        for(s32 i=0; i<8; ++i){
            dst[i] = lmath::Vector4::construct((lmath::lm128)muladd(map[i], sceneExtent, sceneCenter));
        }
    }

    bool ShadowMap::contains(const lmath::Sphere& sphere) const
    {
        return lmath::testSphereAABB(sphere, sceneAABBMin_, sceneAABBMax_);
    }
}
