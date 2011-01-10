/**
@file PrimitiveRender.cpp
@author t-sakai
@date 2009/05/15
*/
#include <lmath/lmath.h>
#include "PrimitiveRender.h"
#include "../lgraphics.h"

#include "../api/GraphicsDeviceRef.h"
#include "../api/RenderStateRef.h"
#include "../api/TextureRef.h"
#include "../api/VertexBufferRef.h"
#include "../api/VertexDeclarationRef.h"
#include "../api/ShaderRef.h"

#include "../scene/ShaderManager.h"

namespace lgraphics
{
    namespace
    {
        // 頂点シェーダ
        static const char PrimitiveRenderVSSource[] =
        {
            "uniform float4x4 mvp;\n" //View * Projection Matrix
            "struct VSInput\n"
            "{\n"
            "float4 position : POSITION;\n"
            "float4 color : COLOR0;\n"
            "float2 uv : TEXCOORD0;\n"
            "};\n"
            "struct VSOutput\n"
            "{\n"
            "float4 position : POSITION0;\n"
            "float4 color : COLOR0;\n"
            "float2 uv : TEXCOORD0;\n"
            "};\n"
            "VSOutput main(VSInput input)\n"
            "{\n"
            "VSOutput output = (VSOutput)0;\n"
            "output.position = mul(input.position, mvp);\n"
            "output.color = input.color;\n"
            "output.uv = input.uv;\n"
            "return output;\n"
            "}"
        };

        static const u32 PrimitiveRenderVSSize = sizeof(PrimitiveRenderVSSource);

        // 頂点シェーダ
        static const char PrimitiveRenderPSSource[] =
        {
            "struct VSOutput\n"
            "{\n"
            "float4 position : POSITION0;\n"
            "float4 color : COLOR0;\n"
            "float2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 main(VSOutput input) : COLOR\n"
            "{\n"
            "return input.color;\n"
            "}"
        };

        static const u32 PrimitiveRenderPSSize = sizeof(PrimitiveRenderPSSource);
    }

    const u32 PRIMITIVE_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    const char* PrimitiveRender::VSName = "PrimitiveVS3D";
    const char* PrimitiveRender::PSName = "PrimitivePS3D";

    PrimitiveRender::PrimitiveRender(VertexBuffer* vb, u32 maxTriangle, Texture* texture)
        :vb_(vb),
        decl_(NULL),
        texture_(texture),
        state_(NULL),
        maxTriangle_(maxTriangle),
        numTriangle_(0),
        vs_(NULL),
        ps_(NULL),
        lockedBuffer_(NULL)
    {
        //頂点宣言
        VertexDeclCreator creator(3);
        u32 offset = 0;
        creator.add(0, offset, DeclType_Float3, DeclUsage_Position, 0);
        offset += sizeof(f32)*3;
        creator.add(0, offset, DeclType_Color, DeclUsage_Color, 0);
        offset += sizeof(u32);
        creator.add(0, offset, DeclType_Float2, DeclUsage_Texcoord, 0);

        decl_ = LIME_NEW VertexDeclaration;
        LASSERT(decl_!=NULL);
        creator.end(*decl_);

        //シェーダ準備
        ShaderManager *manager = ShaderManager::getInstance();
        vs_ = manager->findVS(VSName);
        if(vs_ == NULL){
            vs_ = LIME_NEW VertexShader;
            vs_->createFromMemory(PrimitiveRenderVSSource, PrimitiveRenderVSSize);
            manager->addVS(VSName, vs_);
        }

        viewProjHandle_ = vs_->getHandle("mvp");

        ps_ = manager->findPS(PSName);
        if(ps_ == NULL){
            ps_ = LIME_NEW PixelShader;
            ps_->createFromMemory(PrimitiveRenderPSSource, PrimitiveRenderPSSize);
            manager->addPS(PSName, ps_);
        }

        RenderState::begin();
        setState();
        state_ = LIME_NEW RenderState;
        LASSERT(state_ != NULL);
        RenderState::end(*state_);
    }

    PrimitiveRender::~PrimitiveRender()
    {
        LIME_DELETE(texture_);
    }

    PrimitiveRender* PrimitiveRender::create(u32 maxTriangle, Texture* texture)
    {
        VertexBuffer *vb = LIME_NEW VertexBuffer(sizeof(PrimitiveVertex), maxTriangle*3, Pool_Default, Usage_Dynamic);

        if(vb){
            PrimitiveRender *prim = LIME_NEW PrimitiveRender(vb, maxTriangle, texture);
            LASSERT(prim != NULL);
            return prim;
        };
        return NULL;
    }

    void PrimitiveRender::draw(const lmath::Matrix44& view, const lmath::Matrix44& proj)
    {
        LASSERT(lockedBuffer_ == NULL);

        state_->apply();

        lmath::Matrix44 vp = view;
        vp *= proj;
        vs_->setMatrix(viewProjHandle_, vp);

        GraphicsDevice &device = Graphics::getDevice();
        device.draw(Primitive_TriangleList, 0, numTriangle_);
    }

    void PrimitiveRender::drawNoStateChange()
    {
        LASSERT(lockedBuffer_ == NULL);
        //decl_->attach();
        vb_->attach();

        GraphicsDevice &device = Graphics::getDevice();
        device.draw(Primitive_TriangleList, 0, numTriangle_);
    }

    bool PrimitiveRender::add(const PrimitiveVertex& v1, const PrimitiveVertex& v2, const PrimitiveVertex& v3)
    {
        LASSERT(lockedBuffer_ != NULL);
        if(numTriangle_ >= maxTriangle_){
            return false;
        }

        lockedBuffer_[numTriangle_ * 3 + 0] = v1;
        lockedBuffer_[numTriangle_ * 3 + 1] = v2;
        lockedBuffer_[numTriangle_ * 3 + 2] = v3;

        ++numTriangle_;
        return true;
    }

    void PrimitiveRender::clear()
    {
        numTriangle_ = 0;
    }

    void PrimitiveRender::lock()
    {
        bool isLock = vb_->lock((void**)&lockedBuffer_, Lock_Discard);
        if(false == isLock){
            return;
        }

        clear();
    }

    void PrimitiveRender::unlock()
    {
        vb_->unlock();
        lockedBuffer_ = NULL;
    }

    void PrimitiveRender::setState()
    {
        LASSERT(vb_ != NULL);
        LASSERT(decl_ != NULL);

        RenderState::setZEnable(true);
        RenderState::setZWriteEnable(true);
        RenderState::setAlphaTest(true);

        RenderState::setAlphaBlendEnable(false);

        //RenderState::setAlphaBlendSrc(Blend_SrcAlpha);
        //RenderState::setAlphaBlendDst(Blend_InvSrcAlpha);

        //Disable Lighting
        RenderState::setLighting(false);

        RenderState::setCullMode(lgraphics::CullMode_None);


        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        vs_->attach();
        ps_->attach();
        decl_->attach();

        if(texture_ != NULL){
            d3ddevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

            d3ddevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            d3ddevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

            texture_->attach(0);
        }else{
            d3ddevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            d3ddevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        }

        vb_->attach();

        //lmath::Matrix44 mat;
        //mat.identity();
        //d3ddevice->SetTransform(D3DTS_WORLDMATRIX(0), (D3DMATRIX*)&mat);
    }
}
