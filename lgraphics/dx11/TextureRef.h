#ifndef INC_LGRAPHICS_DX11_TEXTUREREF_H__
#define INC_LGRAPHICS_DX11_TEXTUREREF_H__
/**
@file TextureRef.h
@author t-sakai
@date 2012/07/24 create
*/
#include "lgraphics.h"
#include "GraphicsDeviceRef.h"
#include "Enumerations.h"
#include "SamplerStateRef.h"
#include "ViewRef.h"

struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct ID3D11Buffer;

namespace lgfx
{
    class ContextRef;

    typedef D3D11_TEXTURE2D_DESC Texture2DDesc;

    //--------------------------------------------------------
    //---
    //--- ResourceRefBase
    //---
    //--------------------------------------------------------
    template<class Derived, class T>
    class ResourceRefBase
    {
    public:
        typedef ResourceRefBase<Derived, T> this_type;
        typedef Derived derived_type;
        typedef T element_type;
        typedef T* pointer_type;
        typedef pointer_type const* pointer_array_type;

        pointer_type get() { return resource_;}
        operator pointer_type(){ return resource_;}
        operator pointer_array_type(){ return &resource_;}

        bool valid() const{ return (NULL != resource_);}

        void swap(derived_type& rhs)
        {
            lcore::swap(resource_, rhs.resource_);
        }

        inline ShaderResourceViewRef createSRView(const SRVDesc& desc);
        inline RenderTargetViewRef createRTView(const RTVDesc& desc);
        inline DepthStencilViewRef createDSView(const DSVDesc& desc);
        inline UnorderedAccessViewRef createUAView(const UAVDesc& desc);

        inline void copy(ContextRef& context, this_type& src);
        inline void updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch);

