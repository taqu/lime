/**
@file DebugPrimitive.cpp
@author t-sakai
@date 2011/01/03
*/
#include "DebugPrimitive.h"

#include <lgraphics/lgraphicscore.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/IndexBufferRef.h>
#include <lgraphics/api/RenderStateRef.h>

#include <lframework/scene/ShaderBase.h>
#include <lframework/scene/shader/DefaultShader.h>

#include "../System.h"
#include "../render/Drawable.h"
#include "../render/RenderingSystem.h"
#include "../scene/Geometry.h"
#include "../scene/Material.h"
#include "../scene/shader/DefaultShader.h"

using namespace lgraphics;

namespace
{
    struct DebugVertex
    {
        lmath::Vector3 position_;
        u32 color_;
    };

// デバッグバーテックスシェーダソース
    const char DebugVSSource[] =
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

    const u32 DebugVSSourceSize = sizeof(DebugVSSource);

    // デバッグピクセルシェーダソース
    const char DebugPSSource[] =
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

    const u32 DebugPSSourceSize = sizeof(DebugPSSource);



    // 頂点宣言作成
    void createVertexDecl(VertexDeclarationRef& decl)
    {
        VertexDeclCreator creator(2);
        u16 voffset = 0;
        voffset += creator.add(0, voffset, DeclType_Float3, DeclMethod_Default, DeclUsage_Position, 0);
        voffset += creator.add(0, voffset, DeclType_UB4, DeclMethod_Normalize, DeclUsage_Color, 0);


        creator.end(decl);
    }

    // レンダリングステート作成
    void createRenderState(RenderStateRef& state)
    {
        //ステート設定はてきとう
        state.setCullMode(lgraphics::CullMode_None);

        // アルファ設定
        state.setAlphaBlendEnable(true);
        state.setAlphaBlend(lgraphics::Blend_SrcAlpha, lgraphics::Blend_InvSrcAlpha);
        state.setAlphaTest(false);
        state.setAlphaTestFunc(lgraphics::Cmp_Less);

        // Zバッファ設定
        state.setZEnable(true);
        state.setZWriteEnable(true);
    }

    //-----------------------------------------------------------------------------
    // マテリアル設定
    void setMaterialParameter(lscene::Material& material)
    {
        material.diffuse_.set(0.0f, 0.0f, 0.0f, 1.0f);
        material.specular_.set(0.0f, 0.0f, 0.0f, 0.0f);
        material.ambient_.set(0.0f, 0.0f, 0.0f);
    }

    //頂点バッファ作成
    void createVertexBuffer(VertexBufferRef& vb, u32 numVertices)
    {
        vb = lgraphics::VertexBuffer::create(sizeof(DebugVertex), numVertices, Pool_Default, Usage_Dynamic);
    }

    //void createIndexBuffer(IndexBufferRef& ib, u32 numIndices)
    //{
    //    ib = IndexBuffer::create(numIndices, Pool_Default, Usage_None);
    //}



    //---------------------------------------------------
    //---
    //--- DebugShaderVS
    //---
    //---------------------------------------------------
    class DebugShaderVS : public lscene::ShaderVSBase
    {
    public:
        enum ParamVS
        {
            ParamVS_WVP =0,
            ParamVS_Num,
        };

        DebugShaderVS()
        {}

        DebugShaderVS(const lgraphics::VertexShaderRef& ref)
            :ShaderVSBase(ref)
        {}

        virtual ~DebugShaderVS()
        {}

        virtual void initialize()
        {
            for(s32 i=0; i<ParamVS_Num; ++i){
                setHandle(params_[i], lscene::shader::ParamVSNames[i]);
            }
        }

        virtual void setParameters(const lrender::Batch&, const lscene::Scene& scene)
        {

            shader_.setMatrix(params_[ParamVS_WVP], scene.getViewProjMatrix());
        }

        lgraphics::HANDLE params_[ParamVS_Num];
    };


    //---------------------------------------------------
    //---
    //--- DebugShaderPS
    //---
    //---------------------------------------------------
    class DebugShaderPS : public lscene::ShaderPSBase
    {
    public:
        DebugShaderPS()
        {}

        DebugShaderPS(const lgraphics::PixelShaderRef& ref)
            :ShaderPSBase(ref)
        {}

        ~DebugShaderPS()
        {}

        virtual void initialize(){}
        virtual void setParameters(const lrender::Batch&, const lscene::Scene&){}
    };


