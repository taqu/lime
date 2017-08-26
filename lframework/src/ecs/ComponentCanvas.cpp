/**
@file ComponentCanvas.cpp
@author t-sakai
@date 2017/05/02 create
*/
#include "ecs/ComponentCanvas.h"
#include <lmath/Vector4.h>
#include <lgraphics/Window.h>
#include <lgraphics/Graphics.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/ConstantBufferTableSet.h>

#include "System.h"

#include "render/Renderer.h"
#include "render/RenderQueue.h"
#include "render/Camera.h"

#include "resource/ShaderID.h"
#include "resource/Resources.h"
#include "resource/ResourceTexture2D.h"

#include "ecs/ECSManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentCanvasElement.h"

namespace lfw
{
namespace
{
    inline ComponentRendererManager* getManager()
    {
        return System::getECSManager().getComponentManager<ComponentRendererManager>();
    }
}

    ComponentCanvas::ComponentCanvas()
        :constantBufferTableSet_(NULL)
        ,pixelShader_(NULL)
        ,srv_(NULL)
        ,left_(0)
        ,top_(0)
        ,width_(0)
        ,height_(0)
        ,prevNumVertices_(0)
        ,prevNumIndices_(0)
        ,horizontalScale_(1.0f)
        ,verticalScale_(1.0f)
    {
        setLayer(Layer_Default2D);
        commands_.reserve(64);
    }

    ComponentCanvas::~ComponentCanvas()
    {
    }

    void ComponentCanvas::onCreate()
    {
        onDestroy();

        constantBufferTableSet_ = &System::getRenderer().getConstantBuffer();

        Resources& resources = System::getResources();
        inputLayout_ = resources.getInputLayoutFactory().get(lfw::InputLayout_Sprite2D);

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

        ShaderManager& shaderManager = resources.getShaderManager();
        vertexShader_ = shaderManager.getVS(ShaderVS_Sprite2D);
        defaultPixelShader_ = shaderManager.getPS(ShaderPS_Sprite2D);

        defaultSRV_ = resources.getEmptyTextureWhite()->cast<ResourceTexture2D>()->getShaderResourceView();
        requestUpdate();
    }

    void ComponentCanvas::onStart()
    {
        if(width_<=0 || height_<=0){
            s32 width,height;
            System::getWindow().getViewSize(width, height);
            setScreen(0, 0, width, height);
        }
        rebuild();
    }

    void ComponentCanvas::update()
    {
    }

    void ComponentCanvas::postUpdate()
    {
        if(checkFlag(Flag_UpdateCommands)){
            resetFlag(Flag_UpdateCommands);
            rebuild();
        }
    }

    void ComponentCanvas::onDestroy()
    {
        commands_.clear();
        canvasElements_.clear();
        indices_.clear();
        vertices_.clear();

        defaultSRV_.destroy();
        samLinear_.destroy();
        blendState_.destroy();
        inputLayout_.destroy();
        for(s32 i=0; i<2; ++i){
            ibs_[i].destroy();
            vbs_[i].destroy();
        }
    }

    void ComponentCanvas::drawDepth(RenderContext&)
    {
    }

    void ComponentCanvas::drawOpaque(RenderContext&)
    {
    }