        inline bool map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped);
        inline bool map(ContextRef& context, void*& data, u32& rowPitch, u32& depthPitch, u32 subresource, s32 type);
        inline void unmap(ContextRef& context, u32 subresource);

        inline bool operator==(const this_type& rhs) const
        {
            return (resource_ == rhs.resource_);
        }

        inline bool operator!=(const this_type& rhs) const
        {
            return (resource_ != rhs.resource_);
        }
    protected:
        ResourceRefBase& operator=(const ResourceRefBase&) = delete;

        ResourceRefBase()
            :resource_(NULL)
        {}

        ResourceRefBase(const this_type& rhs)
            :resource_(rhs.resource_)
        {
            if(resource_){
                resource_->AddRef();
            }
        }

        ResourceRefBase(this_type&& rhs)
            :resource_(rhs.resource_)
        {
            rhs.resource_ = NULL;
        }

        explicit ResourceRefBase(pointer_type resource)
            :resource_(resource)
        {}

        ~ResourceRefBase()
        {
            destroy();
        }

        void destroy()
        {
            LDXSAFE_RELEASE(resource_);
        }

        pointer_type resource_;
    };

    template<class Derived, class T>
    inline ShaderResourceViewRef ResourceRefBase<Derived, T>::createSRView(const SRVDesc& desc)
    {
        return View::createSRView(desc, resource_);
    }

    template<class Derived, class T>
    inline RenderTargetViewRef ResourceRefBase<Derived, T>::createRTView(const RTVDesc& desc)
    {
        return View::createRTView(desc, resource_);
    }

    template<class Derived, class T>
    inline DepthStencilViewRef ResourceRefBase<Derived, T>::createDSView(const DSVDesc& desc)
    {
        return View::createDSView(desc, resource_);
    }

    template<class Derived, class T>
    inline UnorderedAccessViewRef ResourceRefBase<Derived, T>::createUAView(const UAVDesc& desc)
    {
        return View::createUAView(desc, resource_);
    }

    template<class Derived, class T>
    inline void ResourceRefBase<Derived, T>::copy(ContextRef& context, this_type& src)
    {
        context.copyResource(resource_, src.resource_);
    }

    template<class Derived, class T>
    inline void ResourceRefBase<Derived, T>::updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch)
    {
        context.updateSubresource(resource_, index, box, data, rowPitch, depthPitch);
    }

    template<class Derived, class T>
    inline bool ResourceRefBase<Derived, T>::map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped)
    {
        return context.map(
            resource_,
            subresource,
            type,
            mapped);

    }

    template<class Derived, class T>
    inline bool ResourceRefBase<Derived, T>::map(ContextRef& context, void*& data, u32& rowPitch, u32& depthPitch, u32 subresource, s32 type)
    {
        return context.map(data, rowPitch, depthPitch, resource_, subresource, type);
    }

    template<class Derived, class T>
    inline void ResourceRefBase<Derived, T>::unmap(ContextRef& context, u32 subresource)
    {
        context.unmap(resource_, subresource);
    }

    //--------------------------------------------------------
    //---
    //--- ResourceRef
    //---
    //--------------------------------------------------------
    class Texture1DRef;
    class Texture2DRef;
    class Texture3DRef;
    class BufferRef;

    class ResourceRef : public ResourceRefBase<ResourceRef, ID3D11Resource>
    {
    public:
        typedef ResourceRefBase<ResourceRef, ID3D11Resource> parent_type;
        typedef ID3D11Resource element_type;
        typedef ID3D11Resource* pointer_type;

        ResourceRef()
        {}

        ResourceRef(const ResourceRef& rhs)
            :parent_type(rhs)
        {}

        ResourceRef(ResourceRef&& rhs)
            :parent_type(rhs)
        {}

        ~ResourceRef()
        {}

        ResourceRef& operator=(const ResourceRef& rhs);
        ResourceRef& operator=(ResourceRef&& rhs);

        ResourceRef& operator=(Texture1DRef&& rhs);
        ResourceRef& operator=(Texture2DRef&& rhs);
        ResourceRef& operator=(Texture3DRef&& rhs);
        ResourceRef& operator=(BufferRef&& rhs);
    private:
        explicit ResourceRef(ID3D11Resource* texture)
            :parent_type(texture)
        {}
    };

    //--------------------------------------------------------
    //---
    //--- Texture1DRef
    //---
    //--------------------------------------------------------
    class Texture1DRef : public ResourceRefBase<Texture1DRef, ID3D11Texture1D>
    {
    public:
        typedef ResourceRefBase<Texture1DRef, ID3D11Texture1D> parent_type;
        typedef ID3D11Texture1D element_type;
        typedef ID3D11Texture1D* pointer_type;

        Texture1DRef()
        {}

        Texture1DRef(const Texture1DRef& rhs)
            :parent_type(rhs)
        {}

        Texture1DRef(Texture1DRef&& rhs)
            :parent_type(rhs)
        {}

        ~Texture1DRef()
        {}

        Texture1DRef& operator=(const Texture1DRef& rhs);
        Texture1DRef& operator=(Texture1DRef&& rhs);

    private:
        friend class ResourceRef;
        friend class Texture;

        explicit Texture1DRef(ID3D11Texture1D* texture)
            :parent_type(texture)
        {}
    };

    //--------------------------------------------------------
    //---
    //--- Texture2DRef
    //---
    //--------------------------------------------------------
    class Texture2DRef : public ResourceRefBase<Texture2DRef, ID3D11Texture2D>
    {
    public:
        typedef ResourceRefBase<Texture2DRef, ID3D11Texture2D> parent_type;
        typedef ID3D11Texture2D element_type;
        typedef ID3D11Texture2D* pointer_type;

        Texture2DRef()
        {}

        Texture2DRef(const Texture2DRef& rhs)
            :parent_type(rhs)
        {}

        Texture2DRef(Texture2DRef&& rhs)
            :parent_type(rhs)
        {}

        ~Texture2DRef()
        {}

        inline void getDesc(Texture2DDesc& desc);

        Texture2DRef& operator=(const Texture2DRef& rhs);
        Texture2DRef& operator=(Texture2DRef&& rhs);

    private:
        friend class ResourceRef;
        friend class Texture;

        explicit Texture2DRef(ID3D11Texture2D* texture)
            :parent_type(texture)
        {}
    };

    inline void Texture2DRef::getDesc(Texture2DDesc& desc)
    {
        LASSERT(valid());
        resource_->GetDesc(&desc);
    }

    //--------------------------------------------------------
    //---
    //--- Texture3DRef
    //---
    //--------------------------------------------------------
    class Texture3DRef : public ResourceRefBase<Texture3DRef, ID3D11Texture3D>
    {
    public:
        typedef ResourceRefBase<Texture3DRef, ID3D11Texture3D> parent_type;
        typedef ID3D11Texture3D element_type;
        typedef ID3D11Texture3D* pointer_type;

        Texture3DRef()
        {}

        Texture3DRef(const Texture3DRef& rhs)
            :parent_type(rhs)
        {}

        Texture3DRef(Texture3DRef&& rhs)
            :parent_type(rhs)
        {}

        ~Texture3DRef()
        {}

        Texture3DRef& operator=(const Texture3DRef& rhs);
        Texture3DRef& operator=(Texture3DRef&& rhs);

    private:
        friend class ResourceRef;
        friend class Texture;

        explicit Texture3DRef(ID3D11Texture3D* texture)
            :parent_type(texture)
        {}
    };


    //--------------------------------------------------------
    //---
    //--- BufferRef
    //---
    //--------------------------------------------------------
    class BufferRef : public ResourceRefBase<BufferRef, ID3D11Buffer>
    {
    public:
        typedef ResourceRefBase<BufferRef, ID3D11Buffer> parent_type;
        typedef ID3D11Buffer element_type;
        typedef ID3D11Buffer* pointer_type;

        BufferRef()
        {}

        BufferRef(const BufferRef& rhs)
            :parent_type(rhs)
        {}

        BufferRef(BufferRef&& rhs)
            :parent_type(rhs)
        {}

        ~BufferRef()
        {}

        BufferRef& operator=(const BufferRef& rhs);
        BufferRef& operator=(BufferRef&& rhs);
    private:
        friend class ResourceRef;
        friend class Texture;

        explicit BufferRef(ID3D11Buffer* buffer)
            :parent_type(buffer)
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
            const SubResourceData* initData);

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
            const SubResourceData* initData);

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
            const SubResourceData* initData);

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
            const SubResourceData* initData);
            
        static BufferRef createBuffer(
            u32 size,
            Usage usage,
            u32 bind,
            CPUAccessFlag access,
            ResourceMisc misc,
            u32 structureByteStride,
            const SubResourceData* initData);
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
