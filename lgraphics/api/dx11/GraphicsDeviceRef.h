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
struct ID3D11CommandList;
struct ID3D11DeviceContext;

namespace lgraphics
{
    struct InitParam;
    class BlendStateRef;
    class AsynchronousRef;
    class PredicateRef;
    class ContextRef;

    template<class T>
    inline void setDebugObjectName(T* ptr, const Char* name)
    {
        LASSERT(NULL != name);
        size_t length = lcore::strlen(name);
        ptr->SetPrivateData(WKPDID_D3DDebugObjectName, length, name);
    }
#ifdef _DEBUG
#define LIME_DEBUGNAME(ptr, name) setDebugObjectName(ptr, name)
#else
#define LIME_DEBUGNAME(ptr, name) 
#endif

    //--------------------------------------
    //---
    //--- DepthStencilStateRef
    //---
    //--------------------------------------
    class DepthStencilStateRef
    {
    public:
        typedef ID3D11DepthStencilState element_type;
        typedef ID3D11DepthStencilState* pointer_type;

        struct StencilOPDesc
        {
            StencilOp failOp_;
            StencilOp depthFailOp_;
            StencilOp passOp_;
            CmpFunc cmpFunc_;
        };

        DepthStencilStateRef();
        DepthStencilStateRef(const DepthStencilStateRef& rhs);
        ~DepthStencilStateRef();

        void destroy();

        bool valid() const;
        void attach(ContextRef& context, u32 stencilRef);

        DepthStencilStateRef& operator=(const DepthStencilStateRef& rhs);
        void swap(DepthStencilStateRef& rhs);
    private:
        friend class GraphicsDeviceRef;

        explicit DepthStencilStateRef(pointer_type state);

        pointer_type state_;
    };

    //--------------------------------------
    //---
    //--- CommandListRef
    //---
    //--------------------------------------
    class CommandListRef
    {
    public:
        typedef ID3D11CommandList element_type;
        typedef ID3D11CommandList* pointer_type;

        CommandListRef();
        CommandListRef(const CommandListRef& rhs);
        ~CommandListRef();

        bool valid() const{ return NULL != commandList_;}

        CommandListRef& operator=(const CommandListRef& rhs);
        void swap(CommandListRef& rhs);
    private:
        friend class ContextRef;
        explicit CommandListRef(pointer_type commandList);

        pointer_type commandList_;
    };

    //--------------------------------------
    //---
    //--- ContextRef
    //---
    //--------------------------------------
    class ContextRef
    {
    public:
        enum FeatureFlag
        {
            FeatureFlag_None = 0,
            FeatureFlag_Immediate = (0x01U<<0),
            FeatureFlag_Deffered = (0x01U<<1),
            FeatureFlag_CommandList = (0x01U<<2),
        };

        enum ClearDepthStencilFlag
        {
            ClearDepth = D3D11_CLEAR_DEPTH,
            ClearStencil = D3D11_CLEAR_STENCIL,
        };

        enum RasterizerState
        {
            Rasterizer_FillSolid = 0,
            Rasterizer_FillSolidNoCull,
            Rasterizer_FillWireFrame,
            Rasterizer_FillWireFrameNoCull,
            Rasterizer_DepthMap,
            Rasterizer_Num,
        };

        enum DepthStencilState
        {
            DepthStencil_DEnableWEnable =0,
            DepthStencil_DEnableWDisable,
            DepthStencil_DDisableWEnable,
            DepthStencil_DDisableWDisable,

            DepthStencil_DEnableWDisableLessEqual,
            DepthStencil_Num,
        };

        enum BlendState
        {
            BlendState_NoAlpha =0,
            BlendState_Alpha,
            BlendState_AlphaAdditive,
            BlendState_Add,
            BlendState_Num,
        };

        static const u32 MaxSOBufferSlot = D3D11_SO_BUFFER_SLOT_COUNT;
        static const u32 MaxSOStreamCount = D3D11_SO_STREAM_COUNT;
        static const u32 SONoRasterizedStream = D3D11_SO_NO_RASTERIZED_STREAM;

        static const Char* GSModel;
        static const Char* VSModel;
        static const Char* PSModel;
        static const Char* CSModel;
        static const Char* DSModel;
        static const Char* HSModel;

