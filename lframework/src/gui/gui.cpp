/**
@file gui.cpp
@author t-sakai
@date 2017/01/14 create
*/
#include "gui/gui.h"
#include "gui/imgui.h"
#include <Windows.h>
#include <lgraphics/Window.h>
#include <lgraphics/Graphics.h>
#include <lgraphics/GraphicsDeviceRef.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/SamplerStateRef.h>
#include <lgraphics/BlendStateRef.h>
#include <lgraphics/DepthStencilStateRef.h>
#include <lgraphics/RasterizerStateRef.h>
#include <lgraphics/ConstantBufferRef.h>
#include <lgraphics/TextureRef.h>

#include "System.h"
#include "Timer.h"
#include "resource/InputLayoutFactory.h"
#include "resource/ShaderID.h"
#include "resource/ShaderManager.h"
#include "resource/Resources.h"

#include "input/Keyboard.h"
#include "input/Mouse.h"

ImGuiContext* GImGuiContext = NULL;

namespace lfw
{
    class GUI::Resource
    {
    public:
        Resource();
        ~Resource();

        void initialize();
        void terminate();
        void render(ImDrawData* drawData);

        s32 numVertices_;
        s32 numIndices_;

        ID3D11Buffer* vb_[2];
        ID3D11Buffer* ib_[2];
        lgfx::ConstantBufferRef constants_;
        lgfx::ShaderResourceViewRef srv_;

        lgfx::InputLayoutRef inputLayout_;
        lgfx::VertexShaderRef vs_;
        lgfx::PixelShaderRef ps_;
        lgfx::SamplerStateRef sampler_;
        lgfx::BlendStateRef blendState_;
        lgfx::DepthStencilStateRef depthStencilState_;
        lgfx::RasterizerStateRef rasterizerState_;
    };


    GUI::Resource::Resource()
        :numVertices_(0)
        ,numIndices_(0)
    {
        for(s32 i=0; i<2; ++i){
            vb_[i] = NULL;
            ib_[i] = NULL;
        }
    }

    GUI::Resource::~Resource()
    {
        terminate();
    }

