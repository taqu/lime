/**
@file RenderContext2D.cpp
@author t-sakai
@date 2016/12/21 create
*/
#include "render/RenderContext2D.h"
#include <lgraphics/GraphicsDeviceRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/ConstantBufferTableSet.h>
#include <lgraphics/FrameSyncQuery.h>
#include <lmath/Vector4.h>
#include "resource/ShaderID.h"
#include "resource/Resources.h"
#include "resource/ResourceTexture2D.h"

namespace lfw
{
    RenderContext2D::RenderContext2D()
        :context_(NULL)
        ,constantBufferTableSet_(NULL)
        ,pixelShader_(NULL)
        ,srv_(NULL)
        ,left_(0)
        ,top_(0)
        ,screenWidth_(0)
        ,screenHeight_(0)
        ,maxVerticesPerFrame_(0)
        ,maxIndicesPerFrame_(0)
        ,prevNumVertices_(0)
        ,numVertices_(0)
        ,prevNumIndices_(0)
        ,numIndices_(0)
        ,vertices_(NULL)
        ,indices_(NULL)
    {
        commands_.reserve(64);
    }

    RenderContext2D::~RenderContext2D()
    {
        terminate();
    }

    void RenderContext2D::initialize(
        lgfx::ContextRef* context,
        lgfx::ConstantBufferTableSet* constantBufferTableSet,
        lgfx::FrameSyncQuery* frameSyncQuery,
        s32 left,
        s32 top,
        s32 screenWidth,
        s32 screenHeight,
        u16 maxVerticesPerFrame,
        u16 maxIndicesPerFrame)
    {
        LASSERT(0<maxVerticesPerFrame);
        LASSERT(0<maxIndicesPerFrame);

        terminate();
        context_ = context;
        constantBufferTableSet_ = constantBufferTableSet;

        resize(left, top, screenWidth, screenHeight);
        maxVerticesPerFrame_ = static_cast<u16>(maxVerticesPerFrame);
        maxIndicesPerFrame_ = maxIndicesPerFrame;

        s32 totalVBSize = maxVerticesPerFrame_ * sizeof(Vertex) * frameSyncQuery->getFrames();
        dynamicVertexBuffer_.initialize(frameSyncQuery, totalVBSize);

        s32 totalIBSize = maxIndicesPerFrame_ * sizeof(u16) * frameSyncQuery->getFrames();
        dynamicIndexBuffer_.initialize(frameSyncQuery, totalIBSize);

        inputLayout_ = Resources::getInstance().getInputLayoutFactory().get(lfw::InputLayout_Sprite2D);

        blendState_ = lgfx::BlendState::create(
            FALSE,
            TRUE,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_InvSrcAlpha,
            lgfx::BlendOp_Add,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_DestAlpha,
            lgfx::BlendOp_Add,
            lgfx::ColorWrite_All);

        samLinear_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Clamp,
            lgfx::Cmp_Always,
            0.0f);

        ShaderManager& shaderManager = Resources::getInstance().getShaderManager();
        vertexShader_ = shaderManager.getVS(ShaderVS_Sprite2D);
        defaultPixelShader_ = shaderManager.getPS(ShaderPS_Sprite2D);

        defaultSRV_ = Resources::getInstance().getEmptyTextureWhite()->cast<ResourceTexture2D>()->getShaderResourceView();

