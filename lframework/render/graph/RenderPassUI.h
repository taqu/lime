#ifndef INC_LFW_RENDERPASSUI_H__
#define INC_LFW_RENDERPASSUI_H__
/**
@file RenderPassUI.h
@author t-sakai
@date 2017/07/16 create
*/
#include "RenderPass.h"
#include <lgraphics/TextureRef.h>
#include <lgraphics/ShaderRef.h>

namespace lfw
{
namespace graph
{
    class RenderPassUI : public RenderPass
    {
    public:
        static const u32 ID_Albedo;
        static const u32 ID_Specular;
        static const u32 ID_DepthNormal;
        static const u32 ID_Velocity;
        static const u32 ID_DSVDepthStencil;
        static const u32 ID_Stencil;
        static const u32 ID_FullVelocity;

        RenderPassUI();
        virtual ~RenderPassUI();

        virtual void create(const Camera& camera);
        virtual void destroy();
        virtual void execute(RenderContext& renderContext, Camera& camera);

    protected:
        RenderPassUI(const RenderPassUI&) = delete;
        RenderPassUI& operator=(const RenderPassUI&) = delete;

    };
}
}
#endif //INC_LFW_RENDERPASSUI_H__