        static const u32 MinRefreshRate = 15;
        static const u32 MaxRefreshRate = 120;

        static const f32 Zero[4];
        static const f32 One[4];
        static const u32 UIZero[4];

        static const u32 MaxMultiSampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;

        ID3D11DeviceContext* getContext(){ return context_;}

        ID3D11Texture2D* getBackBuffer() { return backBuffer_; }
        ID3D11RenderTargetView* getBackBufferRTView(){ return renderTargetView_; }

        ID3D11Texture2D* getDepthStencil(){ return depthStencil_; }
        ID3D11DepthStencilView* getDepthStencilView() { return depthStencilView_; }
        ID3D11ShaderResourceView* getDepthStencilSRView() { return depthStencilShaderResourceView_; }

        inline void clearRenderTargetView(ID3D11RenderTargetView* view, const f32* color);
        inline void clearDepthStencilView(ID3D11DepthStencilView* view, u32 flags, f32 depth, u8 stencil);
        inline void clearUnorderedAccessView(ID3D11UnorderedAccessView* view, const u32 values[4]);
        inline void clearUnorderedAccessView(ID3D11UnorderedAccessView* view, const f32 values[4]);

        inline void draw(u32 numVertices, u32 start);
        inline void drawIndexed(u32 numIndices, u32 start, u32 baseVertex);

        inline void drawInstanced(u32 numVertices, u32 numInstances, u32 startVertex, u32 startInstance);
        inline void drawIndexedInstanced(u32 numIndices, u32 numInstances, u32 startIndex, u32 baseVertex, u32 startInstance);

        inline void drawAuto();

        inline void dispatch(u32 xthreads, u32 ythreads, u32 zthreads);
        inline void dispatchIndirect(ID3D11Buffer* buffers, u32 alignedOffset);

        void setViewport(u32 x, u32 y, u32 width, u32 height);
        void setDefaultViewport(u32 x, u32 y, u32 width, u32 height);
        void restoreDefaultViewport();

        void restoreDefaultRenderTargets();

        void getRenderTargetDesc(u32& width, u32& height);

        inline void setRasterizerState(RasterizerState state);

        inline void setBlendState(ID3D11BlendState* state);
        inline void setBlendState(BlendState state);
        inline void setDepthStencilState(DepthStencilState state);
        inline void setDepthStencilState(ID3D11DepthStencilState* state, u32 stencilRef);

