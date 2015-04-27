/**
@file RenderContext.cpp
@author t-sakai
@date 2014/10/30 create
*/
#include "RenderContext.h"
#include <lmath/Matrix34.h>
#include <lmath/Matrix44.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/BlendStateRef.h>
#include <lgraphics/api/ShaderRef.h>

#include "anim/lanim.h"
#include "Scene.h"
#include "ConstantBuffer.h"
#include "ShaderConstant.h"

namespace lscene
{
    RenderContext::RenderContext()
        :path_(0)
        ,scene_(NULL)
        ,shadowMap_(NULL)
        ,context_(NULL)
        ,constantBuffer_(NULL)
        ,sceneConstantBufferVS_(NULL)
        ,sceneConstantBufferPS_(NULL)
        ,blendState_(lgraphics::ContextRef::BlendState_Num)
    {
        for(s32 i=0; i<Shader_Num; ++i){
            numPathConstants_[i] = 0;
        }
    }

    RenderContext::~RenderContext()
    {
    }

    void RenderContext::initialize(lgraphics::ContextRef* context, s32 numPaths)
    {
        renderQueue_.initialize(context, numPaths);
    }

    void RenderContext::terminate()
    {
        renderQueue_.terminate();
    }

    bool RenderContext::setSystemConstantBuffer(Shader shader, s32 index, lgraphics::ConstantBufferRef* constant)
    {
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != constant);

        switch(shader)
        {
        case Shader_VS:
            context_->setVSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_HS:
            context_->setHSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_DS:
            context_->setDSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_GS:
            context_->setGSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_PS:
            context_->setPSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_CS:
            context_->setCSConstantBuffers(index, 1, constant->get());
            break;

        default:
            return false;
        }
        return true;
    }

    bool RenderContext::setSystemConstant(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != data);

        lgraphics::ConstantBufferRef* constant = constantBuffer_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgraphics::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::copyAlign16DstOnlySize16Times(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setSystemConstantBuffer(shader, index, constant);
        } else{
            return false;
        }
    }

    bool RenderContext::setSystemConstantAligned16(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != data);

        lgraphics::ConstantBufferRef* constant = constantBuffer_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgraphics::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::copyAlign16Size16Times(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setSystemConstantBuffer(shader, index, constant);
        } else{
            return false;
        }
    }

    void RenderContext::setSceneConstantVS(const Scene& scene, const ShadowMap& shadowMap)
    {
        sceneConstantBufferVS_ = createConstantBuffer(sizeof(lscene::SceneConstantVS));

        lgraphics::MappedSubresource mapped;
        if(sceneConstantBufferVS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::SceneConstantVS* sceneConstantVS = reinterpret_cast<lscene::SceneConstantVS*>(mapped.data_);
            lscene::setSceneConstantVS(*sceneConstantVS, scene, shadowMap);
            sceneConstantBufferVS_->unmap(*context_, 0);
            setSystemConstantBuffer(lscene::RenderContext::Shader_VS, lscene::DefaultSceneConstantVSAttachIndex, sceneConstantBufferVS_);
        }
    }

    void RenderContext::setSceneConstantPS(const SceneConstantPS& src, const Scene& scene)
    {
        sceneConstantBufferPS_ = createConstantBuffer(sizeof(lscene::SceneConstantPS));

        lgraphics::MappedSubresource mapped;
        if(sceneConstantBufferPS_->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::SceneConstantPS* sceneConstantPS = reinterpret_cast<lscene::SceneConstantPS*>(mapped.data_);
            lscene::setSceneConstantPS(*sceneConstantPS, src, scene);
            sceneConstantBufferPS_->unmap(*context_, 0);
            setSystemConstantBuffer(lscene::RenderContext::Shader_PS, lscene::DefaultSceneConstantPSAttachIndex, sceneConstantBufferPS_);
        }
    }

    lgraphics::ConstantBufferRef* RenderContext::createConstantBuffer(u32 size)
    {
        return constantBuffer_->allocate(size);
    }


    inline bool RenderContext::setConstantBuffer(Shader shader, s32 userIndex, lgraphics::ConstantBufferRef* constant)
    {
        s32 index = numPathConstants_[shader] + userIndex;
        switch(shader)
        {
        case Shader_VS:
            context_->setVSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_HS:
            context_->setHSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_DS:
            context_->setDSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_GS:
            context_->setGSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_PS:
            context_->setPSConstantBuffers(index, 1, constant->get());
            break;

        case Shader_CS:
            context_->setCSConstantBuffers(index, 1, constant->get());
            break;

        default:
            return false;
        }
        return true;
    }

    bool RenderContext::setConstant(Shader shader, s32 userIndex, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != data);

        lgraphics::ConstantBufferRef* constant = constantBuffer_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgraphics::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::copyAlign16DstOnlySize16Times(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, userIndex, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstantAligned16(Shader shader, s32 userIndex, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != data);

        lgraphics::ConstantBufferRef* constant = constantBuffer_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgraphics::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
            lscene::copyAlign16Size16Times(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, userIndex, constant);
        }else{
            return false;
        }
    }

    bool RenderContext::setConstantMatricesAligned16(Shader shader, s32 userIndex, s32 num, const lmath::Matrix34* matrices)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBuffer_);
        LASSERT(NULL != matrices);