        vertices_ = (Vertex*)LALIGNED_MALLOC(maxVerticesPerFrame_ * sizeof(Vertex), lcore::SSE_ALIGN);
        indices_ = (u16*)LALIGNED_MALLOC(maxIndicesPerFrame_ * sizeof(u16), lcore::SSE_ALIGN);
    }

    void RenderContext2D::terminate()
    {
        LALIGNED_FREE(indices_, lcore::SSE_ALIGN);
        LALIGNED_FREE(vertices_, lcore::SSE_ALIGN);
        defaultSRV_.destroy();
        samLinear_.destroy();
        blendState_.destroy();
        inputLayout_.destroy();
        dynamicVertexBuffer_.terminate();
    }

    void RenderContext2D::resize(s32 left, s32 top, s32 screenWidth, s32 screenHeight)
    {
        left_ = left;
        top_ = top;
        screenWidth_ = screenWidth;
        screenHeight_ = screenHeight;
        Constant2D constant2D;
        constant2D.invScreenWidth2_ = 2.0f/screenWidth;
        constant2D.invScreenHeight2_ = -2.0f/screenHeight;
        constant2D.left_ = static_cast<f32>(left);
        constant2D.top_ = static_cast<f32>(top);

        constant2D_ = lgfx::ConstantBuffer::create(
            sizeof(Constant2D),
            lgfx::Usage_Immutable,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            &constant2D,
            sizeof(Constant2D));
    }

    void RenderContext2D::begin()
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);

        dynamicVertexBuffer_.begin();
        dynamicIndexBuffer_.begin();

        srv_ = defaultSRV_;
        pixelShader_ = defaultPixelShader_;

        prevNumVertices_ = numVertices_ = 0;
        prevNumIndices_ = numIndices_ = 0;

        commands_.clear();
    }

    void RenderContext2D::end()
    {
        if(prevNumVertices_<numVertices_){
            pushCommand();
        }

        lgfx::ContextRef& context = *context_;

        s32 size = numVertices_*sizeof(Vertex);
        lgfx::MappedSubresourceTransientBuffer mapped;
        if(dynamicVertexBuffer_.map(context, 0, size, mapped)){
            f32* dst = mapped.getData<f32>();
            const f32* src = reinterpret_cast<const f32*>(vertices_);
            const f32* end = reinterpret_cast<const f32*>(vertices_+numVertices_);
            while(src<end){
                _mm_store_ps(dst, _mm_load_ps(src));
                dst += 4;
                src += 4;
            }
            dynamicVertexBuffer_.unmap(context, 0);

            dynamicVertexBuffer_.attach(context, 0, sizeof(Vertex), mapped.offsetInBytes_);
        }
        size = numIndices_*sizeof(u16);
        if(dynamicIndexBuffer_.map(context, 0, size, mapped)){
            u16* dst = mapped.getData<u16>();
            const u16* src = indices_;
            lcore::memcpy(dst, src, size);
            dynamicIndexBuffer_.unmap(context, 0);
            dynamicIndexBuffer_.attach(context, lgfx::Data_R16_UInt, mapped.offsetInBytes_);
        }

        context_->setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);
        context_->setBlendState(blendState_);
        context_->setPSSamplers(0, 1, samLinear_);
        context_->setVertexShader(vertexShader_);
        context_->setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context_->setInputLayout(inputLayout_);
        context_->setDepthStencilState(lgfx::ContextRef::DepthStencil_DDisableWDisable);
        context_->setVSConstantBuffers(0, 1, constant2D_);

        for(s32 i=0; i<commands_.size(); ++i){
            Command& command = commands_[i];
            context_->setPSResources(0, 1, &command.srv_);
            context_->setPixelShader(command.pixelShader_);
            context_->drawIndexed(command.numIndices_, command.indexStart_, 0);
        }

        dynamicVertexBuffer_.end();
        dynamicIndexBuffer_.end();

        context_->setInputLayout(NULL);
        context_->clearVertexBuffers(0, 1);
        context_->clearIndexBuffers();
        context_->setVertexShader(NULL);
        context_->setPixelShader(NULL);
        context_->clearPSResources(1);
        context_->clearPSSamplers(1);
    }

    lgfx::ConstantBufferRef* RenderContext2D::createConstantBuffer(u32 size)
    {
        return constantBufferTableSet_->allocate(size);
    }

    lgfx::ConstantBufferRef* RenderContext2D::createConstantBuffer(u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return NULL;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return constant;
        }else{
            return NULL;
        }
    }

    inline bool RenderContext2D::setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant)
    {
        switch(shader)
        {
        case Shader_PS:
            context_->setPSConstantBuffers(index, 1, (*constant));
            break;
        default:
            return false;
        }
        return true;
    }

    bool RenderContext2D::setConstant(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool RenderContext2D::setConstantAligned16(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != context_);
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::MappedSubresource mapped;
        if(constant->map(*context_, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDstAlignedSrc16(mapped.data_, data, size);
            constant->unmap(*context_, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    void RenderContext2D::addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        u16 uv[4];
        uv[0] = lcore::toBinary16Float(texcoord.x_);
        uv[1] = lcore::toBinary16Float(texcoord.y_);
        uv[2] = lcore::toBinary16Float(texcoord.z_);
        uv[3] = lcore::toBinary16Float(texcoord.w_);
        addRect(rect, abgr, uv, srv, ps);
    }

    void RenderContext2D::addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        u16 numVertices = numVertices_ + 4;
        if(maxVerticesPerFrame_<numVertices){
            lcore::Log("RenderContext2D::addRect over max vertices");
            return;
        }
        u16 numIndices = numIndices_ + 6;
        if(maxIndicesPerFrame_<numIndices){
            lcore::Log("RenderContext2D::addRect over max indices");
            return;
        }
        if(prevNumVertices_<numVertices_ && (srv_ != srv || pixelShader_ != ps)){
            pushCommand();
        }
        srv_ = srv;
        pixelShader_ = ps;
        Vertex* vertices = vertices_ + numVertices_;
        vertices[0].position_[0] = rect.x_;
        vertices[0].position_[1] = rect.y_;
        vertices[0].abgr_ = abgr;
        vertices[0].texcoord_[0] = uv[0];
        vertices[0].texcoord_[1] = uv[1];

        vertices[1].position_[0] = rect.z_;
        vertices[1].position_[1] = rect.y_;
        vertices[1].abgr_ = abgr;
        vertices[1].texcoord_[0] = uv[2];
        vertices[1].texcoord_[1] = uv[1];

        vertices[2].position_[0] = rect.x_;
        vertices[2].position_[1] = rect.w_;
        vertices[2].abgr_ = abgr;
        vertices[2].texcoord_[0] = uv[0];
        vertices[2].texcoord_[1] = uv[3];

        vertices[3].position_[0] = rect.z_;
        vertices[3].position_[1] = rect.w_;
        vertices[3].abgr_ = abgr;
        vertices[3].texcoord_[0] = uv[2];
        vertices[3].texcoord_[1] = uv[3];

        u16* indices = indices_ + numIndices_;
        indices[0] = numVertices_ + 0;
        indices[1] = numVertices_ + 1;
        indices[2] = numVertices_ + 2;
        indices[3] = indices[2];
        indices[4] = indices[1];
        indices[5] = numVertices_ + 3;

        numVertices_ = numVertices;
        numIndices_ = numIndices;
    }

    void RenderContext2D::addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        LASSERT(0<=numTriangles);
        LASSERT(NULL != vertices);

        u16 n = static_cast<u16>(numTriangles*3);

        u16 numVertices = numVertices_ + n;
        if(maxVerticesPerFrame_<numVertices){
            lcore::Log("RenderContext2D::addRect over max vertices");
            return;
        }
        u16 numIndices = numIndices_ + n;
        if(maxIndicesPerFrame_<numIndices){
            lcore::Log("RenderContext2D::addRect over max indices");
            return;
        }

        if(prevNumVertices_<numVertices_ && (srv_ != srv || pixelShader_ != ps)){
            pushCommand();
            srv_ = srv;
            pixelShader_ = ps;
        }

        f32* dst = reinterpret_cast<f32*>(vertices_+numVertices_);
        const f32* src = reinterpret_cast<const f32*>(vertices);
        u16* indices = reinterpret_cast<u16*>(indices_+numIndices_);
        u16 index = numVertices_;
        for(s32 i=0; i<n; ++i){
            _mm_store_ps(dst, _mm_loadu_ps(src));
            indices[i] = index;
            dst += 4;
            src += 4;
            ++index;
        }
        numVertices_ = numVertices;
        numIndices_ = numIndices;
    }

    void RenderContext2D::pushCommand()
    {
        Command command;
        command.vertexStart_ = prevNumVertices_;
        command.numVertices_ = numVertices_ - prevNumVertices_;
        command.indexStart_ = prevNumIndices_;
        command.numIndices_ = numIndices_ - prevNumIndices_;
        command.srv_ = srv_;
        command.pixelShader_ = pixelShader_;
        commands_.push_back(command);

        prevNumVertices_ = numVertices_;
        prevNumIndices_ = numIndices_;
    }
}