        inline void setVSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);
        inline void setGSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);
        inline void setPSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);
        inline void setCSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);
        inline void setDSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);
        inline void setHSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers);

        inline void setPrimitiveTopology(Primitive topology);
        inline void setInputLayout(ID3D11InputLayout* layout);
        inline void setVertexBuffers(u32 startSlot, u32 num, ID3D11Buffer* const* buffers, const u32* strides, const u32* offsetInBytes);
        inline void setVertexBuffer(u32 startSlot, ID3D11Buffer* buffer, u32 strides, u32 offsetInBytes);
        inline void clearVertexBuffers(u32 startSlot, u32 num);

        inline void setIndexBuffer(ID3D11Buffer* buffer, DataFormat format, u32 offsetInBytes);

        inline void setVSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setGSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setPSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setCSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setDSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);
        inline void setHSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers);

        inline void setVSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setGSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setPSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setCSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setDSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);
        inline void setHSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views);

        inline void setVSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setGSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setPSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setCSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setDSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);
        inline void setHSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states);

        inline void setGeometryShader(ID3D11GeometryShader* shader);
        inline void setVertexShader(ID3D11VertexShader* shader);
        inline void setPixelShader(ID3D11PixelShader* shader);
        inline void setComputeShader(ID3D11ComputeShader* shader);
        inline void setDomainShader(ID3D11DomainShader* shader);
        inline void setHullShader(ID3D11HullShader* shader);

        inline void setRenderTargets(
            u32 numViews,
            ID3D11RenderTargetView* const* views,
            ID3D11DepthStencilView* depthStencilView);

        inline void getRenderTargets(
            u32 numViews,
            ID3D11RenderTargetView** views,
            ID3D11DepthStencilView** depthStencilView);

        inline void setRenderTargetsAndUAV(
            u32 numViews,
            ID3D11RenderTargetView* const* views,
            ID3D11DepthStencilView* depthStencilView,
            u32 UAVStart,
            u32 numUAVs,
            ID3D11UnorderedAccessView* const* uavs,
            const u32* UAVInitCounts);

        inline void setCSUnorderedAccessViews(
            u32 UAVStart,
            u32 numUAVs,
            ID3D11UnorderedAccessView* const* uavs,
            const u32* UAVInitCounts);

        inline void setStreamOutTargets(u32 num, ID3D11Buffer* const* buffers, const u32* offsetInBytes);
        inline void clearStreamOutTargets(u32 num);

        inline void updateSubresource(
            ID3D11Resource* dstResource,
            u32 dstSubresource,
            const Box* dstBox,
            const void* srcData,
            u32 srcRowPitch,
            u32 srcDepthPitch);

        inline void updateSubresource(
            ID3D11Resource* dstResource,
            u32 dstSubresource,
            const void* srcData,
            u32 srcRowPitch,
            u32 srcDepthPitch);

        inline void clearVSResources(u32 numResources);
        inline void clearGSResources(u32 numResources);
        inline void clearPSResources(u32 numResources);
        inline void clearCSResources(u32 numResources);
        inline void clearDSResources(u32 numResources);
        inline void clearHSResources(u32 numResources);

        inline void clearRenderTargets(u32 numResources);
        inline void clearCSUnorderedAccessView(u32 numResources);

        inline void copyResource(ID3D11Resource* dst, u32 dstSubResource, u32 dstX, u32 dstY, u32 dstZ, ID3D11Resource* src, u32 srcSubResource, const Box* box);
        inline void copyResource(ID3D11Resource* dst, ID3D11Resource* src);
        inline void copyStructureCount(ID3D11Buffer* dst, u32 dstOffset, ID3D11UnorderedAccessView* view);

        inline bool map(ID3D11Resource* resource, u32 subresource, MapType type, MappedSubresource& mapped);
        inline void unmap(ID3D11Resource* resource, u32 subresource);

        void begin(AsynchronousRef& asynchronous);
        void end(AsynchronousRef& asynchronous);
        void setPredicate(PredicateRef& predicate, BOOL predicateValue);
        s32 getData(AsynchronousRef& asynchronous, void* data, u32 size, u32 flag);

        CommandListRef finishCommandList(s32 restoreState);
        void executeCommandList(CommandListRef& commandList, s32 restoreState);
    protected:
        static const u32 MaxShaderResources = 32;
        static ID3D11ShaderResourceView* const NULLResources[MaxShaderResources];

        static ID3D11RenderTargetView* const NullTargets[MaxRenderTargets];

        ContextRef();
        ContextRef(const ContextRef& rhs);
        ContextRef(ID3D11Device* device, ID3D11DeviceContext* context, const ContextRef& rhs);
        ~ContextRef();

        void addRef();

        void setFeatureFlags(ID3D11Device* device);
        inline bool checkFeature(FeatureFlag flag) const;

        ContextRef& operator=(const ContextRef&);

        void terminate();

        void swap(ContextRef& rhs);

        u32 featureFlags_;
        u32 viewportX_;
        u32 viewportY_;
        u32 viewportWidth_;
        u32 viewportHeight_;

        ID3D11DeviceContext* context_;

        ID3D11RasterizerState* rasterizerStates_[Rasterizer_Num];

        f32 blendFactors_[MaxRenderTargets];
        ID3D11DepthStencilState* depthStencilStates_[DepthStencil_Num];
        ID3D11BlendState* blendStates_[BlendState_Num];

        ID3D11Texture2D* backBuffer_;
        ID3D11RenderTargetView* renderTargetView_;
        ID3D11Texture2D* depthStencil_;
        ID3D11DepthStencilView* depthStencilView_;
        ID3D11ShaderResourceView* depthStencilShaderResourceView_;
    };

    inline void ContextRef::clearRenderTargetView(ID3D11RenderTargetView* view, const f32* color)
    {
        context_->ClearRenderTargetView(view, color);
    }

    inline void ContextRef::clearDepthStencilView(ID3D11DepthStencilView* view, u32 flags, f32 depth, u8 stencil)
    {
        context_->ClearDepthStencilView(view, flags, depth, stencil);
    }

    inline void ContextRef::clearUnorderedAccessView(ID3D11UnorderedAccessView* view, const u32 values[4])
    {
        context_->ClearUnorderedAccessViewUint(view, values);
    }

    inline void ContextRef::clearUnorderedAccessView(ID3D11UnorderedAccessView* view, const f32 values[4])
    {
        context_->ClearUnorderedAccessViewFloat(view, values);
    }

    inline void ContextRef::draw(u32 numVertices, u32 start)
    {
        context_->Draw(numVertices, start);
    }

    inline void ContextRef::drawIndexed(u32 numIndices, u32 start, u32 baseVertex)
    {
        context_->DrawIndexed(numIndices, start, baseVertex);
    }

    inline void ContextRef::drawInstanced(u32 numVertices, u32 numInstances, u32 startVertex, u32 startInstance)
    {
        context_->DrawInstanced(numVertices, numInstances, startVertex, startInstance);
    }

    inline void ContextRef::drawIndexedInstanced(u32 numIndices, u32 numInstances, u32 startIndex, u32 baseVertex, u32 startInstance)
    {
        context_->DrawIndexedInstanced(numIndices, numInstances, startIndex, baseVertex, startInstance);
    }

    inline void ContextRef::drawAuto()
    {
        context_->DrawAuto();
    }

    inline void ContextRef::dispatch(u32 xthreads, u32 ythreads, u32 zthreads)
    {
        context_->Dispatch(xthreads, ythreads, zthreads);
    }

    inline void ContextRef::dispatchIndirect(ID3D11Buffer* buffers, u32 alignedOffset)
    {
        context_->DispatchIndirect(buffers, alignedOffset);
    }

    inline void ContextRef::setRasterizerState(RasterizerState state)
    {
        context_->RSSetState(rasterizerStates_[state]);
    }

    inline void ContextRef::setBlendState(ID3D11BlendState* state)
    {
        context_->OMSetBlendState(state, blendFactors_, 0xFFFFFFFFU);
    }

    inline void ContextRef::setBlendState(BlendState state)
    {
        context_->OMSetBlendState(blendStates_[state], blendFactors_, 0xFFFFFFFFU);
    }

    inline void ContextRef::setDepthStencilState(DepthStencilState state)
    {
        context_->OMSetDepthStencilState(depthStencilStates_[state], 0);
    }

    inline void ContextRef::setDepthStencilState(ID3D11DepthStencilState* state, u32 stencilRef)
    {
        context_->OMSetDepthStencilState(state, stencilRef);
    }

    inline void ContextRef::setVSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->VSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setGSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->GSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setPSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->PSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setCSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->CSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setDSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->DSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setHSSamplerStates(u32 start, u32 num, ID3D11SamplerState* const* samplers)
    {
        context_->HSSetSamplers(start, num, samplers);
    }

    inline void ContextRef::setPrimitiveTopology(Primitive topology)
    {
        context_->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
    }

    inline void ContextRef::setInputLayout(ID3D11InputLayout* layout)
    {
        context_->IASetInputLayout(layout);
    }

    inline void ContextRef::setVertexBuffers(u32 startSlot, u32 num, ID3D11Buffer* const* buffers, const u32* strides, const u32* offsetInBytes)
    {
        context_->IASetVertexBuffers(startSlot, num, buffers, strides, offsetInBytes);
    }

    inline void ContextRef::setVertexBuffer(u32 startSlot, ID3D11Buffer* buffer, u32 strides, u32 offsetInBytes)
    {
        context_->IASetVertexBuffers(startSlot, 1, &buffer, &strides, &offsetInBytes);
    }

    inline void ContextRef::clearVertexBuffers(u32 startSlot, u32 num)
    {
        ID3D11Buffer* const buffers[] = {NULL, NULL, NULL, NULL};
        const u32 strides[] = {0, 0, 0, 0};
        const u32 offsets[] = {0, 0, 0, 0};

        context_->IASetVertexBuffers(startSlot, num, buffers, strides, offsets);
    }

    inline void ContextRef::setIndexBuffer(ID3D11Buffer* buffer, DataFormat format, u32 offsetInBytes)
    {
        context_->IASetIndexBuffer(buffer, static_cast<DXGI_FORMAT>(format), offsetInBytes);
    }

    inline void ContextRef::setVSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->VSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setGSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->GSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setPSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->PSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setCSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->CSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setDSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->DSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setHSConstantBuffers(u32 start, u32 num, ID3D11Buffer* const* buffers)
    {
        context_->HSSetConstantBuffers(start, num, buffers);
    }

    inline void ContextRef::setVSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->VSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setGSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->GSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setPSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->PSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setCSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->CSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setDSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->DSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setHSResources(u32 start, u32 num, ID3D11ShaderResourceView* const* views)
    {
        context_->HSSetShaderResources(start, num, views);
    }

    inline void ContextRef::setVSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->VSSetSamplers(start, num, states);
    }

    inline void ContextRef::setGSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->GSSetSamplers(start, num, states);
    }

    inline void ContextRef::setPSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->PSSetSamplers(start, num, states);
    }

    inline void ContextRef::setCSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->CSSetSamplers(start, num, states);
    }

    inline void ContextRef::setDSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->DSSetSamplers(start, num, states);
    }

    inline void ContextRef::setHSSamplers(u32 start, u32 num, ID3D11SamplerState* const* states)
    {
        context_->HSSetSamplers(start, num, states);
    }

    inline void ContextRef::setGeometryShader(ID3D11GeometryShader* shader)
    {
        context_->GSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setVertexShader(ID3D11VertexShader* shader)
    {
        context_->VSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setPixelShader(ID3D11PixelShader* shader)
    {
        context_->PSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setComputeShader(ID3D11ComputeShader* shader)
    {
        context_->CSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setDomainShader(ID3D11DomainShader* shader)
    {
        context_->DSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setHullShader(ID3D11HullShader* shader)
    {
        context_->HSSetShader(shader, NULL, 0);
    }

    inline void ContextRef::setRenderTargets(
        u32 numViews,
        ID3D11RenderTargetView* const* views,
        ID3D11DepthStencilView* depthStencilView)
    {
        context_->OMSetRenderTargets(numViews, views, depthStencilView);
    }

    inline void ContextRef::getRenderTargets(
        u32 numViews,
        ID3D11RenderTargetView** views,
        ID3D11DepthStencilView** depthStencilView)
    {
        context_->OMGetRenderTargets(numViews, views, depthStencilView);
    }

    inline void ContextRef::setRenderTargetsAndUAV(
        u32 numViews,
        ID3D11RenderTargetView* const* views,
        ID3D11DepthStencilView* depthStencilView,
        u32 UAVStart,
        u32 numUAVs,
        ID3D11UnorderedAccessView* const* uavs,
        const u32* UAVInitCounts)
    {
        context_->OMSetRenderTargetsAndUnorderedAccessViews(numViews, views, depthStencilView, UAVStart, numUAVs, uavs, UAVInitCounts);
    }

    inline void ContextRef::setCSUnorderedAccessViews(
        u32 UAVStart,
        u32 numUAVs,
        ID3D11UnorderedAccessView* const* uavs,
        const u32* UAVInitCounts)
    {
        context_->CSSetUnorderedAccessViews(UAVStart, numUAVs, uavs, UAVInitCounts);
    }

    inline void ContextRef::setStreamOutTargets(u32 num, ID3D11Buffer* const* buffers, const u32* offsetInBytes)
    {
        context_->SOSetTargets(num, buffers, offsetInBytes);
    }

    inline void ContextRef::clearStreamOutTargets(u32 num)
    {
        ID3D11Buffer* const buffers[MaxSOBufferSlot] = {NULL, NULL, NULL, NULL};
        const u32 offsets[MaxSOBufferSlot] = {0, 0, 0, 0};

        context_->SOSetTargets(num, buffers, offsets);
    }

    inline void ContextRef::updateSubresource(
        ID3D11Resource* dstResource,
        u32 dstSubresource,
        const Box* dstBox,
        const void* srcData,
        u32 srcRowPitch,
        u32 srcDepthPitch)
    {
        LASSERT(!checkFeature(FeatureFlag_Deffered));

        context_->UpdateSubresource(
            dstResource,
            dstSubresource,
            reinterpret_cast<const D3D11_BOX*>(dstBox),
            srcData,
            srcRowPitch,
            srcDepthPitch);
    }

    inline void ContextRef::updateSubresource(
        ID3D11Resource* dstResource,
        u32 dstSubresource,
        const void* srcData,
        u32 srcRowPitch,
        u32 srcDepthPitch)
    {
        LASSERT(!checkFeature(FeatureFlag_Deffered));

        context_->UpdateSubresource(
            dstResource,
            dstSubresource,
            NULL,
            srcData,
            srcRowPitch,
            srcDepthPitch);
    }

    inline void ContextRef::clearVSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->VSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearGSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->GSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearPSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->PSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearCSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->CSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearDSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->DSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearHSResources(u32 numResources)
    {
        LASSERT(numResources<MaxShaderResources);
        context_->HSSetShaderResources(0, numResources, NULLResources);
    }

    inline void ContextRef::clearRenderTargets(u32 numTargets)
    {
        LASSERT(numTargets<MaxRenderTargets);
        context_->OMSetRenderTargets(numTargets, NullTargets, NULL);
    }

    inline void ContextRef::clearCSUnorderedAccessView(u32 numResources)
    {
        LASSERT(numResources<MaxRenderTargets);
        context_->CSSetUnorderedAccessViews(0, numResources, (ID3D11UnorderedAccessView* const *)NullTargets, NULL);
    }

    inline void ContextRef::copyResource(ID3D11Resource* dst, u32 dstSubResource, u32 dstX, u32 dstY, u32 dstZ, ID3D11Resource* src, u32 srcSubResource, const Box* box)
    {
        context_->CopySubresourceRegion(dst, dstSubResource, dstX, dstY, dstZ, src, srcSubResource, reinterpret_cast<const D3D11_BOX*>(box));
    }

    inline void ContextRef::copyResource(ID3D11Resource* dst, ID3D11Resource* src)
    {
        context_->CopyResource(dst, src);
    }

    inline void ContextRef::copyStructureCount(ID3D11Buffer* dst, u32 dstOffset, ID3D11UnorderedAccessView* view)
    {
        context_->CopyStructureCount(dst, dstOffset, view);
    }

    inline bool ContextRef::map(ID3D11Resource* resource, u32 subresource, MapType type, MappedSubresource& mapped)
    {
        HRESULT hr = context_->Map(resource, subresource, (D3D11_MAP)type, 0, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mapped));
        return SUCCEEDED(hr);
    }

    inline void ContextRef::unmap(ID3D11Resource* resource, u32 subresource)
    {
        context_->Unmap(resource, subresource);
    }

    inline bool ContextRef::checkFeature(FeatureFlag flag) const
    {
        return 0 != (featureFlags_&flag);
    }

    //--------------------------------------
    //---
    //--- DefferedContextRef
    //---
    //--------------------------------------
    class DefferedContextRef : public ContextRef
    {
    public:
        DefferedContextRef();
        DefferedContextRef(const DefferedContextRef& rhs);
        DefferedContextRef(ID3D11Device* device, ID3D11DeviceContext* context, const ContextRef& rhs);
        ~DefferedContextRef();

        DefferedContextRef& operator=(const DefferedContextRef& rhs);
        void swap(DefferedContextRef& rhs);

        void terminate();
    protected:
        friend class GraphicsDeviceRef;
    };

    //--------------------------------------
    //---
    //--- GraphicsDeviceRef
    //---
    //--------------------------------------
    /**
    @brief Direct3D11 グラフィクスデバイス
    */
    class GraphicsDeviceRef : public ContextRef
    {
    public:

        u32 getRefreshRate() const{ return refreshRate_;}
        u32 getFeatureLevel() const{ return featureLevel_;}
        u32 getMaximumFrameLatency() const{ return maximumFrameLatency_; }

        ID3D11Device* getD3DDevice(){ return device_;}

        void onSize(u32 width, u32 height);

        bool isFullScreen();
        void changeScreenMode();

        inline void present();

        DepthStencilStateRef createDepthStencilState(
            bool depthEnable,
            DepthWriteMask depthWriteMask,
            CmpFunc depthFunc,
            bool stencilEnable,
            u8 stencilReadMask,
            u8 stencilWriteMask,
            const DepthStencilStateRef::StencilOPDesc& frontFace,
            const DepthStencilStateRef::StencilOPDesc& backFace);

        bool checkMultisampleQualityLevels(
            lgraphics::DataFormat format,
            u32 sampleCount,
            u32* qualityLevels);

        DefferedContextRef createDefferedContext();
    private:
        friend class Graphics;

        GraphicsDeviceRef();
        ~GraphicsDeviceRef();

        bool initialize(const InitParam& initParam);
        void terminate();
        bool createBackBuffer(u32 width, u32 height, u32 depthStencilFormat, u32 flag);

        u32 flags_;
        u32 syncInterval_;
        u32 refreshRate_;
        u32 featureLevel_;
        u32 maximumFrameLatency_;

        ID3D11Device* device_; /// デバイス実態
        IDXGISwapChain* swapChain_; /// スワップチェイン
    };

    inline void GraphicsDeviceRef::present()
    {
        swapChain_->Present(syncInterval_, 0);
    }


    //--------------------------------------
    //---
    //--- ShaderResourceView
    //---
    //--------------------------------------
    class ShaderResourceView
    {
    public:
        inline static void set(
            ID3D11ShaderResourceView* res0,
            ID3D11ShaderResourceView* res1);

        inline static void set(
            ID3D11ShaderResourceView* res0,
            ID3D11ShaderResourceView* res1,
            ID3D11ShaderResourceView* res2);

        inline static void set(
            ID3D11ShaderResourceView* res0,
            ID3D11ShaderResourceView* res1,
            ID3D11ShaderResourceView* res2,
            ID3D11ShaderResourceView* res3);

        inline static void set(
            ID3D11ShaderResourceView* res0,
            ID3D11ShaderResourceView* res1,
            ID3D11ShaderResourceView* res2,
            ID3D11ShaderResourceView* res3,
            ID3D11ShaderResourceView* res4);

        inline static void set(
            ID3D11ShaderResourceView* res0,
            ID3D11ShaderResourceView* res1,
            ID3D11ShaderResourceView* res2,
            ID3D11ShaderResourceView* res3,
            ID3D11ShaderResourceView* res4,
            ID3D11ShaderResourceView* res5);

        inline static void setVS(ContextRef& context, u32 start, u32 num);
        inline static void setGS(ContextRef& context, u32 start, u32 num);
        inline static void setPS(ContextRef& context, u32 start, u32 num);
        inline static void setCS(ContextRef& context, u32 start, u32 num);

        static ID3D11ShaderResourceView* resources_[6];
    };

    inline void ShaderResourceView::set(
        ID3D11ShaderResourceView* res0,
        ID3D11ShaderResourceView* res1)
    {
        resources_[0] = res0;
        resources_[1] = res1;
    }

    inline void ShaderResourceView::set(
        ID3D11ShaderResourceView* res0,
        ID3D11ShaderResourceView* res1,
        ID3D11ShaderResourceView* res2)
    {
        resources_[0] = res0;
        resources_[1] = res1;
        resources_[2] = res2;
    }

    inline void ShaderResourceView::set(
        ID3D11ShaderResourceView* res0,
        ID3D11ShaderResourceView* res1,
        ID3D11ShaderResourceView* res2,
        ID3D11ShaderResourceView* res3)
    {
        resources_[0] = res0;
        resources_[1] = res1;
        resources_[2] = res2;
        resources_[3] = res3;
    }

    inline void ShaderResourceView::set(
        ID3D11ShaderResourceView* res0,
        ID3D11ShaderResourceView* res1,
        ID3D11ShaderResourceView* res2,
        ID3D11ShaderResourceView* res3,
        ID3D11ShaderResourceView* res4)
    {
        resources_[0] = res0;
        resources_[1] = res1;
        resources_[2] = res2;
        resources_[3] = res3;
        resources_[4] = res4;
    }

    inline void ShaderResourceView::set(
        ID3D11ShaderResourceView* res0,
        ID3D11ShaderResourceView* res1,
        ID3D11ShaderResourceView* res2,
        ID3D11ShaderResourceView* res3,
        ID3D11ShaderResourceView* res4,
        ID3D11ShaderResourceView* res5)
    {
        resources_[0] = res0;
        resources_[1] = res1;
        resources_[2] = res2;
        resources_[3] = res3;
        resources_[4] = res4;
        resources_[5] = res5;
    }

    inline void ShaderResourceView::setVS(ContextRef& context, u32 start, u32 num)
    {
        context.setVSResources(start, num, resources_);
    }

    inline void ShaderResourceView::setGS(ContextRef& context, u32 start, u32 num)
    {
        context.setGSResources(start, num, resources_);
    }

    inline void ShaderResourceView::setPS(ContextRef& context, u32 start, u32 num)
    {
        context.setPSResources(start, num, resources_);
    }

    inline void ShaderResourceView::setCS(ContextRef& context, u32 start, u32 num)
    {
        context.setCSResources(start, num, resources_);
    }

    //--------------------------------------
    //---
    //--- ShaderSamplerState
    //---
    //--------------------------------------
    class ShaderSamplerState
    {
    public:
        inline static void set(
            ID3D11SamplerState* state0,
            ID3D11SamplerState* state1);

        inline static void set(
            ID3D11SamplerState* state0,
            ID3D11SamplerState* state1,
            ID3D11SamplerState* state2);

        inline static void set(
            ID3D11SamplerState* state0,
            ID3D11SamplerState* state1,
            ID3D11SamplerState* state2,
            ID3D11SamplerState* state3);

        inline static void set(
            ID3D11SamplerState* state0,
            ID3D11SamplerState* state1,
            ID3D11SamplerState* state2,
            ID3D11SamplerState* state3,
            ID3D11SamplerState* state4);

        inline static void set(
            ID3D11SamplerState* state0,
            ID3D11SamplerState* state1,
            ID3D11SamplerState* state2,
            ID3D11SamplerState* state3,
            ID3D11SamplerState* state4,
            ID3D11SamplerState* state5);

        inline static void setVS(ContextRef& context, u32 start, u32 num);
        inline static void setGS(ContextRef& context, u32 start, u32 num);
        inline static void setPS(ContextRef& context, u32 start, u32 num);
        inline static void setCS(ContextRef& context, u32 start, u32 num);

        static ID3D11SamplerState* states_[6];
    };

    inline void ShaderSamplerState::set(
        ID3D11SamplerState* state0,
        ID3D11SamplerState* state1)
    {
        states_[0] = state0;
        states_[1] = state1;
    }

    inline void ShaderSamplerState::set(
        ID3D11SamplerState* state0,
        ID3D11SamplerState* state1,
        ID3D11SamplerState* state2)
    {
        states_[0] = state0;
        states_[1] = state1;
        states_[2] = state2;
    }

    inline void ShaderSamplerState::set(
        ID3D11SamplerState* state0,
        ID3D11SamplerState* state1,
        ID3D11SamplerState* state2,
        ID3D11SamplerState* state3)
    {
        states_[0] = state0;
        states_[1] = state1;
        states_[2] = state2;
        states_[3] = state3;
    }

    inline void ShaderSamplerState::set(
        ID3D11SamplerState* state0,
        ID3D11SamplerState* state1,
        ID3D11SamplerState* state2,
        ID3D11SamplerState* state3,
        ID3D11SamplerState* state4)
    {
        states_[0] = state0;
        states_[1] = state1;
        states_[2] = state2;
        states_[3] = state3;
        states_[4] = state4;
    }

    inline void ShaderSamplerState::set(
        ID3D11SamplerState* state0,
        ID3D11SamplerState* state1,
        ID3D11SamplerState* state2,
        ID3D11SamplerState* state3,
        ID3D11SamplerState* state4,
        ID3D11SamplerState* state5)
    {
        states_[0] = state0;
        states_[1] = state1;
        states_[2] = state2;
        states_[3] = state3;
        states_[4] = state4;
        states_[5] = state5;
    }

    inline void ShaderSamplerState::setVS(ContextRef& context, u32 start, u32 num)
    {
        context.setVSSamplerStates(start, num, states_);
    }

    inline void ShaderSamplerState::setGS(ContextRef& context, u32 start, u32 num)
    {
        context.setGSSamplerStates(start, num, states_);
    }

    inline void ShaderSamplerState::setPS(ContextRef& context, u32 start, u32 num)
    {
        context.setPSSamplerStates(start, num, states_);
    }

    inline void ShaderSamplerState::setCS(ContextRef& context, u32 start, u32 num)
    {
        context.setCSSamplerStates(start, num, states_);
    }
}
#endif //INC_LGRAPHICS_DX11_GRAPHICSDEVICEREF_H__