    void GUI::Resource::initialize()
    {
        Resources& resources = System::getResources();
        inputLayout_ = resources.getInputLayoutFactory().get(lfw::InputLayout_UI);

        ShaderManager& shaderManager = resources.getShaderManager();
        vs_ = shaderManager.getVS(ShaderVS_UI);
        ps_ = shaderManager.getPS(ShaderPS_UI);

        sampler_ = lgfx::SamplerState::create(
            lgfx::TexFilter_MinMagMipLinear,
            lgfx::TexAddress_Wrap,
            lgfx::Cmp_Always,
            0.0f);

        blendState_ = lgfx::BlendState::create(
            FALSE,
            TRUE,
            lgfx::Blend_SrcAlpha,
            lgfx::Blend_InvSrcAlpha,
            lgfx::BlendOp_Add,
            lgfx::Blend_InvSrcAlpha,
            lgfx::Blend_Zero,
            lgfx::BlendOp_Add,
            lgfx::ColorWrite_All);

        {
            lgfx::StencilOPDesc desc;
            desc.failOp_ = desc.depthFailOp_ = desc.passOp_ = lgfx::StencilOp_Keep;
            desc.cmpFunc_ = lgfx::Cmp_Always;
            depthStencilState_ = lgfx::DepthStencilState::create(
                FALSE,
                lgfx::DepthWrite_Zero,
                lgfx::Cmp_Always,
                FALSE,
                lgfx::StencilOp_Keep,
                lgfx::StencilOp_Keep,
                desc,
                desc);
        }

        {
            rasterizerState_ = lgfx::RasterizerState::create(
                lgfx::Fill_Solid,
                lgfx::Cull_None,
                false,
                0, 0.0f, 0.0f,
                true, true, false, false);
        }

        ImGuiIO& io = ImGui::GetIO();

        {
            f32 L = 0.0f;
            f32 R = io.DisplaySize.x;
            f32 B = io.DisplaySize.y;
            f32 T = 0.0f;
            f32 mvp[4][4] =
            {
                { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
                { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
                { 0.0f,         0.0f,           0.5f,       0.0f },
                { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
            };
            constants_ = lgfx::ConstantBuffer::create(
                sizeof(f32)*16,
                lgfx::Usage_Immutable,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                0, mvp, sizeof(f32)*16);
        }

        {
            u8* pixels = NULL;
            s32 width = 0, height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            lgfx::SubResourceData data;
            data.mem_ = pixels;
            data.pitch_ = width<<2;
            data.slicePitch_ = 0;
            lgfx::Texture2DRef texture = lgfx::Texture::create2D(
                width, height,
                1, 1,
                lgfx::Data_R8G8B8A8_UNorm,
                lgfx::Usage_Default,
                lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                &data);
            lgfx::SRVDesc srvDesc;
            srvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
            srvDesc.dimension_ = lgfx::SRVDimension_Texture2D;
            srvDesc.tex2D_.mipLevels_ = 1;
            srvDesc.tex2D_.mostDetailedMip_ = 0;
            srv_ = texture.createSRView(srvDesc);
            io.Fonts->TexID = (void *)srv_.get();
        }
    }

    void GUI::Resource::terminate()
    {
        for(s32 i=0; i<2; ++i){
            LDXSAFE_RELEASE(vb_[i]);
            LDXSAFE_RELEASE(ib_[i]);
        }
        numVertices_ = 0;
        numIndices_ = 0;
    }

    void GUI::Resource::render(ImDrawData* drawData)
    {
        lgfx::GraphicsDeviceRef& device = lgfx::getDevice();

        if(numVertices_<drawData->TotalVtxCount){
            for(s32 i=0; i<2; ++i){
                LDXSAFE_RELEASE(vb_[i]);
            }
            while(numVertices_<drawData->TotalVtxCount){
                numVertices_ += 1024;
            }
            D3D11_BUFFER_DESC desc;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = numVertices_ * sizeof(ImDrawVert);
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            desc.StructureByteStride = 0;
            for(s32 i=0; i<2; ++i){
                device.getD3DDevice()->CreateBuffer(&desc, NULL, &vb_[i]);
            } 
        }else{
            lcore::swap(vb_[0], vb_[1]);
        }

        if(numIndices_<drawData->TotalIdxCount){
            for(s32 i=0; i<2; ++i){
                LDXSAFE_RELEASE(ib_[i]);
            }
            while(numIndices_<drawData->TotalIdxCount){
                numIndices_ += 1024;
            }
            D3D11_BUFFER_DESC desc;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = numIndices_ * sizeof(ImDrawIdx);
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            desc.StructureByteStride = 0;
            for(s32 i=0; i<2; ++i){
                device.getD3DDevice()->CreateBuffer(&desc, NULL, &ib_[i]);
            } 
        }else{
            lcore::swap(ib_[0], ib_[1]);
        }

        D3D11_MAPPED_SUBRESOURCE mappedVB;
        if(NULL == vb_[0] || FAILED(device.getContext()->Map(vb_[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB))){
            return;
        }
        D3D11_MAPPED_SUBRESOURCE mappedIB;
        if(NULL == ib_[0] || FAILED(device.getContext()->Map(ib_[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB))){
            device.getContext()->Unmap(vb_[0], 0);
            return;
        }

        ImDrawVert* dstVertices = (ImDrawVert*)mappedVB.pData;
        ImDrawIdx* dstIndices = (ImDrawIdx*)mappedIB.pData;
        for(s32 i=0; i<drawData->CmdListsCount; ++i){
            const ImDrawList* drawList = drawData->CmdLists[i];
            lcore::memcpy(dstVertices, drawList->VtxBuffer.Data, drawList->VtxBuffer.Size * sizeof(ImDrawVert));
            lcore::memcpy(dstIndices, drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof(ImDrawIdx));
            dstVertices += drawList->VtxBuffer.Size;
            dstIndices += drawList->IdxBuffer.Size;
        }

        device.getContext()->Unmap(vb_[0], 0);
        device.getContext()->Unmap(ib_[0], 0);

        ImGuiIO& io = ImGui::GetIO();

        //Constants
        device.setVSConstantBuffers(0, 1, constants_);

        //Viewport
        device.setViewport(0, 0, static_cast<s32>(io.DisplaySize.x), static_cast<s32>(io.DisplaySize.y));

        //Shaders and Buffers
        device.setInputLayout(inputLayout_);
        device.setVertexBuffer(0, vb_[0], sizeof(ImDrawVert), 0);
        device.setIndexBuffer(ib_[0], lgfx::Data_R16_UInt, 0);
        device.setVertexShader(vs_);
        device.setPixelShader(ps_);
        device.setPSSamplers(0, 1, sampler_);
        device.setPrimitiveTopology(lgfx::Primitive_TriangleList);

        //Render State
        const f32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
        device.getContext()->OMSetBlendState(blendState_, blend_factor, 0xffffffffU);
        //device.setBlendState(blendState_);
        device.setDepthStencilState(depthStencilState_, 0);
        device.setRasterizerState(rasterizerState_);

        //Render
        s32 vbOffset = 0;
        s32 ibOffset = 0;
        for(s32 i=0; i<drawData->CmdListsCount; ++i){
            const ImDrawList* drawList = drawData->CmdLists[i];
            for(s32 j=0; j<drawList->CmdBuffer.Size; ++j){
                const ImDrawCmd* drawCommand = &drawList->CmdBuffer[j];
                if(drawCommand->UserCallback){
                    drawCommand->UserCallback(drawList, drawCommand);
                }else{
                    const D3D11_RECT r = { (LONG)drawCommand->ClipRect.x, (LONG)drawCommand->ClipRect.y, (LONG)drawCommand->ClipRect.z, (LONG)drawCommand->ClipRect.w };
                    device.getContext()->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&drawCommand->TextureId);
                    device.getContext()->RSSetScissorRects(1, &r);
                    device.getContext()->DrawIndexed(drawCommand->ElemCount, ibOffset, vbOffset);
                }
                ibOffset += drawCommand->ElemCount;
            }
            vbOffset += drawList->VtxBuffer.Size;
        }

        //Clear
        device.getContext()->RSSetScissorRects(0, NULL);
        device.restoreDefaultViewport();

        device.setInputLayout(NULL);
        device.clearVertexBuffers(0,1);
        device.clearIndexBuffers();

        ID3D11Buffer* constants[] = {NULL};
        device.setVSConstantBuffers(0, 1, constants);

        device.setVertexShader(NULL);

        device.clearPSResources(0, 1);
        device.clearPSSamplers(0, 1);
        device.setPixelShader(NULL);
    }

    GUI* GUI::instance_ = NULL;

    bool GUI::initialize()
    {
        if(NULL == GImGuiContext){
            GImGuiContext = ImGui::CreateContext(lcore_malloc, lcore_free);
            ImGui::SetCurrentContext(GImGuiContext);
        }
        LDELETE(instance_);
        instance_ = LNEW GUI;
        return instance_->init();
    }

    void GUI::terminate()
    {
        LDELETE(instance_);
        if(NULL != GImGuiContext){
            ImGui::DestroyContext(GImGuiContext);
            GImGuiContext = NULL;
        }
    }

    GUI::GUI()
    {
        resource_ = LNEW Resource();
    }

    GUI::~GUI()
    {
        LDELETE(resource_);
        ImGui::Shutdown();
    }

    bool GUI::init()
    {
        ImGuiIO& io = ImGui::GetIO();
        lgfx::Window::Vector2 size = System::getWindow().getViewSize();
        io.DisplaySize.x = static_cast<f32>(size.x_);
        io.DisplaySize.y = static_cast<f32>(size.y_);

        io.KeyMap[ImGuiKey_Tab] = VK_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
        io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
        io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
        io.KeyMap[ImGuiKey_Home] = VK_HOME;
        io.KeyMap[ImGuiKey_End] = VK_END;
        io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
        io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
        io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';

        io.Fonts->AddFontDefault();

        io.RenderDrawListsFn = NULL;
        io.ImeWindowHandle = NULL;

        resource_->initialize();

        return true;
    }

    void GUI::begin()
    {
        linput::Input& input = System::getInput();

        ImGuiIO& io = ImGui::GetIO();

        io.DeltaTime = System::getTimer().getDeltaTime();

        const linput::Keyboard* keyboard = input.getKeyboard();
        if(NULL != keyboard){
            io.KeyCtrl = keyboard->isOn(linput::Key_LCONTROL);
            io.KeyShift = keyboard->isOn(linput::Key_LSHIFT);
            io.KeyAlt = keyboard->isOn(linput::Key_LMENU);
            io.KeySuper = false;
            for(s32 i=0; i<256; ++i){
                io.KeysDown[i] = keyboard->isOn((linput::KeyCode)i);
            }
        }
        const linput::Mouse* mouse = input.getMouse();
        if(NULL != mouse){
            io.MouseDown[0] = mouse->isOn(linput::MouseButton_0);
            s32 d = mouse->getDuration(linput::MouseAxis_Z);
            if(0!=d){
                io.MouseWheel += (0<d)? 1.0f:-1.0f;
            }

            io.MousePos.x = static_cast<f32>(mouse->getX());
            io.MousePos.y = static_cast<f32>(mouse->getY());
        }

        ImGui::NewFrame();
    }

    void GUI::render()
    {
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        if(NULL == drawData){
            return;
        }
        resource_->render(drawData);
    }
}