    void ComponentCanvas::drawTransparent(RenderContext&)
    {
        lgfx::ContextRef& context = lgfx::getDevice();

        lgfx::Viewport viewport;
        context.getViewport(viewport);

        vbs_[bufferIndex_].attach(context, 0, sizeof(Vertex), 0);
        ibs_[bufferIndex_].attach(context, lgfx::Data_R16_UInt, 0);

        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);
        context.setBlendState(blendState_);
        context.setPSSamplers(0, 1, samLinear_);
        context.setVertexShader(vertexShader_);
        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setInputLayout(inputLayout_);
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DDisableWDisable);
        context.setVSConstantBuffers(0, 1, screen_);
        context.setViewport(left_, top_, width_, height_);

        for(s32 i=0; i<commands_.size(); ++i){
            Command& command = commands_[i];
            context.setPSResources(0, 1, &command.srv_);
            context.setPixelShader(command.pixelShader_);
            context.drawIndexed(command.numIndices_, command.indexStart_, 0);
        }

        context.setViewport(viewport);
        context.setInputLayout(NULL);
        context.clearVertexBuffers(0, 1);
        context.clearIndexBuffers();
        context.setVertexShader(NULL);
        context.setPixelShader(NULL);
        context.clearPSResources(0, 1);
        context.clearPSSamplers(0, 1);
    }

    bool ComponentCanvas::addQueue(RenderQueue& queue)
    {
        if(vertices_.size()<=0){
            return false;
        }
        const lmath::Vector4& position = this->getEntity().getGeometric()->getPosition();
        //f32 depth = lmath::manhattanDistance3(queue.getCamera().getEyePosition(), position);
        f32 depth = position.z_;
        queue.add(RenderPath_UI, depth, this);
        return true;
    }

    void ComponentCanvas::setScreen(s32 left, s32 top, s32 width, s32 height)
    {
        left_ = left;
        top_ = top;
        width_ = width;
        height_ = height;
        f32 invWidth = 1.0f/width_;
        f32 invHeight = 1.0f/height_;
        Screen screen;
        screen.invHalfWidth_ = 2.0f*invWidth;
        screen.invHalfHeight_ = -2.0f*invHeight;
        screen.left_ = static_cast<f32>(left);
        screen.top_ = static_cast<f32>(top);

        screen_ = lgfx::ConstantBuffer::create(
            sizeof(Screen),
            lgfx::Usage_Immutable,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            0,
            &screen,
            sizeof(Screen));
    }

    void ComponentCanvas::setScale(f32 horizontal, f32 vertical)
    {
        horizontalScale_ = horizontal;
        verticalScale_ = vertical;
    }

    void ComponentCanvas::add(ComponentCanvasElement* element)
    {
        for(s32 i=0; i<canvasElements_.size(); ++i){
            if(canvasElements_[i] == element){
                return;
            }
        }
        canvasElements_.push_back(element);
        requestUpdate();
    }

    void ComponentCanvas::remove(ComponentCanvasElement* element)
    {
        for(s32 i=0; i<canvasElements_.size(); ++i){
            if(canvasElements_[i] == element){
                canvasElements_.removeAt(i);
                requestUpdate();
                return;
            }
        }
    }

    void ComponentCanvas::requestUpdate()
    {
        setFlag(Flag_UpdateCommands);
    }

    void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const lmath::Vector4& texcoord, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        u16 uv[4];
        uv[0] = lcore::toBinary16Float(texcoord.x_);
        uv[1] = lcore::toBinary16Float(texcoord.y_);
        uv[2] = lcore::toBinary16Float(texcoord.z_);
        uv[3] = lcore::toBinary16Float(texcoord.w_);
        addRect(rect, abgr, uv, srv, ps);
    }

    void ComponentCanvas::addRect(const lmath::Vector4& rect, u32 abgr, const u16 uv[4], ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        LASSERT((vertices_.size() + 4)<=0xFFFFU);
        LASSERT((indices_.size() + 6)<=0xFFFFU);

        if(prevNumVertices_<vertices_.size() && (srv_ != srv || pixelShader_ != ps)){
            pushCommand();
            srv_ = srv;
            pixelShader_ = ps;
        }

        lmath::lm128 scale = _mm_set_ps(horizontalScale_, verticalScale_, horizontalScale_, verticalScale_);
        lmath::lm128 r = _mm_mul_ps(_mm_loadu_ps(&rect.x_), scale);
        lmath::Vector4 trect;
        _mm_storeu_ps(&trect.x_, r);

        srv_ = srv;
        pixelShader_ = ps;
        u16 numVertices = vertices_.size();
        vertices_.expand(4);
        Vertex* vertices = vertices_.begin() + numVertices;
        vertices[0].position_[0] = trect.x_;
        vertices[0].position_[1] = trect.y_;
        vertices[0].abgr_ = abgr;
        vertices[0].texcoord_[0] = uv[0];
        vertices[0].texcoord_[1] = uv[1];

        vertices[1].position_[0] = trect.z_;
        vertices[1].position_[1] = trect.y_;
        vertices[1].abgr_ = abgr;
        vertices[1].texcoord_[0] = uv[2];
        vertices[1].texcoord_[1] = uv[1];

        vertices[2].position_[0] = trect.x_;
        vertices[2].position_[1] = trect.w_;
        vertices[2].abgr_ = abgr;
        vertices[2].texcoord_[0] = uv[0];
        vertices[2].texcoord_[1] = uv[3];

        vertices[3].position_[0] = trect.z_;
        vertices[3].position_[1] = trect.w_;
        vertices[3].abgr_ = abgr;
        vertices[3].texcoord_[0] = uv[2];
        vertices[3].texcoord_[1] = uv[3];

        u16 numIndices = indices_.size();
        indices_.expand(6);
        u16* indices = indices_.begin() + numIndices;
        indices[0] = numVertices + 0;
        indices[1] = numVertices + 1;
        indices[2] = numVertices + 2;
        indices[3] = indices[2];
        indices[4] = indices[1];
        indices[5] = numVertices + 3;
    }

    void ComponentCanvas::addTriangle(s32 numTriangles, const Vertex* vertices, ID3D11ShaderResourceView* srv, ID3D11PixelShader* ps)
    {
        LASSERT(0<=numTriangles);
        LASSERT(NULL != vertices);

        s32 n = numTriangles*3;

        LASSERT((vertices_.size() + n)<=0xFFFFU);
        LASSERT((indices_.size() + n)<=0xFFFFU);


        if(prevNumVertices_<vertices_.size() && (srv_ != srv || pixelShader_ != ps)){
            pushCommand();
            srv_ = srv;
            pixelShader_ = ps;
        }

        u16 numVertices = vertices_.size();
        vertices_.expand(static_cast<u16>(n));
        f32* dst = reinterpret_cast<f32*>(vertices_.begin() + numVertices);
        const f32* src = reinterpret_cast<const f32*>(vertices);

        u16 numIndices = indices_.size();
        indices_.expand(static_cast<u16>(n));
        u16* indices = indices_.begin() + numIndices;

        u16 index = numVertices;
        for(s32 i=0; i<n; ++i){
            _mm_store_ps(dst, _mm_loadu_ps(src));
            dst[0] *= horizontalScale_;
            dst[1] *= verticalScale_;
            indices[i] = index;
            dst += 4;
            src += 4;
            ++index;
        }
    }

    void ComponentCanvas::pushCommand()
    {
        Command command;
        command.vertexStart_ = prevNumVertices_;
        command.numVertices_ = vertices_.size() - prevNumVertices_;
        command.indexStart_ = prevNumIndices_;
        command.numIndices_ = indices_.size() - prevNumIndices_;
        command.srv_ = srv_;
        command.pixelShader_ = pixelShader_;
        commands_.push_back(command);

        prevNumVertices_ = vertices_.size();
        prevNumIndices_ = indices_.size();
    }

    void ComponentCanvas::rebuild()
    {
        u16 capacityVertices = vertices_.capacity();
        u16 capacityIndices = indices_.capacity();

        srv_ = defaultSRV_;
        pixelShader_ = defaultPixelShader_;

        prevNumVertices_ = 0;
        prevNumIndices_ = 0;
        vertices_.clear();
        indices_.clear();
        commands_.clear();

        for(s32 i=0; i<canvasElements_.size(); ++i){
            canvasElements_[i]->updateMesh(*this);
        }

        if(prevNumVertices_<vertices_.size()){
            pushCommand();
        }

        u32 size;
        if(capacityVertices<vertices_.capacity()){
            size = sizeof(Vertex) * vertices_.capacity();
            for(s32 i=0; i<2; ++i){
                vbs_[i] = lgfx::VertexBuffer::create(
                    size,
                    lgfx::Usage_Dynamic,
                    lgfx::CPUAccessFlag_Write,
                    lgfx::ResourceMisc_None);
            }
        }
        if(capacityIndices<indices_.capacity()){
            size = sizeof(u16) * indices_.capacity();
            for(s32 i=0; i<2; ++i){
                ibs_[i] = lgfx::IndexBuffer::create(
                    size,
                    lgfx::Usage_Dynamic,
                    lgfx::CPUAccessFlag_Write,
                    lgfx::ResourceMisc_None);
            }
        }
        if(vertices_.size()<=0){
            return;
        }

        //バッファ更新
        lgfx::ContextRef& context = lgfx::getDevice();

        bufferIndex_ = (bufferIndex_+1) & 0x01U;
        size = vertices_.size()*sizeof(Vertex);
        lgfx::MappedSubresource mapped;
        if(vbs_[bufferIndex_].map(context, 0, lgfx::MapType_WriteDiscard, mapped)){
            f32* dst = mapped.getData<f32>();
            const f32* src = reinterpret_cast<const f32*>(vertices_.begin());
            const f32* end = reinterpret_cast<const f32*>(vertices_.begin()+vertices_.size());
            while(src<end){
                _mm_store_ps(dst, _mm_load_ps(src));
                dst += 4;
                src += 4;
            }
            vbs_[bufferIndex_].unmap(context, 0);
        }

        size = indices_.size()*sizeof(u16);
        if(ibs_[bufferIndex_].map(context, 0, lgfx::MapType_WriteDiscard, mapped)){
            u16* dst = mapped.getData<u16>();
            const u16* src = indices_.begin();
            lcore::memcpy(dst, src, size);
            ibs_[bufferIndex_].unmap(context, 0);
        }
    }

    lgfx::ConstantBufferRef* ComponentCanvas::createConstantBuffer(u32 size, const void* data)
    {
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return NULL;
        }

        lgfx::GraphicsDeviceRef& context = lgfx::getDevice();
        lgfx::MappedSubresource mapped;
        if(constant->map(context, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(context, 0);
            return constant;
        }else{
            return NULL;
        }
    }

    bool ComponentCanvas::setConstantBuffer(Shader shader, s32 index, lgfx::ConstantBufferRef* constant)
    {
        lgfx::GraphicsDeviceRef& context = lgfx::getDevice();
        switch(shader)
        {
        case Shader_PS:
            context.setPSConstantBuffers(index, 1, (*constant));
            break;
        default:
            return false;
        }
        return true;
    }

    bool ComponentCanvas::setConstant(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::GraphicsDeviceRef& context = lgfx::getDevice();
        lgfx::MappedSubresource mapped;
        if(constant->map(context, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDst16(mapped.data_, data, size);
            constant->unmap(context, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }

    bool ComponentCanvas::setConstantAligned16(Shader shader, s32 index, u32 size, const void* data)
    {
        LASSERT(NULL != constantBufferTableSet_);
        LASSERT(NULL != data);

        lgfx::ConstantBufferRef* constant = constantBufferTableSet_->allocate(size);
        if(NULL == constant){
            return false;
        }

        lgfx::GraphicsDeviceRef& context = lgfx::getDevice();
        lgfx::MappedSubresource mapped;
        if(constant->map(context, 0, lgfx::MapType_WriteDiscard, mapped)){
            copyAlignedDstAlignedSrc16(mapped.data_, data, size);
            constant->unmap(context, 0);
            return setConstantBuffer(shader, index, constant);
        }else{
            return false;
        }
    }
}
