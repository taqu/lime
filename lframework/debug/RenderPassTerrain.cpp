/**
@file RenderPassTerrain.cpp
@author t-sakai
@date 2017/06/10 create
*/
#include "RenderPassTerrain.h"
#include <lframework/System.h>
#include <lframework/Timer.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture3D.h>
#include <lframework/render/RenderContext.h>
#include <lframework/render/Camera.h>
#include <lframework/render/RenderQueue.h>
#include <lframework/ecs/ComponentRenderer.h>
#include <lframework/render/graph/RenderGraph.h>
#include <lframework/render/graph/RenderPassGBuffer.h>
#include <lframework/render/graph/RenderPassLighting.h>
#include "ShaderID.h"

namespace debug
{
    RenderPassTerrain::RenderPassTerrain()
        :type_(Type_Land)
    {
    }

    RenderPassTerrain::~RenderPassTerrain()
    {
    }

    void RenderPassTerrain::initialize()
    {
        samLinear_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);

        samLinearMirror_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Wrap,
            lgfx::Cmp_Always,
            0.0f);

        lfw::ShaderManager& shaderManager = lfw::System::getResources().getShaderManager();
        vsProcedual_ = shaderManager.getVS(ShaderVS_ProceduralSpace);
        psProcedual_[Type_Land] = shaderManager.getPS(ShaderPS_ProceduralTerrain00);

        lfw::s32 setID = 0;
        lfw::ResourceTexture3D* texNoise = lfw::System::getResources().load(setID, "LDR_RGBA_3D.dds", lfw::ResourceType::ResourceType_Texture3D, lfw::TextureParameter::NoSRGB_)->cast<lfw::ResourceTexture3D>();
        srvNoise_ = texNoise->getShaderResourceView();

        //xthreads_ = width_>>lfw::ComputeShader_NumThreads_Shift;
        //ythreads_ = height_>>lfw::ComputeShader_NumThreads_Shift;
        //lfw::s32 halfWidth = lfw::padSizeForComputeShader(width_>>1);
        //lfw::s32 halfHeight = lfw::padSizeForComputeShader(height_>>1);
        //halfXThreads_ = halfWidth>>lfw::ComputeShader_NumThreads_Shift;
        //halfYThreads_ = halfHeight>>lfw::ComputeShader_NumThreads_Shift;

        constant_.width_ = width_;
        constant_.height_ = height_;
        constant_.invWidth_ = 1.0f/constant_.width_;
        constant_.invHeight_ = 1.0f/constant_.height_;
    }

    void RenderPassTerrain::execute(lfw::RenderContext& renderContext, lfw::Camera& camera)
    {
        if(type_ == Type_None){
            return;
        }

        lgfx::ContextRef& context = renderContext.getContext();

        /*constant_.time_ += lfw::System::getTimer().getDeltaTime();

        if(600.0f<constant_.time_){
            constant_.time_ -= 600.0f;
        }*/


        lgfx::RenderTargetViewRef::pointer_type rtvs[] =
        {
            rtvAlbedo_,
            rtvSpecular_,
            rtvDepthNormal_,
        };
        context.setRenderTargets(3, rtvs, dsvDepthStencil_);
        context.setViewport(0, 0, width_, height_);

        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DDisableWEnable);

        context.setVertexShader(vsProcedual_);
        context.setPixelShader(psProcedual_[type_]);

        lgfx::ShaderSamplerState samplerStates(context, samLinearMirror_, samLinear_);
        samplerStates.setPS(0);

        context.setPSResources(0, 1, srvNoise_);

        renderContext.setConstant(lfw::RenderContext::Shader_PS, 6, sizeof(ProcedualConstant), &constant_);
        context.draw(4, 0);

        context.setVertexShader(NULL);
        context.setPixelShader(NULL);

        context.clearPSResources(0, 1);
        context.clearPSConstantBuffers(6, 1);
        context.clearRenderTargets(3);
        context.clearPSSamplers(0, 2);
    }

    void RenderPassTerrain::set(Type type)
    {
        type_ = type;
    }
}
