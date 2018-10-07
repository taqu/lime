#ifndef INC_LFW_RENDERPASSTRANSPARENT_H_
#define INC_LFW_RENDERPASSTRANSPARENT_H_
/**
@file RenderPassTransparent.h
@author t-sakai
@date 2017/07/17 create
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
    class RenderPassTransparent : public RenderPass
    {
    public:
        RenderPassTransparent();
        virtual ~RenderPassTransparent();

        virtual void create(const Camera& camera);
        virtual void destroy();
        virtual void execute(RenderContext& renderContext, Camera& camera);

    protected:
        RenderPassTransparent(const RenderPassTransparent&) = delete;
        RenderPassTransparent& operator=(const RenderPassTransparent&) = delete;

        s32 width_;
        s32 height_;

        lgfx::ViewRef rtvAccumLighting_;
        lgfx::ViewRef dsvDepthStencil_;

        lgfx::ViewRef srvLinearDepth_;
    };
}
}
#endif //INC_LFW_RENDERPASSTRANSPARENT_H_
