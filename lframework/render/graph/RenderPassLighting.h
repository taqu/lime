#ifndef INC_LFW_RENDERPASSLIGHTING_H__
#define INC_LFW_RENDERPASSLIGHTING_H__
/**
@file RenderPassLighting.h
@author t-sakai
@date 2017/07/06 create
*/
#include "RenderPass.h"
#include <lmath/Vector4.h>
#include <lgraphics/ConstantBufferRef.h>
#include <lgraphics/TextureRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/DepthStencilStateRef.h>

namespace lmath
{
    class Matrix34;
}

namespace lfw
{
    namespace graph
    {
        class RenderPassLighting : public RenderPass
        {
        public:
            static const u32 ID_RTVAccumLighting;
            static const u32 ID_SRVAccumLighting;

            RenderPassLighting();
            virtual ~RenderPassLighting();

            virtual void create(const Camera& camera);
            virtual void destroy();
            virtual void execute(RenderContext& renderContext, Camera& camera);

        protected:
            RenderPassLighting(const RenderPassLighting&) = delete;
            RenderPassLighting& operator=(const RenderPassLighting&) = delete;

            s32 width_;
            s32 height_;
            struct GBufferConstant
            {
                s32 width_;
                s32 height_;
                f32 invWidth_;
                f32 invHeight_;
            };

            struct DirectionalLight
            {
                lmath::Vector4 color_;
                lmath::Vector4 direction_;
            };

            struct PointLight
            {
                lmath::Vector4 color_;
                lmath::Vector4 position_; //position, radius
            };

            struct SpotLight
            {
                lmath::Vector4 color_;
                lmath::Vector4 position_; //position, angle
                lmath::Vector4 direction_; //direction, falloffAngle
            };

            lmath::Matrix34* lightMatrices_;
            DirectionalLight directionalLights_[LFW_CONFIG_MAX_DIRECTIONAL_LIGHTS];
            PointLight pointLights_[LFW_CONFIG_MAX_POINT_LIGHTS];
            SpotLight spotLights_[LFW_CONFIG_MAX_SPOT_LIGHTS];

            lgfx::DepthStencilStateRef depthStencilStateBack_;
            lgfx::DepthStencilStateRef depthStencilStateFront_;
            lgfx::DepthStencilStateRef depthStencilState_;

            lgfx::ConstantBufferRef constantGBuffer_;

            lgfx::VertexShaderRef vsFullQuadInstanced_;
            lgfx::VertexShaderRef vsDeferredLighting_;

            lgfx::PixelShaderRef psDeferredLightingDirectional_;
            lgfx::PixelShaderRef psDeferredLightingPoint_;
            lgfx::PixelShaderRef psDeferredLightingCone_;

            lgfx::ViewRef viewAlbedo_;
            lgfx::ViewRef viewSpecular_;
            lgfx::ViewRef viewDepthNormal_;
            lgfx::ViewRef viewVelocity_;
            lgfx::ViewRef viewDepthStencil_;
            lgfx::ViewRef viewStencil_;
            lgfx::ViewRef viewFullVelocity_;

            lgfx::RenderTargetViewRef rtvAccumLighting_;
            lgfx::ShaderResourceViewRef srvAccumLighting_;

            lgfx::ResourceRef resourceDepthStencil_;
            lgfx::Texture2DRef texDepthStencil_;
            lgfx::DepthStencilViewRef dsvDepthStencil_;
        };
    }
}
#endif //INC_LFW_RENDERPASSLIGHTING_H__
