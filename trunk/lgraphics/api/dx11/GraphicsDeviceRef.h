#ifndef INC_LGRAPHICS_DX11_GRAPHICSDEVICEREF_H__
#define INC_LGRAPHICS_DX11_GRAPHICSDEVICEREF_H__
/**
@file GraphicsDevice.h
@author t-sakai
@date 2010/03/23 create
*/
#include "../../lgraphicscore.h"
#include "../../lgraphicsAPIInclude.h"
#include "Enumerations.h"

struct IDXGISwapChain;

namespace lgraphics
{
    struct InitParam;

    struct Box
    {
        u32 left_;
        u32 top_;
        u32 front_;
        u32 right_;
        u32 bottom_;
        u32 back_;
    };

    /**
    @brief Direct3D11 グラフィクスデバイス
    */
    class GraphicsDeviceRef
    {
    public:
        enum ClearDepthStencilFlag
        {
            ClearDepth = D3D11_CLEAR_DEPTH,
            ClearStencil = D3D11_CLEAR_STENCIL,
        };

        enum RasterizerState
        {
            Rasterizer_FillSolid = 0,
            Rasterizer_FillWireFrame,
            Rasterizer_DepthMap,
            Rasterizer_Num,
        };

        enum DepthStencilState
        {
            DepthStencil_DEnableWEnable =0,
            DepthStencil_DEnableWDisable,
            DepthStencil_DDisableWEnable,
            DepthStencil_DDisableWDisable,
            DepthStencil_Num,
        };

        enum BlendState
        {
            BlendState_NoAlpha =0,
            BlendState_Alpha,
            BlendState_AlphaAdditive,
            BlendState_Num,
        };

        static const Char* GSModel;
        static const Char* VSModel;
        static const Char* PSModel;

        u32 getFeatureLevel() const{ return featureLevel_;}
        ID3D11Device* getD3DDevice(){ return device_;}
        ID3D11DeviceContext* getContext(){ return context_;}

        void getRenderTargetDesc(u32& width, u32& height);

        inline void present();

        inline void setClearColor(const f32* color);
        inline void setClearDepthStencil(u32 flag, f32 depth, u8 stencil);

        inline void clearRenderTargetView();
        inline void clearDepthStencilView();

        inline void clearRenderTargetView(ID3D11RenderTargetView* view, const f32* color);
        inline void clearDepthStencilView(ID3D11DepthStencilView* view, u32 flags, f32 depth, u8 stencil);

        inline void draw(u32 numVertices, u32 start);
        inline void drawIndexed(u32 numIndices, u32 start, u32 baseVertex);

        inline void drawInstanced(u32 numVertices, u32 numInstances, u32 startVertex, u32 startInstance);

        void setViewport(s32 x, s32 y, u32 width, u32 height);

        inline void setRasterizerState(RasterizerState state);

        inline void setBlendState(ID3D11BlendState* state);
        inline void setBlendState(BlendState state);
        inline void setDepthStencilState(DepthStencilState state);

        inline void setPrimitiveTopology(Primitive topology);
        inline void setInputLayout(ID3D11InputLayout* layout);
        inline void setVertexBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers, const u32* strides, const u32* offsets);
        inline void setIndexBuffer(ID3D11Buffer* buffer, DataFormat format, u32 offset);