#ifdef _DEBUG
        u32 size = lanim::LANIM_MAX_SKINNING_MATRICES*sizeof(lmath::Matrix34);
#else
        u32 size = num*sizeof(lmath::Matrix34);
#endif
        lgraphics::ConstantBufferRef* constant = constantBuffer_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgraphics::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgraphics::MapType_WriteDiscard, mapped)){
#ifdef _DEBUG
            lscene::copyAlign16Size16Times(mapped.data_, matrices, num*sizeof(lmath::Matrix34));
            lmath::Matrix34* m = reinterpret_cast<lmath::Matrix34*>(mapped.data_);
            for(s32 i=num; i<lanim::LANIM_MAX_SKINNING_MATRICES; ++i){
                m[i].identity();
            }
#else
            lscene::copyAlign16Size16Times(mapped.data_, matrices, size);
#endif
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, userIndex, constant);
        }else{
            return false;
        }
    }

    void RenderContext::setDefaultNumPathConstants(s32 path)
    {
        switch(path)
        {
        case Path_Shadow:
            numPathConstants_[Shader_VS] = 1;
            numPathConstants_[Shader_HS] = 0;
            numPathConstants_[Shader_DS] = 0;
            numPathConstants_[Shader_GS] = 0;
            numPathConstants_[Shader_PS] = 0;
            numPathConstants_[Shader_CS] = 0;
            break;

        case Path_Opaque:
        case Path_Transparent:
            numPathConstants_[Shader_VS] = 1;
            numPathConstants_[Shader_HS] = 0;
            numPathConstants_[Shader_DS] = 0;
            numPathConstants_[Shader_GS] = 0;
            numPathConstants_[Shader_PS] = 1;
            numPathConstants_[Shader_CS] = 0;
            break;

        case Path_MotionVelocity:
            numPathConstants_[Shader_VS] = 1;
            numPathConstants_[Shader_HS] = 0;
            numPathConstants_[Shader_DS] = 0;
            numPathConstants_[Shader_GS] = 0;
            numPathConstants_[Shader_PS] = 0;
            numPathConstants_[Shader_CS] = 0;
            break;

        //case Path_Effect:
        //    numPathConstants_[Shader_VS] = 0;
        //    numPathConstants_[Shader_GS] = 0;
        //    numPathConstants_[Shader_PS] = 0;
        //    numPathConstants_[Shader_CS] = 0;
        //    break;

        //case Path_2D:
        //    numPathConstants_[Shader_VS] = 0;
        //    numPathConstants_[Shader_GS] = 0;
        //    numPathConstants_[Shader_PS] = 0;
        //    numPathConstants_[Shader_CS] = 0;
        //    break;

        default:
            numPathConstants_[Shader_VS] = 0;
            numPathConstants_[Shader_HS] = 0;
            numPathConstants_[Shader_DS] = 0;
            numPathConstants_[Shader_GS] = 0;
            numPathConstants_[Shader_PS] = 0;
            numPathConstants_[Shader_CS] = 0;
            break;
        }
    }

    void RenderContext::setBlendState(s32 blendState)
    {
        LASSERT(0<=blendState && blendState<lgraphics::ContextRef::BlendState_Num);
        if(blendState_ != blendState){
            blendState_ = blendState;
            context_->setBlendState((lgraphics::ContextRef::BlendState)blendState);
        }
    }

    void RenderContext::setBlendState(lgraphics::BlendStateRef& blendState)
    {
        context_->setBlendState(blendState.getBlendState());
        blendState_ = lgraphics::ContextRef::BlendState_Num;
    }

    void RenderContext::updateWorldFrustum()
    {
        worldFrustum_.calcInWorld(scene_->getCamera());
    }

    void RenderContext::attachDepthShader(DepthShader type)
    {
        depthShaderVS_[type]->attach(*context_);
    }
}
