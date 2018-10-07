#ifndef INC_LFW_RENDERPASSINTEGRATION_H_
#define INC_LFW_RENDERPASSINTEGRATION_H_
/**
@file RenderPassIntegration.h
@author t-sakai
@date 2017/07/04 create
*/
#include "RenderPass.h"
#include <lgraphics/ConstantBufferRef.h>
#include <lgraphics/TextureRef.h>
#include <lgraphics/ShaderRef.h>

namespace lfw
{
    namespace graph
    {
        class RenderPassIntegration : public RenderPass
        {
        public:
            RenderPassIntegration();
            virtual ~RenderPassIntegration();

            virtual void create(const Camera& camera);
            virtual void destroy();
            virtual void execute(RenderContext& renderContext, Camera& camera);

        protected:
            RenderPassIntegration(const RenderPassIntegration&) = delete;
            RenderPassIntegration& operator=(const RenderPassIntegration&) = delete;

            struct GBufferConstant
            {
                s32 width_;
                s32 height_;
                f32 invWidth_;
                f32 invHeight_;
            };

            lgfx::ConstantBufferRef constantGBuffer_;
            lgfx::VertexShaderRef vsFullQuad_;
            lgfx::PixelShaderRef psDeferredIntegration_;

            lgfx::ViewRef viewStencil_;
            lgfx::ViewRef viewFullVelocity_;
            lgfx::ViewRef viewAccumLighting_;
        };
    }
}
#endif //INC_LFW_RENDERPASSINTEGRATION_H_
