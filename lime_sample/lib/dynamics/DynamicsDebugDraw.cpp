/**
@file DynamicsDebugDraw.cpp
@author t-sakai
@date 2011/07/26
*/
#include <lmath/lmath.h>
#include "DynamicsDebugDraw.h"

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/VertexDeclarationRef.h>

#include "System.h"
#include <lframework/scene/ShaderBase.h>
#include "scene/shader/DefaultShader.h"

#include "render/RenderingSystem.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

using namespace lgraphics;

namespace dynamics
{
namespace
{
    // ラインVS
    const char LineVSSource[] =
#if defined(LIME_GLES2)
        "precision mediump float;\n"
        "precision mediump int;\n"
        "attribute vec3 a_pos0;\n"
        "attribute vec4 a_c0;\n"
        "varying vec4 v_c0;\n"
        "uniform mat4 mwvp;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(a_pos0, float(1)) * mwvp;\n"
        "v_c0 = a_c0;\n"
        "}\n"
#else
        "struct VSInput\n"
        "{\n"
        "float4 position : POSITION;\n"
        "float2 tex0 : TEXCOORD0;\n"
        "};\n"
        "struct VSOutput\n"
        "{\n"
        "float4 position : POSITION0;\n"
        "float2 tex0 : TEXCOORD0;\n"
        "};\n"
        "VSOutput main(VSInput input)\n"
        "{\n"
        "VSOutput output = (VSOutput)0;"
        "output.position = float4(input.position.x, input.position.y, 0.1, 1.0);\n"
        "output.tex0 = input.tex0;\n"
        "return output;\n"
        "}\n"
#endif
        ;

    const u32 LineVSSourceSize = sizeof(LineVSSource);

    // ラインPS
    const char LinePSSource[] =
#if defined(LIME_GLES2)
        "precision mediump float;\n"
        "precision mediump int;\n"
        "varying vec4 v_c0;\n"
        "void main()\n"
        "{\n"
        "gl_FragColor=v_c0;\n"
        "}\n"
#else
        "struct VSOutput\n"
        "{\n"
        "float4 position : POSITION0;\n"
        "float4 tex0 : TEXCOORD0;\n"
        "};\n"
        "sampler texAlbedo;\n"
        "float4 main(VSOutput input) : COLOR\n"
        "{\n"
        "return tex2D(texAlbedo, input.tex0);\n"
        "}\n"
#endif
        ;

    const u32 LinePSSourceSize = sizeof(LinePSSource);


    // 頂点宣言作成
    void createVertexDecl(VertexDeclarationRef& decl)
    {
        VertexDeclCreator creator(2);
        u16 voffset = 0;
        voffset += creator.add(0, voffset, DeclType_Float3, DeclMethod_Default, DeclUsage_Position, 0);
        voffset += creator.add(0, voffset, DeclType_UB4, DeclMethod_Default, DeclUsage_Color, 0);

        creator.end(decl);
    }

    void createShader(DynamicsDebugDraw::LineShaderVS& vs, DynamicsDebugDraw::LineShaderPS& ps, VertexDeclarationRef& decl)
    {
#if defined(LIME_GLES2)
        lgraphics::Descriptor* vsDesc = lgraphics::Shader::createVertexShaderFromMemory(
                LineVSSource,
                LineVSSourceSize,
                0,
                NULL);
        LASSERT(vsDesc != NULL);

        lgraphics::Descriptor* psDesc = lgraphics::Shader::createPixelShaderFromMemory(
                LinePSSource,
                LinePSSourceSize,
                0,
                NULL);
        LASSERT(psDesc != NULL);

        lgraphics::VertexShaderRef vsRef;
        lgraphics::PixelShaderRef psRef;
        lgraphics::Shader::linkShader(vsRef, psRef, 1, decl, vsDesc, psDesc);

        vs.getShaderRef() = vsRef;
        ps.getShaderRef() = psRef;
#else

        lgraphics::VertexShaderRef vsRef = lgraphics::Shader::createVertexShaderFromMemory(
                TextVSSource,
                TextVSSourceSize,
                0,
                NULL);

        lgraphics::PixelShaderRef psRef = lgraphics::Shader::createPixelShaderFromMemory(
                TextPSSource,
                TextPSSourceSize,
                0,
                NULL);


        vs.getShaderRef() = vsRef;
        ps.getShaderRef() = psRef;
#endif
        vs.initialize();
        ps.initialize();
    }
}

