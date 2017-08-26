#ifndef INC_DEBUG_RENDERPASSTERRAIN_H__
#define INC_DEBUG_RENDERPASSTERRAIN_H__
/**
@file RenderPassTerrain.h
@author t-sakai
@date 2017/06/10 create
*/
#include <lframework/render/graph/RenderPassGBuffer.h>

namespace debug
{
    class RenderPassTerrain : public lfw::graph::RenderSubPassGBuffer
    {
    public:
        enum Type
        {
            Type_None =-1,
            Type_Land,
            Type_Num,
        };

        RenderPassTerrain();
        virtual ~RenderPassTerrain();

        virtual void initialize();
        virtual void execute(lfw::RenderContext& renderContext, lfw::Camera& camera);

        void set(Type type);
    protected:
        RenderPassTerrain(const RenderPassTerrain&) = delete;
        RenderPassTerrain& operator=(const RenderPassTerrain&) = delete;

        struct ProcedualConstant
        {
            lfw::f32 width_;
            lfw::f32 height_;
            lfw::f32 invWidth_;
            lfw::f32 invHeight_;
        };
        //lfw::s32 xthreads_;
        //lfw::s32 ythreads_;
        //lfw::s32 halfXThreads_;
        //lfw::s32 halfYThreads_;

        Type type_;
        ProcedualConstant constant_;

        lgfx::SamplerStateRef samLinearMirror_;
        lgfx::SamplerStateRef samLinear_;

        lgfx::VertexShaderRef vsProcedual_;
        lgfx::PixelShaderRef psProcedual_[Type_Num];

        lgfx::ShaderResourceViewRef srvNoise_;
    };
}
#endif //INC_DEBUG_RENDERPASSTERRAIN_H__