    void createShader(DebugShaderVS& vs, DebugShaderPS& ps, VertexDeclarationRef& decl)
    {
#if defined(LIME_GLES2)
        lgraphics::Descriptor* vsDesc = lgraphics::Shader::createVertexShaderFromMemory(
                DebugVSSource,
                DebugVSSourceSize,
                0,
                NULL);
        LASSERT(vsDesc != NULL);

        lgraphics::Descriptor* psDesc = lgraphics::Shader::createPixelShaderFromMemory(
                DebugPSSource,
                DebugPSSourceSize,
                0,
                NULL);
        LASSERT(psDesc != NULL);

        lgraphics::VertexShaderRef vsRef;
        lgraphics::PixelShaderRef psRef;
        bool ret = lgraphics::Shader::linkShader(vsRef, psRef, decl, vsDesc, psDesc);
        LASSERT(ret);

        vs.getShaderRef() = vsRef;
        ps.getShaderRef() = psRef;
#else

        lgraphics::VertexShaderRef vsRef = lgraphics::Shader::createVertexShaderFromMemory(
                DebugVSSource,
                DebugVSSourceSize,
                0,
                NULL);

        lgraphics::PixelShaderRef psRef = lgraphics::Shader::createPixelShaderFromMemory(
                DebugPSSource,
                DebugPSSourceSize,
                0,
                NULL);


        vs.getShaderRef() = vsRef;
        ps.getShaderRef() = psRef;
#endif
        vs.initialize();
        ps.initialize();
    }
}

namespace debug
{
    //------------------------------------------
    //---
    //--- DebugPrimitive::PrimitiveBatch
    //---
    //------------------------------------------
    class DebugPrimitive::PrimitiveBatch : public lrender::Drawable
    {
    public:
        PrimitiveBatch(u16 numMaxTriangles);
        ~PrimitiveBatch();

        void draw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color);
        void clear();
    private:
        PrimitiveBatch(const PrimitiveBatch&);
        PrimitiveBatch& operator=(const PrimitiveBatch&);

        lscene::Geometry geometryInstance_;
        lscene::Material materialInstance_;

        DebugShaderVS shaderVS_;
        DebugShaderPS shaderPS_;

        u16 numMaxTriangles_;
        u16 count_;

        DebugVertex buffer_[3];
    };

    DebugPrimitive::PrimitiveBatch::PrimitiveBatch(u16 numMaxTriangles)
        :numMaxTriangles_(numMaxTriangles)
        ,count_(0)
    {

        VertexDeclarationRef decl;
        createVertexDecl(decl);

        VertexBufferRef vb;
        createVertexBuffer(vb, numMaxTriangles_*3);
#if defined(LIME_GLES2)
        vb.blit(NULL, true); //バッファ予約
#endif
        IndexBufferRef ib;

        GeometryBuffer::pointer geomBuffer( LIME_NEW GeometryBuffer(lgraphics::Primitive_TriangleList, decl, vb, ib));

        lscene::Geometry geometry(geomBuffer, numMaxTriangles_, 0, 0);
        geometryInstance_.swap(geometry);

        setMaterialParameter(materialInstance_);
        createRenderState( materialInstance_.getRenderState() );

        createShader(shaderVS_, shaderPS_, decl);

        geometryInstance_.setShaderVS(&shaderVS_);
        geometryInstance_.setShaderPS(&shaderPS_);

        numGeometries_ = 1;
        geometries_ = &geometryInstance_;
        numMaterials_ = 1;
        materials_ = &materialInstance_;
    }

    DebugPrimitive::PrimitiveBatch::~PrimitiveBatch()
    {
    }

    void DebugPrimitive::PrimitiveBatch::draw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color)
    {
        if(count_>=numMaxTriangles_){
            return;
        }

        lgraphics::GeometryBuffer::pointer& geomBuffer = geometryInstance_.getGeometryBuffer();
        lgraphics::VertexBufferRef& vb = geomBuffer->getVertexBuffer();

        buffer_[0].position_ = v0;
        buffer_[0].color_ = color;

        buffer_[1].position_ = v1;
        buffer_[1].color_ = color;

        buffer_[2].position_ = v2;
        buffer_[2].color_ = color;

        u32 offset = 3 * count_ * sizeof(DebugVertex);
        u32 size = 3 * sizeof(DebugVertex);

        vb.blit(reinterpret_cast<void*>(buffer_), offset, size);
        ++count_;

        geometryInstance_.setPrimitiveCount(static_cast<u16>(0xFFFFU&count_));
    }

    void DebugPrimitive::PrimitiveBatch::clear()
    {
        count_ = 0;
    }


    //------------------------------------------
    //---
    //--- DebugPrimitive
    //---
    //------------------------------------------
    DebugPrimitive::DebugPrimitive()
        :batch_(NULL)
    {
    }

    DebugPrimitive::~DebugPrimitive()
    {
        LASSERT(batch_ == NULL);
    }

    void DebugPrimitive::initialize(u16 numMaxTriangles)
    {
        //batch_ = LIME_NEW PrimitiveBatch(numMaxTriangles);
    }

    void DebugPrimitive::terminate()
    {
        //lframework::System::getRenderSys().remove(lrender::Pass_Main, batch_);
        //LIME_DELETE(batch_);
    }

    void DebugPrimitive::attach()
    {
        //lframework::System::getRenderSys().add(lrender::Pass_Main, batch_);
    }

    void DebugPrimitive::detach()
    {
        //lframework::System::getRenderSys().remove(lrender::Pass_Main, batch_);
    }

    void DebugPrimitive::draw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color)
    {
        //LASSERT(batch_ != NULL);
        //batch_->draw(v0, v1, v2, color);
    }

    void DebugPrimitive::clear()
    {
        //LASSERT(batch_ != NULL);
        //batch_->clear();
    }
}
