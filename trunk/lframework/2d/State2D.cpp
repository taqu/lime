/**
@file State2D.cpp
@author t-sakai
@date 2010/03/02 create
*/
#include "State2D.h"

using namespace lgraphics;

namespace lframework
{
    namespace
    {
        const char VertexShader2DSource[] =
        {
            "struct VS_OUTPUT{ float4 position : POSITION; float2 tex : TEXCOORD0; float3 color : COLOR0;};\n"
            "VS_OUTPUT main(float4 position : POSITION, float2 tex : TEXCOORD0, float3 color : COLOR0)\n"
            "{\n"
            "VS_OUTPUT output;\n"
            "output.position = position;\n"
            "output.tex = tex;\n"
            "output.color = color;\n"
            "return output;\n"
            "}"
        };

        const char PixelShader2DSource[] =
        {
            "struct VS_OUTPUT{ float4 position : POSITION; float2 tex : TEXCOORD0; float3 color : COLOR0;};\n"
            "sampler tex1;\n"
            "float4 main(VS_OUTPUT input) : COLOR0\n"
            "{\n"
            "float4 color;\n"
            "color = tex2D(tex1, input.tex).w;\n"
            "color.xyz = input.color;\n"
            "return color;\n"
            "}"
        };
    }

    State2D::State2D()
    {
        // シェーダ作成
        //-----------------------------------------------
        vertexShader_ = Shader::createVertexShaderFromMemory(VertexShader2DSource, sizeof(VertexShader2DSource));
        LASSERT(vertexShader_.valid());

        pixelShader_ = Shader::createPixelShaderFromMemory(PixelShader2DSource, sizeof(PixelShader2DSource));
        LASSERT(pixelShader_.valid());

        // 頂点宣言作成
        //-----------------------------------------------
        VertexDeclCreator declCreator(3);

        u32 offset = 0;

        //変換済み頂点
        declCreator.add(0, offset, lgraphics::DeclType_Float3, lgraphics::DeclUsage_PositionT, 0);
        offset += sizeof(lmath::Vector3);

        declCreator.add(0, offset, lgraphics::DeclType_Float2, lgraphics::DeclUsage_Texcoord, 0);
        offset += sizeof(lmath::Vector2);

        declCreator.add(0, offset, lgraphics::DeclType_Color, lgraphics::DeclUsage_Color, 0);

        declCreator.end(vertexDecl_);


        // ステート作成
        //-----------------------------------------------
        RenderStateRef::begin();
#if 0
        //アルファテスト
        RenderStateRef::setAlphaTest(true);
        RenderStateRef::setAlphaTestFunc(Cmp_GreaterEqual);
        RenderStateRef::setAlphaTestRef(0x80U);
#else
        //アルファブレンド
        RenderStateRef::setAlphaBlendEnable(true);
        RenderStateRef::setAlphaBlend(Blend_SrcAlpha, Blend_InvSrcAlpha);
#endif

        vertexShader_.attach();
        pixelShader_.attach();

        vertexDecl_.attach();

        RenderStateRef::end(renderState_);

    }

    State2D::~State2D()
    {
    }

}
