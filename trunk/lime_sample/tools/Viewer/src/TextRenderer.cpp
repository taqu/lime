/**
@file TextRenderer.cpp
@author t-sakai
@date 2011/04/01 create
*/
#include "stdafx.h"
#include "TextRenderer.h"

#include <lcore/liostream.h>

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/IndexBufferRef.h>
#include <lgraphics/api/RenderStateRef.h>
#include <lgraphics/api/SamplerState.h>
#include <lgraphics/api/TextureRef.h>

#include <lgraphics/io/IOPNG.h>

#include <lframework/scene/Material.h>
#include <lframework/scene/Geometry.h>
#include <lframework/scene/ShaderBase.h>
#include <lframework/scene/shader/DefaultShader.h>

#include <lframework/render/Batch.h>

using namespace lgraphics;
namespace viewer
{
namespace
{
    struct TextVertex
    {
        lmath::Vector2 position_;
        u16 uv_[2];
    };

    // テキストVS
    const char TextVSSource[] =
#if defined(LIME_GLES2)
        "precision mediump float;\n"
        "precision mediump int;\n"
        "attribute vec2 a_pos0;\n"
        "attribute vec2 a_tex0;\n"
        "varying vec2 v_tex0;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(a_pos0.x, a_pos0.y, 0.1, float(1));\n"
        "v_tex0 = a_tex0;\n"
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

    const u32 TextVSSourceSize = sizeof(TextVSSource);

    // テキストPS
    const char TextPSSource[] =
#if defined(LIME_GLES2)
        "precision mediump float;\n"
        "precision mediump int;\n"
        "uniform sampler2D texAlbedo;\n"
        "varying vec2 v_tex0;\n"
        "void main()\n"
        "{\n"
        "vec4 c = texture2D(texAlbedo, v_tex0);\n"
        "if(c.r==0.0){discard;}else{gl_FragColor=c;}\n"
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

    const u32 TextPSSourceSize = sizeof(TextPSSource);


    // 頂点宣言作成
    void createVertexDecl(VertexDeclarationRef& decl)
    {
        VertexDeclCreator creator(2);
        u16 voffset = 0;
        voffset += creator.add(0, voffset, DeclType_Float2, DeclMethod_Default, DeclUsage_Position, 0);
        voffset += creator.add(0, voffset, DeclType_UShort2N, DeclMethod_Normalize, DeclUsage_Texcoord, 0);


        creator.end(decl);
    }

    // レンダリングステート作成
    void createRenderState(RenderStateRef& state)
    {
        //ステート設定はてきとう
        state.setCullMode(lgraphics::CullMode_CCW);

        // アルファ設定
        state.setAlphaBlendEnable(false);
        state.setAlphaBlend(lgraphics::Blend_SrcAlpha, lgraphics::Blend_InvSrcAlpha);
        state.setAlphaTest(true);
        state.setAlphaTestFunc(lgraphics::Cmp_Greater);

        // Zバッファ設定
        state.setZEnable(false);
        state.setZWriteEnable(false);
    }

    //---------------------------------------------------
    //---
    //--- TextShaderVS
    //---
    //---------------------------------------------------
    class TextShaderVS : public lscene::ShaderVSBase
    {
    public:
        TextShaderVS()
        {}

        TextShaderVS(const lgraphics::VertexShaderRef& ref)
            :ShaderVSBase(ref)
        {}

        virtual ~TextShaderVS()
        {}

        virtual void initialize()
        {
        }

        virtual void setParameters(const lrender::Batch&, const lscene::Scene&)
        {
        }
    };


    //---------------------------------------------------
    //---
    //--- TextShaderPS
    //---
    //---------------------------------------------------
    class TextShaderPS : public lscene::ShaderPSBase
    {
    public:
        enum TexPS
        {
            TexPS_Albedo =0,
            TexPS_Num,
        };

        TextShaderPS()
        {}

        TextShaderPS(const lgraphics::PixelShaderRef& ref)
            :ShaderPSBase(ref)
        {}

        ~TextShaderPS()
        {}

        virtual void initialize()
        {
            setSampler(textures_[0], "texAlbedo");
        }

        virtual void setParameters(const lrender::Batch& , const lscene::Scene& )
        {
        }

        void setParameters(const lscene::Material& material)
        {
            const lgraphics::TextureRef& tex = material.getTexture(0);
#if defined(LIME_GLES2)

            lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
            device.setTexture(0, tex.getTextureID(), textures_[0]);

#else
            const lgraphics::SamplerState& samplerState = material.getSamplerState(0);
            samplerState.apply( textures_[0] );
            tex.attach(0);
#endif
        }

