#ifndef INC_LFW_RENDERPASSGBUFFER_H__
#define INC_LFW_RENDERPASSGBUFFER_H__
/**
@file RenderPassGBuffer.h
@author t-sakai
@date 2017/06/10 create
*/
#include "RenderPass.h"
#include <lgraphics/TextureRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/DepthStencilStateRef.h>

namespace lfw
{
namespace graph
{
    class RenderPassGBuffer : public RenderPass
    {
    public:
        static const u32 ID_Albedo;
        static const u32 ID_Specular;
        static const u32 ID_DepthNormal;
        static const u32 ID_Velocity;
        static const u32 ID_DSVDepthStencil;
        static const u32 ID_Stencil;
        static const u32 ID_FullVelocity;
        static const u32 ID_LinearDepth;

        static const u32 ID_UAVAlbedo;
        static const u32 ID_UAVSpecular;
        static const u32 ID_UAVDepthNormal;
        static const u32 ID_UAVVelocity;
        static const u32 ID_UAVFullVelocity;

        RenderPassGBuffer();
        virtual ~RenderPassGBuffer();

        virtual void create(const Camera& camera);
        virtual void destroy();
        virtual void execute(RenderContext& renderContext, Camera& camera);

    protected:
        RenderPassGBuffer(const RenderPassGBuffer&) = delete;
        RenderPassGBuffer& operator=(const RenderPassGBuffer&) = delete;

        s32 width_;
        s32 height_;
        s32 xthreads_;
        s32 ythreads_;
        s32 halfXThreads_;
        s32 halfYThreads_;

        lgfx::DepthStencilStateRef depthStencilState_;

        lgfx::RenderTargetViewRef rtvAlbedo_;
        lgfx::UnorderedAccessViewRef uavAlbedo_;
        lgfx::ShaderResourceViewRef srvAlbedo_;

        lgfx::RenderTargetViewRef rtvSpecular_;
        lgfx::UnorderedAccessViewRef uavSpecular_;
        lgfx::ShaderResourceViewRef srvSpecular_;

        lgfx::RenderTargetViewRef rtvDepthNormal_;
        lgfx::UnorderedAccessViewRef uavDepthNormal_;
        lgfx::ShaderResourceViewRef srvDepthNormal_;

        lgfx::RenderTargetViewRef rtvVelocity_;
        lgfx::UnorderedAccessViewRef uavVelocity_;
        lgfx::ShaderResourceViewRef srvVelocity_;

        lgfx::DepthStencilViewRef dsvDepthStencil_;
        lgfx::ShaderResourceViewRef srvStencil_;

        lgfx::UnorderedAccessViewRef uavFullVelocity_;
        lgfx::ShaderResourceViewRef srvFullVelocity_;

        lgfx::UnorderedAccessViewRef uavLinearDepth_;
        lgfx::ShaderResourceViewRef srvLinearDepth_;

        lgfx::ComputeShaderRef csCameraMotion_;
        lgfx::ComputeShaderRef csLinearDepth_;
    };
}
}
#endif //INC_LFW_RENDERPASSGBUFFER_H__