        inline void setVSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setGSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setPSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);

        inline void setVSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setGSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setPSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);

        inline void setVSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setGSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setPSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);

        inline void setGeometryShader(ID3D11GeometryShader* shader);
        inline void setVertexShader(ID3D11VertexShader* shader);
        inline void setPixelShader(ID3D11PixelShader* shader);

        inline void setRenderTargets(
            u32 numViews,
            ID3D11RenderTargetView* const* views,
            ID3D11DepthStencilView* depthStencilView);

        inline void getRenderTargets(
            u32 numViews,
            ID3D11RenderTargetView** views,
            ID3D11DepthStencilView** depthStencilView);

        inline void updateSubresource(
            ID3D11Resource* dstResource,
            u32 dstSubresource,
            const Box* dstBox,
            const void* srcData,
            u32 srcRowPitch,
            u32 srcDepthPitch);

        inline void clearVSResources(u32 numResources);
        inline void clearGSResources(u32 numResources);
        inline void clearPSResources(u32 numResources);
        inline void clearRenderTargets(u32 numResources);
    private:
        static const u32 MaxShaderResources = 32;
        static ID3D11ShaderResourceView* const NULLResources[MaxShaderResources];

        static ID3D11RenderTargetView* const NullTargets[MaxRenderTargets];

        friend class Graphics;

        GraphicsDeviceRef();
        ~GraphicsDeviceRef();

        bool initialize(const InitParam& initParam);
        void terminate();

        f32 clearColor_[4];
        u32 clearDepthStencilFlag_;
        f32 clearDepth_;
        u8 clearStencil_;
        u32 syncInterval_;
        u32 featureLevel_;

        ID3D11RasterizerState* rasterizerStates_[Rasterizer_Num];

        f32 blendFactors_[MaxRenderTargets];
        ID3D11DepthStencilState* depthStencilStates_[DepthStencil_Num];
        ID3D11BlendState* blendStates_[BlendState_Num];

        ID3D11Device *device_; /// デバイス実態
        ID3D11DeviceContext *context_; /// デバイスコンテキスト
        IDXGISwapChain *swapChain_; /// スワップチェイン
        ID3D11RenderTargetView* renderTargetView_;
        ID3D11Texture2D* depthStencil_;
        ID3D11DepthStencilView* depthStencilView_;
    };

    inline void GraphicsDeviceRef::present()
    {
        swapChain_->Present(syncInterval_, 0);
    }

    inline void GraphicsDeviceRef::setClearColor(const f32* color)
    {
        clearColor_[0] = color[0];
        clearColor_[1] = color[1];
        clearColor_[2] = color[2];
        clearColor_[3] = color[3];
    }

    inline void GraphicsDeviceRef::setClearDepthStencil(u32 flag, f32 depth, u8 stencil)
    {
        clearDepthStencilFlag_ = flag;
        clearDepth_ = depth;
        clearStencil_ = stencil;
    }

    inline void GraphicsDeviceRef::clearRenderTargetView()
    {
        context_->ClearRenderTargetView(renderTargetView_, clearColor_);
    }

    inline void GraphicsDeviceRef::clearDepthStencilView()
    {
        context_->ClearDepthStencilView(depthStencilView_, clearDepthStencilFlag_, clearDepth_, clearStencil_);
    }

    inline void GraphicsDeviceRef::clearRenderTargetView(ID3D11RenderTargetView* view, const f32* color)
    {
        context_->ClearRenderTargetView(view, color);
    }

    inline void GraphicsDeviceRef::clearDepthStencilView(ID3D11DepthStencilView* view, u32 flags, f32 depth, u8 stencil)
    {
        context_->ClearDepthStencilView(view, flags, depth, stencil);
    }

    inline void GraphicsDeviceRef::draw(u32 numVertices, u32 start)
    {
        context_->Draw(numVertices, start);
    }

    inline void GraphicsDeviceRef::drawIndexed(u32 numIndices, u32 start, u32 baseVertex)
    {
        context_->DrawIndexed(numIndices, start, baseVertex);
    }

    inline void GraphicsDeviceRef::drawInstanced(u32 numVertices, u32 numInstances, u32 startVertex, u32 startInstance)
    {
        context_->DrawInstanced(numVertices, numInstances, startVertex, startInstance);
    }

    inline void GraphicsDeviceRef::setRasterizerState(RasterizerState state)
    {
        context_->RSSetState(rasterizerStates_[state]);
    }

    inline void GraphicsDeviceRef::setBlendState(ID3D11BlendState* state)
    {
        context_->OMSetBlendState(state, blendFactors_, 0xFFFFFFFFU);
    }

    inline void GraphicsDeviceRef::setBlendState(BlendState state)
    {
        setBlendState(blendStates_[state]);
    }

    inline void GraphicsDeviceRef::setDepthStencilState(DepthStencilState state)
    {
        context_->OMSetDepthStencilState(depthStencilStates_[state], 0);
    }

    inline void GraphicsDeviceRef::setPrimitiveTopology(Primitive topology)
    {
        context_->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
    }

    inline void GraphicsDeviceRef::setInputLayout(ID3D11InputLayout* layout)
    {
        context_->IASetInputLayout(layout);
    }

    inline void GraphicsDeviceRef::setVertexBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers, const u32* strides, const u32* offsets)
    {
        context_->IASetVertexBuffers(start, num, buffers, strides, offsets);
    }

    inline void GraphicsDeviceRef::setIndexBuffer(ID3D11Buffer* buffer, DataFormat format, u32 offset)
    {
        context_->IASetIndexBuffer(buffer, static_cast<DXGI_FORMAT>(format), offset);
    }

    inline void GraphicsDeviceRef::setVSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->VSSetConstantBuffers(start, num, buffers);
    }

    inline void GraphicsDeviceRef::setGSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->GSSetConstantBuffers(start, num, buffers);
    }

    inline void GraphicsDeviceRef::setPSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->PSSetConstantBuffers(start, num, buffers);
    }

    inline void GraphicsDeviceRef::setVSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->VSSetShaderResources(start, num, views);
    }

    inline void GraphicsDeviceRef::setGSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->GSSetShaderResources(start, num, views);
    }

    inline void GraphicsDeviceRef::setPSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->PSSetShaderResources(start, num, views);
    }

    inline void GraphicsDeviceRef::setVSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->VSSetSamplers(start, num, states);
    }

    inline void GraphicsDeviceRef::setGSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->GSSetSamplers(start, num, states);
    }

    inline void GraphicsDeviceRef::setPSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->PSSetSamplers(start, num, states);
    }

    inline void GraphicsDeviceRef::setGeometryShader(ID3D11GeometryShader* shader)
    {
        context_->GSSetShader(shader, NULL, 0);
    }

    inline void GraphicsDeviceRef::setVertexShader(ID3D11VertexShader* shader)
    {
        context_->VSSetShader(shader, NULL, 0);
    }

    inline void GraphicsDeviceRef::setPixelShader(ID3D11PixelShader* shader)
    {
        context_->PSSetShader(shader, NULL, 0);
    }

    inline void GraphicsDeviceRef::setRenderTargets(
        u32 numViews,
        ID3D11RenderTargetView* const* views,
        ID3D11DepthStencilView* depthStencilView)
    {
        context_->OMSetRenderTargets(numViews, views, depthStencilView);
    }

    inline void GraphicsDeviceRef::getRenderTargets(
        u32 numViews,
        ID3D11RenderTargetView** views,
        ID3D11DepthStencilView** depthStencilView)
    {
        context_->OMGetRenderTargets(numViews, views, depthStencilView);
    }

    inline void GraphicsDeviceRef::updateSubresource(
        ID3D11Resource* dstResource,
        u32 dstSubresource,
        const Box* dstBox,
        const void* srcData,
        u32 srcRowPitch,
        u32 srcDepthPitch)
    {
        context_->UpdateSubresource(
            dstResource,
            dstSubresource,
            reinterpret_cast<const D3D11_BOX*>(dstBox),
            srcData,
            srcRowPitch,
            srcDepthPitch);
    }

    inline void GraphicsDeviceRef::clearVSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->VSSetShaderResources(0, numResources, NULLResources);
    }

    inline void GraphicsDeviceRef::clearGSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->GSSetShaderResources(0, numResources, NULLResources);
    }

    inline void GraphicsDeviceRef::clearPSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->PSSetShaderResources(0, numResources, NULLResources);
    }

    inline void GraphicsDeviceRef::clearRenderTargets(u32 numTargets)
    {
        LASSERT(numTargets<MaxRenderTargets);
        context_->OMSetRenderTargets(numTargets, NullTargets, NULL);
    }
}
#endif //INC_LGRAPHICS_DX11_GRAPHICSDEVICEREF_H__