#ifndef INC_LGRAPHICS_DX11_TEXTUREREF_H__
#define INC_LGRAPHICS_DX11_TEXTUREREF_H__
/**
@file TextureRef.h
@author t-sakai
@date 2012/07/24 create
*/
#include "../../lgraphics.h"
#include "Enumerations.h"
#include "SamplerStateRef.h"
#include "ViewRef.h"

struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;

namespace lgraphics
{
    class ContextRef;

    typedef D3D11_TEXTURE2D_DESC Texture2DDesc;

    //--------------------------------------------------------
    //---
    //--- TextureRefBase
    //---
    //--------------------------------------------------------
    template<class T>
    class TextureRefBase
    {
    public:
        typedef TextureRefBase<T> this_type;
        typedef T element_type;
        typedef T* pointer_type;

        pointer_type get() { return texture_;}
        bool hasSampler() const{ return sampler_.valid();}
        bool hasResourceView() const{ return shaderResourceView_.valid();}
        bool valid() const{ return (NULL != texture_);}

        SamplerStateRef::pointer_type const* getSamplerGet(){ return sampler_.get();}
        ShaderResourceViewRef::pointer_type const* getShaderResourceViewGet(){ return shaderResourceView_.get();}

        SamplerStateRef::pointer_type getSampler(){ return sampler_.getSampler(); }
        ShaderResourceViewRef::pointer_type getShaderResourceView(){ return shaderResourceView_.getView(); }

        ShaderResourceViewRef createSRView(const SRVDesc& desc)
        {
            return createSRView(desc, texture_);
        }

        RenderTargetViewRef createRTView(const RTVDesc& desc)
        {
            return createRTView(desc, texture_);
        }

        DepthStencilViewRef createDSView(const DSVDesc& desc)
        {
            return createDSView(desc, texture_);
        }

        UnorderedAccessViewRef createUAView(const UAVDesc& desc)
        {
            return View::createUAView(desc, texture_);
        }

        void setSamplerState(TextureFilterType filter, TextureAddress adress)
        {
            sampler_ = SamplerState::create(filter, adress, adress, adress);
        }

        inline void attachVS(ContextRef& context, u32 viewIndex, u32 samplerIndex);
        inline void attachGS(ContextRef& context, u32 viewIndex, u32 samplerIndex);
        inline void attachPS(ContextRef& context, u32 viewIndex, u32 samplerIndex);
        inline void attachCS(ContextRef& context, u32 viewIndex, u32 samplerIndex);

        inline void attachVS(ContextRef& context, u32 viewIndex);
        inline void attachGS(ContextRef& context, u32 viewIndex);
        inline void attachPS(ContextRef& context, u32 viewIndex);
        inline void attachCS(ContextRef& context, u32 viewIndex);

        inline void copy(ContextRef& context, this_type& src);
        inline void updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch);