        u32 textures_[TexPS_Num];
    };


    void createShader(TextShaderVS& vs, TextShaderPS& ps, VertexDeclarationRef& decl)
    {
#if defined(LIME_GLES2)
        lgraphics::Descriptor* vsDesc = lgraphics::Shader::createVertexShaderFromMemory(
                TextVSSource,
                TextVSSourceSize,
                0,
                NULL);
        LASSERT(vsDesc != NULL);

        lgraphics::Descriptor* psDesc = lgraphics::Shader::createPixelShaderFromMemory(
                TextPSSource,
                TextPSSourceSize,
                0,
                NULL);
        LASSERT(psDesc != NULL);

        lgraphics::VertexShaderRef vsRef;
        lgraphics::PixelShaderRef psRef;
        bool ret = lgraphics::Shader::linkShader(vsRef, psRef, 1, decl, vsDesc, psDesc);
        LASSERT(ret);

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

    //------------------------------------------
    //---
    //--- TextRenderer::PrimitiveBatch
    //---
    //------------------------------------------
    class TextRenderer::PrimitiveBatch : public lrender::Batch
    {
    public:
        static const s32 MaxRowChars = 16;
        static const u8 MinControlCode = 0x01U;
        static const u8 MaxControlCode = 0x20U;

        static const u8 MinCharCode = 0x20U;
        static const u8 MaxCharCode = 0x7EU;

        PrimitiveBatch(u32 numMaxCharacters, u32 charW, u32 charH, u32 numRows, u32 numCols);
        ~PrimitiveBatch();

        void draw();
        void clear();

        void print(s32 row, s32 col, const u8* str);

        void setViewSize(u32 numRows, u32 numCols)
        {
            unitW_ = (2.0f/numCols);
            unitH_ = (2.0f/numRows);
        }

        void setTextureSize(u32 width, u32 height)
        {
            f32 maxu = static_cast<f32>(charW_<<4) / width * (1.0f/16.0f);
            f32 maxv = static_cast<f32>(charH_* 6) / height * (1.0f/6.0f);
            unitTexW_ = static_cast<u16>(0xFFFFU*maxu);
            unitTexH_ = static_cast<u16>(0xFFFFU*maxv);

        }

        PrimitiveBatch(const PrimitiveBatch&);
        PrimitiveBatch& operator=(const PrimitiveBatch&);

        VertexDeclarationRef decl_;
        lscene::Material material_;

        TextShaderVS shaderVS_;
        TextShaderPS shaderPS_;

        u32 numMaxCharacters_;
        u32 count_;

        TextVertex* buffer_;

        f32 unitW_;
        f32 unitH_;

        u16 charW_;
        u16 charH_;
        u16 unitTexW_;
        u16 unitTexH_;
    };

    TextRenderer::PrimitiveBatch::PrimitiveBatch(u32 numMaxCharacters, u32 charW, u32 charH, u32 numRows, u32 numCols)
        :numMaxCharacters_(numMaxCharacters)
        ,count_(0)
        ,unitW_(2.0f/numCols)
        ,unitH_(2.0f/numRows)
        ,charW_(charW)
        ,charH_(charH)
        ,unitTexW_(0)
        ,unitTexH_(0)
    {
        createVertexDecl(decl_);

        createRenderState( material_.getRenderState() );

        createShader(shaderVS_, shaderPS_, decl_);

        buffer_ = LIME_NEW TextVertex[(numMaxCharacters_*6)];
    }

    TextRenderer::PrimitiveBatch::~PrimitiveBatch()
    {
        LIME_DELETE_ARRAY(buffer_)
    }

    void TextRenderer::PrimitiveBatch::clear()
    {
        count_ = 0;
    }

    void TextRenderer::PrimitiveBatch::draw()
    {
        if(count_>0){
            lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

            device.setZEnable(false);
            device.setAlphaBlendEnable(false);

            material_.applyRenderState();

            shaderVS_.attach();

            shaderPS_.setParameters(material_);

            glBindBuffer( GL_ARRAY_BUFFER, NULL );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL );

            decl_.attach( 0, reinterpret_cast<const u8*>(buffer_) );

#if defined(LIME_GLES2)
            device.draw(Primitive_TriangleList, count_*6, 0);
#else
            device.drawIndexed(Primitive_TriangleList, count_ * 4, count_*2, 0);
#endif
            decl_.detach(0);
            clear();
        }
    }

