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
    class RenderSubPassGBuffer
    {
    public:
        friend class RenderPassGBuffer;

        virtual ~RenderSubPassGBuffer()
        {}

        virtual void initialize() =0;
        virtual void execute(RenderContext& renderContext, Camera& camera) =0;
    protected:
        RenderSubPassGBuffer()
        {}

        RenderSubPassGBuffer(const RenderSubPassGBuffer&) = delete;
        RenderSubPassGBuffer& operator=(const RenderSubPassGBuffer&) = delete;

        s32 width_;
        s32 height_;

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
    };

    class RenderPassGBuffer : public RenderPass
    {
    public:
        static const u32 ID_Albedo;
        static const u32 ID_Specular;
        static const u32 ID_DepthNormal;
        static const u32 ID_Velocity;
        static const u32 ID_DSVDepthStencil;
        static const u32 ID_Stencil;

        static const u32 ID_UAVAlbedo;
        static const u32 ID_UAVSpecular;
        static const u32 ID_UAVDepthNormal;
        static const u32 ID_UAVVelocity;

        RenderPassGBuffer();
        virtual ~RenderPassGBuffer();

        virtual void create(const Camera& camera);
        virtual void destroy();
        virtual void execute(RenderContext& renderContext, Camera& camera);

        void addSubPass(RenderSubPassGBuffer* subPass);

    protected:
        RenderPassGBuffer(const RenderPassGBuffer&) = delete;
        RenderPassGBuffer& operator=(const RenderPassGBuffer&) = delete;

        RenderSubPassGBuffer* subPass_;

        s32 width_;
        s32 height_;

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
    };
}
}
#endif //INC_LFW_RENDERPASSGBUFFER_H__
