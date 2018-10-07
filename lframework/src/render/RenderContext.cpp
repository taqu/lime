/**
@file RenderContext.cpp
@author t-sakai
@date 2016/11/15 create
*/
#include "render/RenderContext.h"
#include <lmath/Matrix34.h>
#include <lmath/Matrix44.h>
#include <lgraphics/GraphicsDeviceRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/ConstantBufferTableSet.h>

#include "System.h"
#include "resource/Resources.h"
#include "animation/lanim.h"
#include "render/ShadowMap.h"
#include "render/Camera.h"
#include "render/Light.h"
#include "render/Geometry.h"
#include "resource/PrimitiveGeometry.h"

namespace lfw
{
    RenderContext::RenderContext()
        :renderPath_(0)
        ,geomLightSphere_(NULL)
        ,context_(NULL)
        ,constantBufferTableSet_(NULL)
        ,depthShaderGS_(NULL)
        ,renderQueue_(NULL)
    {
    }

    RenderContext::~RenderContext()
    {
        LDELETE(geomLightSphere_);
    }

    void RenderContext::initialize(lgfx::ContextRef* context)
    {
        context_ = context;

        geomLightSphere_ = LNEW Geometry;
        PrimitiveGeometry::createLightSphere(*geomLightSphere_, 0);
        samplerSet_.create(
            0,
            lgfx::TexFilter_MinMagMipPoint,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);
        samplerSet_.create(
            1,
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);
        samplerSet_.create(
            2,
            lgfx::TexFilter_CompMinMagMipLinear,
            lgfx::TexAddress_Border,
            lgfx::TexAddress_Border,
            lgfx::TexAddress_Border,
            lgfx::Cmp_Greater,
            0.0f);

        lfw::ShaderManager& shaderManager = lfw::System::getResources().getShaderManager();
        depthShaderVS_[DepthShader_Normal] = &shaderManager.getVS(lfw::ShaderVS_LightDepth);
        depthShaderVS_[DepthShader_Skinning] = &shaderManager.getVS(lfw::ShaderVS_LightDepthSkinning);
        depthShaderGS_ = &shaderManager.getGS(lfw::ShaderGS_LightDepth);
    }

    void RenderContext::terminate()
    {
    }