    void TextRenderer::PrimitiveBatch::print(s32 row, s32 col, const u8* str)
    {
        LASSERT(str != NULL);

        f32 sx = unitW_ * col - 1.0f;
        f32 sy = 1.0f - unitH_ * row;
        f32 ex = sx + unitW_;
        f32 ey = sy - unitH_;

        u16 u;
        u16 v;

        u16 index = count_ * 6;
        while(*str != '\0'){
            if(count_>=numMaxCharacters_){
                return;
            }

            if(MinControlCode<=*str && *str<=MaxControlCode){
                ++str;
                sx += unitW_;
                ex += unitW_;
                continue;
            }

            if(MinCharCode<=*str && *str<=MaxCharCode){

                u8 r = (*str - MinCharCode) >> 4;
                u8 c = (*str - MinCharCode) - (r<<4);
                u = c*unitTexW_;
                v = r*unitTexH_;

                //-----------------------------------------
                buffer_[index].position_.set(sx, sy);
                buffer_[index].uv_[0] = u;
                buffer_[index].uv_[1] = v;
                ++index;

                buffer_[index].position_.set(ex, sy);
                buffer_[index].uv_[0] = u + unitTexW_;
                buffer_[index].uv_[1] = v;
                ++index;

                buffer_[index].position_.set(sx, ey);
                buffer_[index].uv_[0] = u;
                buffer_[index].uv_[1] = v + unitTexH_;
                ++index;


                //-----------------------------------------
                buffer_[index].position_.set(sx, ey);
                buffer_[index].uv_[0] = u;
                buffer_[index].uv_[1] = v + unitTexH_;
                ++index;

                buffer_[index].position_.set(ex, sy);
                buffer_[index].uv_[0] = u + unitTexW_;
                buffer_[index].uv_[1] = v;
                ++index;

                buffer_[index].position_.set(ex, ey);
                buffer_[index].uv_[0] = u + unitTexW_;
                buffer_[index].uv_[1] = v + unitTexH_;
                ++index;

                ++count_;
            }

            ++str;

            sx += unitW_;
            ex += unitW_;
        }
    }

    //------------------------------------------
    //---
    //--- DebugPrimitive
    //---
    //------------------------------------------
    TextRenderer::TextRenderer()
        :batch_(NULL)
    {
    }

    TextRenderer::~TextRenderer()
    {
        LASSERT(batch_ == NULL);
    }

    void TextRenderer::initialize(u32 numMaxCharacters, u32 charW, u32 charH, u32 numRows, u32 numCols)
    {
        batch_ = LIME_NEW PrimitiveBatch(numMaxCharacters, charW, charH, numRows, numCols);
    }

    void TextRenderer::terminate()
    {
        LIME_DELETE(batch_);
    }

    void TextRenderer::setViewSize(u32 numRows, u32 numCols)
    {
        batch_->setViewSize(numRows, numCols);
    }

    void TextRenderer::draw()
    {
        LASSERT(batch_ != NULL);
        batch_->draw();
    }

    void TextRenderer::clear()
    {
        LASSERT(batch_ != NULL);
        batch_->clear();
    }

    void TextRenderer::print(s32 row, s32 col, const char* str)
    {
        LASSERT(batch_ != NULL);
        batch_->print(row, col, reinterpret_cast<const u8*>(str));
    }

    bool TextRenderer::setTextureFromFile(const char* path)
    {
        LASSERT(path != NULL);
        LASSERT(batch_ != NULL);

        lcore::ifstream file(path, lcore::ios::binary);
        if(false == file.is_open()){
            return false;
        }

        lgraphics::TextureRef texture;
        bool ret = lgraphics::io::IOPNG::read(file, texture);

        if(false == ret){
            return false;
        }

        batch_->material_.setTextureNum(1);
        batch_->material_.setTexture(0, texture);

#if defined(LIME_GL)
        //サンプラステートセット
        texture.attach();
        batch_->material_.getSamplerState(0).apply(0);
        texture.detach();
#endif

        SurfaceDesc desc;
        texture.getLevelDesc(0, desc);
        batch_->setTextureSize(desc.width_, desc.height_);
        return true;
    }

    bool TextRenderer::setTextureFromMemory(const s8* memory, u32 size)
    {
        LASSERT(memory != NULL);
        lcore::isstream is(memory, size);

        lgraphics::TextureRef texture;

        bool ret = lgraphics::io::IOPNG::read(is, texture);
        if(false == ret){
            return false;
        }

        batch_->material_.setTextureNum(1);
        batch_->material_.setTexture(0, texture);

#if defined(LIME_GLES2)
        texture.attach();
        batch_->material_.getSamplerState(0).apply(0);
        texture.detach();
#endif

        SurfaceDesc desc;
        texture.getLevelDesc(0, desc);
        batch_->setTextureSize(desc.width_, desc.height_);
        return true;
    }
}
