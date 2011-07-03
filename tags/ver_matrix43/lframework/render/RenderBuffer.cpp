/**
@file RenderBuffer.cpp
@author t-sakai
@date 2011/03/07 create

*/
#include "RenderBuffer.h"

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/RenderStateRef.h>

namespace lrender
{
    void RenderBufferBatchState::create()
    {
        lgraphics::VertexDeclCreator creator(2);
        u16 offset = 0;
        creator.add(0, 0, lgraphics::DeclType_Float2, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Position, 0);
        creator.add(0, sizeof(lmath::Vector2), lgraphics::DeclType_Float2, lgraphics::DeclMethod_Default, lgraphics::DeclUsage_Texcoord, 0);
        creator.end(decl_);
    }

    inline void RenderBufferBatchState::attach()
    {
        decl_.attach();
    }


// デバッグバーテックスシェーダソース
    const Char RenderBuffer::VSSource[] =
#if defined(LIME_GLES2)
        "precision highp float;\n"
        "precision mediump int;\n"
        "uniform mat4 mwvp;\n"
        "attribute vec3 a_pos0;\n"
        "attribute vec4 a_color0;\n"
        "varying vec4 v_color0;\n"
        "void main()\n"
        "{\n"
        "gl_Position = mwvp * vec4(a_pos0.xyz, float(1));\n"
        "v_color0 = a_color0;\n"
        "}\n"
#else

        "uniform float4x4 mwvp;\n"
        "struct VSInput\n"
        "{\n"
        "float4 position : POSITION;\n"
        "float4 color0 : COLOR0;\n"
        "};\n"
        "struct VSOutput\n"
        "{\n"
        "float4 position : POSITION0;\n"
        "float4 color0 : COLOR0;\n"
        "};\n"
        "VSOutput main(VSInput input)\n"
        "{\n"
        "VSOutput output = (VSOutput)0;"
        "output.position = mul(input.position, mwvp);\n"
        "output.color0 = input.color0;\n"
        "return output;\n"
        "}\n"
#endif
        ;

    const u32 RenderBuffer::VSSourceSize = sizeof(RenderBuffer::VSSource);

    // デバッグピクセルシェーダソース
    const Char RenderBuffer::PSSource[] =
#if defined(LIME_GLES2)
        "precision highp float;\n"
        "precision mediump int;\n"
        "varying vec4 v_color0;\n"
        "void main()\n"
        "{\n"
        "gl_FragColor = v_color0;\n"
        "}\n"
#else
        "struct VSOutput\n"
        "{\n"
        "float4 position : POSITION0;\n"
        "float4 color0 : COLOR0;\n"
        "};\n"
        "float4 main(VSOutput input) : COLOR\n"
        "{\n"
        "return input.color0;\n"
        "}\n"
#endif
        ;

    const u32 RenderBuffer::PSSourceSize = sizeof(RenderBuffer::PSSource);

    RenderBuffer::RenderBuffer()
        :surface_(NULL)
        ,batchState_(NULL)
    {
    }

    RenderBuffer::~RenderBuffer()
    {
        SAFE_RELEASE(surface_);
    }

    RenderBuffer::RenderBuffer(IDirect3DSurface9* surface, RenderBufferBatchState* batchState)
        :surface_(surface)
        ,batchState_(batchState)
    {
    }


    void RenderBuffer::draw()
    {
        lgraphics::RenderStateRef::setAlphaBlendEnable(false);
        lgraphics::RenderStateRef::setAlphaTest(false);
        lgraphics::RenderStateRef::setZEnable(false);
        lgraphics::RenderStateRef::setZWriteEnable(false);

        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

    }

    void RenderBuffer::swap(RenderBuffer& rhs)
    {
        lcore::swap(batchState_, rhs.batchState_);
        surface_.swap(rhs.surface_);
    }

    void RenderBuffer::createGeometry()
    {
        vertices_[0].position_.set(-1.0f, 1.0f);
        vertices_[0].uv_.set(0.0f, 0.0f);

        vertices_[1].position_.set(1.0f, 1.0f);
        vertices_[1].uv_.set(1.0f, 0.0f);

        vertices_[2].position_.set(-1.0f, -1.0f);
        vertices_[2].uv_.set(0.0f, 1.0f);

        vertices_[3].position_.set(1.0f, -1.0f);
        vertices_[3].uv_.set(1.0f, 1.0f);

    }
}