        inline bool map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped);
        inline bool map(ContextRef& context, void*& data, u32& rowPitch, u32& depthPitch, u32 subresource, s32 type);
        inline void unmap(ContextRef& context, u32 subresource);

        inline bool operator==(const TextureRefBase& rhs) const
        {
            return (texture_ == rhs.texture_);
        }

        inline bool operator!=(const TextureRefBase& rhs) const
        {
            return (texture_ != rhs.texture_);
        }
    protected:

        TextureRefBase()
            :texture_(NULL)
        {}

        TextureRefBase(const this_type& rhs)
            :sampler_(rhs.sampler_)
            ,shaderResourceView_(rhs.shaderResourceView_)
            ,texture_(rhs.texture_)
        {
            if(texture_){
                texture_->AddRef();
            }
        }

        TextureRefBase(const SamplerStateRef& sampler, ShaderResourceViewRef::pointer_type view, pointer_type texture)
            :sampler_(sampler)
            ,shaderResourceView_(view)
            ,texture_(texture)
        {}

        ~TextureRefBase()
        {
            destroy();
        }

        void destroy()
        {
            sampler_.destroy();
            shaderResourceView_.destroy();
            SAFE_RELEASE(texture_);
        }

        void swap(this_type& rhs)
        {
            sampler_.swap(rhs.sampler_);
            shaderResourceView_.swap(rhs.shaderResourceView_);
            lcore::swap(texture_, rhs.texture_);
        }

        ShaderResourceViewRef createSRView(const SRVDesc& desc, ID3D11Resource* resource);
        RenderTargetViewRef createRTView(const RTVDesc& desc, ID3D11Resource* resource);
        DepthStencilViewRef createDSView(const DSVDesc& desc, ID3D11Resource* resource);
        UnorderedAccessViewRef createUAView(const UAVDesc& desc, ID3D11Resource* resource);

        SamplerStateRef sampler_;
        ShaderResourceViewRef shaderResourceView_;
        pointer_type texture_;
    };

    template<class T>
    ShaderResourceViewRef TextureRefBase<T>::createSRView(const SRVDesc& desc, ID3D11Resource* /*resource*/)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

        if(SRVDesc::copy(viewDesc, desc)){
            ID3D11Device* device = Graphics::getDevice().getD3DDevice();

            ID3D11ShaderResourceView* view = NULL;
            HRESULT hr = device->CreateShaderResourceView(
                texture_,
                &viewDesc,
                &view);

            if(SUCCEEDED(hr)){
                return ShaderResourceViewRef(view);
            }
        }
        return ShaderResourceViewRef();
    }

    template<class T>
    RenderTargetViewRef TextureRefBase<T>::createRTView(const RTVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_RTV_DIMENSION>(desc.dimension_);
        switch(desc.dimension_)
        {
        case ViewRTVDimension_Texture1D:
            viewDesc.Texture1D.MipSlice = desc.tex1D_.mipSlice_;
            break;

        case ViewRTVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MipSlice        = desc.tex1DArray_.mipSlice_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize       = desc.tex1DArray_.arraySize_;
            break;

        case ViewRTVDimension_Texture2D:
            viewDesc.Texture2D.MipSlice = desc.tex2D_.mipSlice_;
            break;

        case ViewRTVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MipSlice        = desc.tex2DArray_.mipSlice_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize       = desc.tex2DArray_.arraySize_;
            break;

        case ViewRTVDimension_Texture3D:
            viewDesc.Texture3D.MipSlice    = desc.tex3D_.mipSlice_;
            viewDesc.Texture3D.FirstWSlice = desc.tex3D_.firstWSlice_;
            viewDesc.Texture3D.WSize       = desc.tex3D_.wSize_;
            break;
        };

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11RenderTargetView* view = NULL;
        HRESULT hr = device->CreateRenderTargetView(
            resource,
            &viewDesc,
            &view);

        return (SUCCEEDED(hr))? RenderTargetViewRef(view) : RenderTargetViewRef();
    }

    template<class T>
    DepthStencilViewRef TextureRefBase<T>::createDSView(const DSVDesc& desc, ID3D11Resource* resource)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
        viewDesc.Format = static_cast<DXGI_FORMAT>(desc.format_);
        viewDesc.ViewDimension = static_cast<D3D11_DSV_DIMENSION>(desc.dimension_);
        viewDesc.Flags = 0;

        switch(desc.dimension_)
        {
        case ViewDSVDimension_Texture1D:
            viewDesc.Texture1D.MipSlice = desc.tex1D_.mipSlice_;
            break;

        case ViewDSVDimension_Texture1DArray:
            viewDesc.Texture1DArray.MipSlice        = desc.tex1DArray_.mipSlice_;
            viewDesc.Texture1DArray.FirstArraySlice = desc.tex1DArray_.firstArraySlice_;
            viewDesc.Texture1DArray.ArraySize       = desc.tex1DArray_.arraySize_;
            break;

        case ViewDSVDimension_Texture2D:
            viewDesc.Texture2D.MipSlice = desc.tex2D_.mipSlice_;
            break;

        case ViewDSVDimension_Texture2DArray:
            viewDesc.Texture2DArray.MipSlice        = desc.tex2DArray_.mipSlice_;
            viewDesc.Texture2DArray.FirstArraySlice = desc.tex2DArray_.firstArraySlice_;
            viewDesc.Texture2DArray.ArraySize       = desc.tex2DArray_.arraySize_;
            break;
        };

        ID3D11Device* device = Graphics::getDevice().getD3DDevice();

        ID3D11DepthStencilView* view = NULL;
        HRESULT hr = device->CreateDepthStencilView(
            resource,
            &viewDesc,
            &view);

        return (SUCCEEDED(hr))? DepthStencilViewRef(view) : DepthStencilViewRef();
    }

    template<class T>
    inline void TextureRefBase<T>::attachVS(ContextRef& context, u32 viewIndex, u32 samplerIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setVSResources(viewIndex, 1, &view);
        context.setVSSamplers(samplerIndex, 1, sampler_.get());
    }

    template<class T>
    inline void TextureRefBase<T>::attachGS(ContextRef& context, u32 viewIndex, u32 samplerIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setGSResources(viewIndex, 1, &view);
        context.setGSSamplers(samplerIndex, 1, sampler_.get());
    }

    template<class T>
    inline void TextureRefBase<T>::attachPS(ContextRef& context, u32 viewIndex, u32 samplerIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setPSResources(viewIndex, 1, &view);
        context.setPSSamplers(samplerIndex, 1, sampler_.get());
    }

    template<class T>
    inline void TextureRefBase<T>::attachCS(ContextRef& context, u32 viewIndex, u32 samplerIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setCSResources(viewIndex, 1, &view);
        context.setCSSamplers(samplerIndex, 1, sampler_.get());
    }

    template<class T>
    inline void TextureRefBase<T>::attachVS(ContextRef& context, u32 viewIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setVSResources(viewIndex, 1, &view);
    }

    template<class T>
    inline void TextureRefBase<T>::attachGS(ContextRef& context, u32 viewIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setGSResources(viewIndex, 1, &view);
    }

    template<class T>
    inline void TextureRefBase<T>::attachPS(ContextRef& context, u32 viewIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setPSResources(viewIndex, 1, &view);
    }

    template<class T>
    inline void TextureRefBase<T>::attachCS(ContextRef& context, u32 viewIndex)
    {
        ID3D11ShaderResourceView* view = shaderResourceView_.getView();
        context.setCSResources(viewIndex, 1, &view);
    }

    template<class T>
    inline void TextureRefBase<T>::copy(ContextRef& context, this_type& src)
    {
        context.copyResource(texture_, src.texture_);
    }

    template<class T>
    inline void TextureRefBase<T>::updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch)
    {
        context.updateSubresource(texture_, index, box, data, rowPitch, depthPitch);
    }

    template<class T>
    inline bool TextureRefBase<T>::map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped)
    {
        return context.map(
            texture_,
            subresource,
            (D3D11_MAP)type,
            reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mapped));

    }

    template<class T>
    inline bool TextureRefBase<T>::map(ContextRef& context, void*& data, u32& rowPitch, u32& depthPitch, u32 subresource, s32 type)
    {
        return context.map(data, rowPitch, depthPitch, texture_, subresource, type);
    }

    template<class T>
    inline void TextureRefBase<T>::unmap(ContextRef& context, u32 subresource)
    {
        context.unmap(texture_, subresource);
    }

    //--------------------------------------------------------
    //---
    //--- Texture1DRef
    //---
    //--------------------------------------------------------
    class Texture1DRef : public TextureRefBase<ID3D11Texture1D>
    {
    public:
        typedef TextureRefBase<ID3D11Texture1D> parent_type;

        Texture1DRef()
        {}

        Texture1DRef(const Texture1DRef& rhs);

        ~Texture1DRef()
        {
            destroy();
        }

        void destroy();

        Texture1DRef& operator=(const Texture1DRef& rhs)
        {
            Texture1DRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void swap(Texture1DRef& rhs)
        {
            parent_type::swap(rhs);
        }
    private:
        friend class Texture;

        explicit Texture1DRef(const SamplerStateRef& sampler, ID3D11ShaderResourceView* view, ID3D11Texture1D* texture)
            :parent_type(sampler, view, texture)
        {}
    };

    //--------------------------------------------------------
    //---
    //--- Texture2DRef
    //---
    //--------------------------------------------------------
    class Texture2DRef : public TextureRefBase<ID3D11Texture2D>
    {
    public:
        typedef TextureRefBase<ID3D11Texture2D> parent_type;

        Texture2DRef()
        {}

        Texture2DRef(const Texture2DRef& rhs);

        ~Texture2DRef()
        {
            destroy();
        }

        void destroy();

        inline void getDesc(Texture2DDesc& desc);

        Texture2DRef& operator=(const Texture2DRef& rhs)
        {
            Texture2DRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void swap(Texture2DRef& rhs)
        {
            parent_type::swap(rhs);
        }
    private:
        friend class Texture;

        explicit Texture2DRef(const SamplerStateRef& sampler, ID3D11ShaderResourceView* view, ID3D11Texture2D* texture)
            :parent_type(sampler, view, texture)
        {}
    };

    inline void Texture2DRef::getDesc(Texture2DDesc& desc)
    {
        LASSERT(valid());
        texture_->GetDesc(&desc);
    }

    //--------------------------------------------------------
    //---
    //--- Texture3DRef
    //---
    //--------------------------------------------------------
    class Texture3DRef : public TextureRefBase<ID3D11Texture3D>
    {
    public:
        typedef TextureRefBase<ID3D11Texture3D> parent_type;

        Texture3DRef()
        {}

        Texture3DRef(const Texture3DRef& rhs);

        ~Texture3DRef()
        {
            destroy();
        }

        void destroy();

        Texture3DRef& operator=(const Texture3DRef& rhs)
        {
            Texture3DRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void swap(Texture3DRef& rhs)
        {
            parent_type::swap(rhs);
        }
    private:
        friend class Texture;

        explicit Texture3DRef(const SamplerStateRef& sampler, ID3D11ShaderResourceView* view, ID3D11Texture3D* texture)
            :parent_type(sampler, view, texture)
        {}
    };


    //--------------------------------------------------------
    //---
    //--- BufferRef
    //---
    //--------------------------------------------------------
    class BufferRef : public TextureRefBase<ID3D11Buffer>
    {
    public:
        typedef TextureRefBase<ID3D11Buffer> parent_type;

        BufferRef()
        {}

        BufferRef(const BufferRef& rhs);

        ~BufferRef()
        {
            destroy();
        }

        void destroy();

        BufferRef& operator=(const BufferRef& rhs)
        {
            BufferRef(rhs).swap(*this);
            return *this;
        }

        void swap(BufferRef& rhs)
        {
            parent_type::swap(rhs);
        }
    private:
        friend class Texture;

        explicit BufferRef(const SamplerStateRef& sampler, ID3D11ShaderResourceView* view, ID3D11Buffer* texture)
            :parent_type(sampler, view, texture)
        {}
    };


    //--------------------------------------------------------
    //---
    //--- Texture
    //---
    //--------------------------------------------------------
    class Texture
    {
    public:
        static Texture1DRef create1D(
            u32 width,
            u32 mipLevels,
            u32 arraySize,
            DataFormat format,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            TextureFilterType filter,
            TextureAddress adress,
            CmpFunc compFunc,
            f32 borderColor,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc);

        static Texture2DRef create2D(
            u32 width,
            u32 height,
            u32 mipLevels,
            u32 arraySize,
            DataFormat format,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            TextureFilterType filter,
            TextureAddress adress,
            CmpFunc compFunc,
            f32 borderColor,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc);

        static Texture2DRef create2D(
            u32 width,
            u32 height,
            u32 mipLevels,
            u32 arraySize,
            DataFormat format,
            u32 samleCount, //MSAAサンプル数
            u32 sampleQuality, //MSAA品質
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            TextureFilterType filter,
            TextureAddress adress,
            CmpFunc compFunc,
            f32 borderColor,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc);

        static Texture3DRef create3D(
            u32 width,
            u32 height,
            u32 depth,
            u32 mipLevels,
            DataFormat format,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            TextureFilterType filter,
            TextureAddress adress,
            CmpFunc compFunc,
            f32 borderColor,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc);
            
        static BufferRef createBuffer(
            u32 size,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            u32 structureByteStride,
            const SubResourceData* initData,
            const SRVDesc* resourceViewDesc);
    };


    void setRenderTargetsAndUAV(
        u32 numViews,
        RenderTargetViewRef* views,
        DepthStencilViewRef* depthStencilView,
        u32 UAVStart,
        u32 numUAVs,
        UnorderedAccessViewRef* uavs,
        const u32* UAVInitCounts);

    void setCSUnorderedAccessViews(
        u32 UAVStart,
        u32 numUAVs,
        UnorderedAccessViewRef* uavs,
        const u32* UAVInitCounts);
}
#endif //INC_LGRAPHICS_DX11_TEXTUREREF_H__
