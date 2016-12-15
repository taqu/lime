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

#include "render/ShaderConstant.h"
#include "animation/lanim.h"
#include "render/ShadowMap.h"
#include "render/Camera.h"

namespace lfw
{
    RenderContext::RenderContext()
        :renderPath_(0)
        ,camera_(NULL)
        ,shadowMap_(NULL)
        ,context_(NULL)
        ,constantBufferTableSet_(NULL)
        ,depthShaderGS_(NULL)
        //,sceneConstantBufferVS_(NULL)
        //,sceneConstantBufferDS_(NULL)
        //,sceneConstantBufferPS_(NULL)
        //,lightClusterConstantBufferPS_(NULL)
    {
    }

    RenderContext::~RenderContext()
    {
    }

    void RenderContext::initialize(lgfx::ContextRef* context)
    {
        context_ = context;
    }

    void RenderContext::terminate()
    {
    }

    void RenderContext::setPerFrameConstants()
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerFrameConstantVS));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerFrameConstantVS* constants = reinterpret_cast<PerFrameConstantVS*>(mapped.data_);
            constants->velocityScale_ = 1.0f;
            constants->velocityMaxMagnitude_ = 100.0f;
            constant->unmap(*context_, 0);
            setConstantBuffer(Shader_VS, 0, constant);
        }
    }

    void RenderContext::setPerCameraConstants(Camera& camera)
    {
        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerCameraConstantVS));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            PerCameraConstantVS* constants = reinterpret_cast<PerCameraConstantVS*>(mapped.data_);
            copyAlignedDst16(constants->view_, camera.getViewMatrix());
            copyAlignedDst16(constants->invview_, camera.getInvViewMatrix());
            copyAlignedDst16(constants->projection_, camera.getProjMatrix());
            copyAlignedDst16(constants->invprojection_, camera.getInvProjMatrix());
            copyAlignedDst16(constants->vp0_, camera.getPrevViewProjMatrix());
            copyAlignedDst16(constants->vp1_, camera.getViewProjMatrix());
            copyAlignedDst16(constants->cameraPos_, camera.getEyePosition());
            camera.getRTSize(constants->screenWidth_, constants->screenHeight_);
            constants->screenInvWidth_ = 1.0f/constants->screenWidth_;
            constants->screenInvHeight_ = 1.0f/constants->screenHeight_;
            constant->unmap(*context_, 0);
            setConstantBuffer(Shader_VS, 1, constant);
            setConstantBuffer(Shader_PS, 2, constant);
        }
    }

    void RenderContext::setPerShadowMapConstants()
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(sizeof(PerShadowMapConstantVS));
        if(NULL == constant){
            return;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            shadowMap_->getLightViewProjectionAlign16(reinterpret_cast<lmath::Matrix44*>(mapped.data_));
            constant->unmap(*context_, 0);
            setConstantBuffer(Shader_VS, 4, constant);
        }
    }

    void RenderContext::setRenderPath(s32 path)
    {
        renderPath_ = path;
        switch(renderPath_)
        {
        case RenderPath_Shadow:
            context_->setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnable);
            context_->setGeometryShader(depthShaderGS_->get());
            context_->setPixelShader(NULL);
            break;
        case RenderPath_Opaque:
            context_->setGeometryShader(NULL);
            break;
        default:
            context_->setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWDisable);
            context_->setGeometryShader(NULL);
            break;
        }
    }

    //void RenderContext::setSceneConstantVS(const Scene& scene, const ShadowMap& shadowMap)
    //{
    //    sceneConstantBufferVS_ = createConstantBuffer(sizeof(lscene::SceneConstantVS));

    //    lgraphics::MappedSubresource mapped;
    //    if(sceneConstantBufferVS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
    //        lscene::SceneConstantVS* sceneConstantVS = reinterpret_cast<lscene::SceneConstantVS*>(mapped.data_);
    //        lscene::setSceneConstantVS(*sceneConstantVS, scene, shadowMap);
    //        sceneConstantBufferVS_->unmap(*context_, 0);
    //        setSystemConstantBuffer(lscene::RenderContext::Shader_VS, lscene::DefaultSceneConstantVSAttachIndex, sceneConstantBufferVS_);
    //    }
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

    bool RenderContext::setConstantMatricesAligned16(Shader shader, s32 userIndex, s32 num, const lmath::Matrix34* matrices)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != matrices);

#ifdef _DEBUG
        u32 size = LANIM_MAX_SKINNING_MATRICES*sizeof(lmath::Matrix34);
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
            for(s32 i=num; i<LANIM_MAX_SKINNING_MATRICES; ++i){
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

    void RenderContext::updateWorldFrustum(const Camera& camera)
    {
        worldFrustum_.calcInWorld(camera);
    }

    void RenderContext::attachDepthShader(DepthShader type)
    {
        depthShaderVS_[type]->attach(*context_);
    }
}
