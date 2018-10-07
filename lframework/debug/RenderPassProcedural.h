#ifndef INC_DEBUG_RENDERPASSPROCEDURAL_H_
#define INC_DEBUG_RENDERPASSPROCEDURAL_H_
/**
@file RenderPassProcedural.h
@author t-sakai
@date 2017/06/10 create
*/
#include <lframework/render/graph/RenderPass.h>
#include <lgraphics/TextureRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/DepthStencilStateRef.h>

namespace debug
{
    class RenderPassProcedural : public lfw::graph::RenderPass
    {
    public:
        enum Type
        {
            Type_None =-1,
            Type_Space =0,
            Type_Num,
        };

        RenderPassProcedural();
        virtual ~RenderPassProcedural();

        virtual void create(const lfw::Camera& camera);
        virtual void destroy();
        virtual void execute(lfw::RenderContext& renderContext, lfw::Camera& camera);

        void set(Type type);
    protected:
        RenderPassProcedural(const RenderPassProcedural&) = delete;
        RenderPassProcedural& operator=(const RenderPassProcedural&) = delete;

        struct ProcedualConstant
        {
            lfw::f32 width_;
            lfw::f32 height_;
            lfw::f32 time_;
            lfw::f32 reserved_;
        };
        lfw::s32 width_;
        lfw::s32 height_;

        Type type_;
        ProcedualConstant constant_;

        lgfx::SamplerStateRef samLinearMirror_;
        lgfx::SamplerStateRef samLinear_;

        lgfx::VertexShaderRef vsProcedual_[Type_Num];
        lgfx::PixelShaderRef psProcedual_[Type_Num];

        lgfx::ShaderResourceViewRef srvNoise_;

        lgfx::ViewRef viewDepthStencil_;
        lgfx::ViewRef viewAccumLighting_;
    };
}
#endif //INC_DEBUG_RENDERPASSPROCEDURAL_H_