    void DynamicsDebugDraw::LineShaderVS::initialize()
    {
        LASSERT(shader_.valid());

        setHandle(params_[0], "mwvp");
    }

    void DynamicsDebugDraw::LineShaderVS::setParameters()
    {
        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();

        shader_.setMatrix(params_[ParamVS_WVP], renderSys.getScene().getViewProjMatrix());
    }

    DynamicsDebugDraw::DynamicsDebugDraw()
        :debugMode_(DBG_DrawWireframe|DBG_FastWireframe|DBG_DrawConstraints)
        //debugMode_(DBG_DrawConstraints)
        ,numLines_(0)
    {
    }

    DynamicsDebugDraw::~DynamicsDebugDraw()
    {
    }

    void DynamicsDebugDraw::initialize()
    {
        createVertexDecl(decl_);

        createShader(shaderVS_, shaderPS_, decl_);

        numLines_ = 0;
    }

    void DynamicsDebugDraw::terminate()
    {
        shaderPS_ = LineShaderPS();
        shaderVS_ = LineShaderVS();
        decl_ = VertexDeclarationRef();

        numLines_ = 0;
    }

    void DynamicsDebugDraw::flush()
    {
        if(numLines_>0){
            lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

            device.setCullMode(lgraphics::CullMode_CCW);

            // アルファ設定
            device.setAlphaBlendEnable(false);
            device.setAlphaTest(false);

            // Zバッファ設定
            device.setZEnable(false);
            device.setZWriteEnable(false);

            shaderVS_.attach();
            shaderVS_.setParameters();
            shaderPS_.attach();

            glBindBuffer( GL_ARRAY_BUFFER, NULL );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );

            decl_.attach(0, reinterpret_cast<const u8*>(vertices_));


            device.draw(lgraphics::Primitive_LineList, numLines_*2, 0);

            decl_.detach(0);

            numLines_ = 0;
        }
    }

    void DynamicsDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        if(numLines_<MaxLines){
            u32 index = numLines_*2;

            u32 c = (0xFFU<<24) | (static_cast<u8>(color.x()*255.0f)<<16) | (static_cast<u8>(color.y()*255.0f)<<8) | static_cast<u8>(color.z()*255.0f);
            vertices_[index].position_.set(from.x(), from.y(), from.z());
            vertices_[index].color_ = c;

            vertices_[index+1].position_.set(to.x(), to.y(), to.z());
            vertices_[index+1].color_ = c;

            ++numLines_;
        }
    }

    void DynamicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        if(numLines_<MaxLines){
            u32 index = numLines_*2;

            u32 c = (0xFFU<<24) | (static_cast<u8>(color.x()*255.0f)<<16) | (static_cast<u8>(color.y()*255.0f)<<8) | static_cast<u8>(color.z()*255.0f);
            vertices_[index].position_.set(PointOnB.x(), PointOnB.y(), PointOnB.z());
            vertices_[index].color_ = c;

            btVector3 PointOnA = normalOnB;
            PointOnA *= distance;
            PointOnA += PointOnB;

            vertices_[index+1].position_.set(PointOnA.x(), PointOnA.y(), PointOnA.z());
            vertices_[index+1].color_ = c;

            ++numLines_;
        }
    }

    void DynamicsDebugDraw::draw3dText(const btVector3& location,const char* textString)
    {
    }

    void DynamicsDebugDraw::setDebugMode(int debugMode)
    {
        debugMode_ = debugMode;
    }

    int DynamicsDebugDraw::getDebugMode() const
    {
        return debugMode_;
    }

    void DynamicsDebugDraw::reportErrorWarning(const char* warningString)
    {
        //lcore::Log("%s", warningString);
    }
}