    void RenderContext::setPerFrameConstants()
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerFrameConstant));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerFrameConstant* constants = reinterpret_cast<PerFrameConstant*>(mapped.data_);
            constants->velocityScale_ = 1.0f;
            constants->velocityMaxMagnitude_ = 100.0f;
            constant->unmap(*context_, 0);
            context_->setVSConstantBuffers(0, 1, *constant);
            context_->setPSConstantBuffers(0, 1, *constant);
            context_->setCSConstantBuffers(0, 1, *constant);
        }
    }

    void RenderContext::setPerCameraConstants(Camera& camera)
    {
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerCameraConstant));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerCameraConstant* constants = reinterpret_cast<PerCameraConstant*>(mapped.data_);
            copyAlignedDst16(constants->view_, camera.getViewMatrix());
            copyAlignedDst16(constants->invview_, camera.getInvViewMatrix());
            copyAlignedDst16(constants->projection_, camera.getProjMatrix());
            copyAlignedDst16(constants->invprojection_, camera.getInvProjMatrix());
            copyAlignedDst16(constants->vp0_, camera.getPrevViewProjMatrix());
            copyAlignedDst16(constants->vp1_, camera.getViewProjMatrix());
            camera.getViewProjMatrix().getInvert(constants->invvp1_);

            copyAlignedDst16(constants->cameraPos_, camera.getEyePosition());
            const lgfx::Viewport& viewport = camera.getViewport();
            constants->screenWidth_ = viewport.width_;
            constants->screenHeight_ = viewport.height_;
            constants->screenInvWidth_ = 1.0f/constants->screenWidth_;
            constants->screenInvHeight_ = 1.0f/constants->screenHeight_;
            constants->linearZParam_ = camera.getLinearZParameter();
            constant->unmap(*context_, 0);

            context_->setVSConstantBuffers(2, 1, *constant);
            context_->setHSConstantBuffers(2, 1, *constant);
            context_->setDSConstantBuffers(2, 1, *constant);
            context_->setGSConstantBuffers(2, 1, *constant);
            context_->setPSConstantBuffers(2, 1, *constant);
            context_->setCSConstantBuffers(2, 1, *constant);
        }
    }

    void RenderContext::setPerLightConstants(Light& light)
    {
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerLightConstant));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerLightConstant* constants = reinterpret_cast<PerLightConstant*>(mapped.data_);
            _mm_store_ps(&constants->dlDir_.x_, _mm_loadu_ps(&light.getDirection().x_));
            _mm_store_ps(&constants->dlColor_.x_, _mm_loadu_ps(&light.getColor().x_)); 
            constant->unmap(*context_, 0);

            context_->setPSConstantBuffers(1, 1, *constant);
        }
    }

    void RenderContext::setPerShadowMapConstants()
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerShadowMapConstant));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerShadowMapConstant* shadowMapConstants = reinterpret_cast<PerShadowMapConstant*>(mapped.data_);
            shadowMap_.getLightViewProjectionAlign16(shadowMapConstants->lvp_);
            shadowMap_.getCascadeScalesAlign16(shadowMapConstants->cascadeScales_);
            _mm_store_ps(&shadowMapConstants->shadowMapLightDir_.x_, _mm_loadu_ps(&shadowMap_.getLightDirection().x_));

            shadowMapConstants->shadowMapSize_ = shadowMap_.getResolution();
            shadowMapConstants->invShadowMapSize_ = shadowMap_.getInvResolution();
            shadowMapConstants->shadowMapDepthBias_ = shadowMap_.getDepthBias();
            shadowMapConstants->shadowMapSlopeScaledDepthBias_ = shadowMap_.getSlopeScaledDepthBias();

            constant->unmap(*context_, 0);
            context_->setVSConstantBuffers(3, 1, *constant);
            context_->setPSConstantBuffers(3, 1, *constant);
        }
    }

    void RenderContext::beginRenderPath(s32 path)
    {
        renderPath_ = path;
        switch(renderPath_)
        {
        case RenderPath_Shadow:
            context_->setRenderTargets(0, NULL, shadowMap_.getDSV());
            context_->setViewport(0, 0, shadowMap_.getResolution(), shadowMap_.getResolution());
            context_->clearDepthStencilView(shadowMap_.getDSV(), lgfx::ClearFlag_Depth, 0.0f, 0);

            context_->setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnableReverseZ);
            context_->setRasterizerState(lgfx::ContextRef::Rasterizer_DepthMap);
            context_->setGeometryShader(depthShaderGS_->get());
            context_->setPixelShader(NULL);
            break;
        case RenderPath_Opaque:
            setDefaultSampler(Shader_PS);
            break;
        default:
            context_->setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWDisableReverseZ);
            setDefaultSampler(Shader_PS);
            break;
        }
    }

    void RenderContext::endRenderPath(s32 /*path*/)
    {
        switch(renderPath_)
        {
        case RenderPath_Shadow:
            context_->restoreDefaultRenderTargets();
            context_->restoreDefaultViewport();
            context_->setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);
            context_->setVertexShader(NULL);
            context_->setGeometryShader(NULL);
            break;
        case RenderPath_Opaque:
            break;
        default:
            break;
        }
    }

    //void RenderContext::beginDeferredLighting(const Light& light)
    //{
    //    LASSERT(NULL != context_);
    //    LASSERT(NULL != constantBufferTableSet_);

    //    lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerLightConstant));
    //    if(NULL == constant){
    //        return;
    //    }

    //    PerLightConstant perLightConstant;
    //    perLightConstant.dlDir_ = -light.getDirection();
    //    perLightConstant.dlColor_ = light.getColor();

    //    lgfx::MappedSubresource mapped;
    //    if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
    //        copyAlignedDstAlignedSrc16(mapped.data_, &perLightConstant, sizeof(PerLightConstant));
    //        constant->unmap(*context_, 0);
    //        context_->setPSConstantBuffers(1, 1, (*constant));
    //    }
    //}

    //void RenderContext::endDeferredLighting(const Light& /*light*/)
    //{
    //}

    //void RenderContext::setSceneConstantDS(const Scene& scene)
    //{
    //    sceneConstantBufferDS_ = createConstantBuffer(sizeof(lscene::SceneConstantDS));

    //    lgraphics::MappedSubresource mapped;
    //    if(sceneConstantBufferDS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
    //        lscene::SceneConstantDS* sceneConstantDS = reinterpret_cast<lscene::SceneConstantDS*>(mapped.data_);
    //        lscene::setSceneConstantDS(*sceneConstantDS, scene);
    //        sceneConstantBufferDS_->unmap(*context_, 0);
    //        setSystemConstantBuffer(lscene::RenderContext::Shader_DS, lscene::DefaultSceneConstantDSAttachIndex, sceneConstantBufferDS_);
    //    }
    //}

    //void RenderContext::setSceneConstantPS(const SceneConstantPS& src, const Scene& scene, const LightClusterConstantPS& lightCluster)
    //{
    //    sceneConstantBufferPS_ = createConstantBuffer(sizeof(lscene::SceneConstantPS));

    //    lgraphics::MappedSubresource mapped;
    //    if(sceneConstantBufferPS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
    //        lscene::SceneConstantPS* sceneConstantPS = reinterpret_cast<lscene::SceneConstantPS*>(mapped.data_);
    //        lscene::setSceneConstantPS(*sceneConstantPS, src, scene);
    //        sceneConstantBufferPS_->unmap(*context_, 0);
    //        setSystemConstantBuffer(lscene::RenderContext::Shader_PS, lscene::DefaultSceneConstantPSAttachIndex, sceneConstantBufferPS_);
    //    }

    //    lightClusterConstantBufferPS_ = createConstantBuffer(sizeof(lscene::LightClusterConstantPS));

    //    if(lightClusterConstantBufferPS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
    //        lscene::copyAlign16Size16Times(mapped.data_, &lightCluster, sizeof(LightClusterConstantPS));
    //        lightClusterConstantBufferPS_->unmap(*context_, 0);
    //        setSystemConstantBuffer(lscene::RenderContext::Shader_PS, lscene::DefaultLightClusterConstantPSAttachIndex, lightClusterConstantBufferPS_);
    //    }
    //}

    lgfx::ConstantBufferRef* RenderContext::createConstantBuffer(u32 size)
    {
        return constantBufferTableSet_->allocate(size);
    }

    lgfx::ConstantBufferRef* RenderContext::createConstantBuffer(u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return NULL;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return constant;
        }else{
            return NULL;
        }
    }

    inline bool RenderContext::setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant)
    {
        switch(shader)
        {
        case Shader_VS:
            context_->setVSConstantBuffers(index, 1, (*constant));
            break;

        case Shader_HS:
            context_->setHSConstantBuffers(index, 1, (*constant));
            break;

        case Shader_DS:
            context_->setDSConstantBuffers(index, 1, (*constant));
            break;

        case Shader_GS:
            context_->setGSConstantBuffers(index, 1, (*constant));
            break;

        case Shader_PS:
            context_->setPSConstantBuffers(index, 1, (*constant));
            break;

        case Shader_CS:
            context_->setCSConstantBuffers(index, 1, (*constant));
            break;

        default:
            return false;
        }
        return true;
    }

    bool RenderContext::setConstant(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstant(Shader shader, s32 index, u32 size, s32 num, s32 maxNum, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);
        LASSERT(0<=num && num<=maxNum);

#ifdef _DEBUG
        size *= maxNum;
#else
        size *= num;
#endif
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstantAligned16(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDstAlignedSrc16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstantAligned16(Shader shader, s32 index, u32 size, s32 num, s32 maxNum, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);
        LASSERT(0<=num && num<=maxNum);
#ifdef _DEBUG
        size *= maxNum;
#else
        size *= num;
#endif
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDstAlignedSrc16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstantMatricesAligned16(Shader shader, s32 userIndex, s32 num, s32 maxNum, const lmath::Matrix34* matrices)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(0<=num && num<=maxNum);
        LASSERT(NULL != matrices);

//#ifdef _DEBUG
//        u32 size = LANIM_MAX_SKINNING_MATRICES*sizeof(lmath::Matrix34);
//#else
//        u32 size = num*sizeof(lmath::Matrix34);
//#endif
#ifdef _DEBUG
        u32 size = maxNum*sizeof(lmath::Matrix34);
#else
        u32 size = num*sizeof(lmath::Matrix34);
#endif
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
#ifdef _DEBUG
            copyAlignedDstAlignedSrc16(mapped.data_, matrices, num*sizeof(lmath::Matrix34));
            lmath::Matrix34* m = reinterpret_cast<lmath::Matrix34*>(mapped.data_);
            for(s32 i=num; i<maxNum; ++i){
                m[i].identity();
            }
#else
            copyAlignedDstAlignedSrc16(mapped.data_, matrices, size);
#endif
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, userIndex, constant);
        }else{
            return false;
        }
    }

    void RenderContext::attachDepthShader(DepthShader type)
    {
        depthShaderVS_[type]->attach(*context_);
    }

    void RenderContext::setDefaultSampler(Shader shader)
    {
        switch(shader)
        {
        case Shader_VS:
            context_->setVSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        case Shader_HS:
            context_->setHSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        case Shader_DS:
            context_->setDSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        case Shader_GS:
            context_->setGSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        case Shader_PS:
            context_->setPSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        case Shader_CS:
            context_->setCSSamplers(0, samplerSet_.getNumSamplers(), samplerSet_.getSamplers());
            break;
        }
    }

    void RenderContext::clearDefaultSampler(Shader shader)
    {
        switch(shader)
        {
        case Shader_VS:
            context_->clearVSSamplers(0, samplerSet_.getNumSamplers());
            break;
        case Shader_HS:
            context_->clearHSSamplers(0, samplerSet_.getNumSamplers());
            break;
        case Shader_DS:
            context_->clearDSSamplers(0, samplerSet_.getNumSamplers());
            break;
        case Shader_GS:
            context_->clearGSSamplers(0, samplerSet_.getNumSamplers());
            break;
        case Shader_PS:
            context_->clearPSSamplers(0, samplerSet_.getNumSamplers());
            break;
        case Shader_CS:
            context_->clearCSSamplers(0, samplerSet_.getNumSamplers());
            break;
        }
    }
}
