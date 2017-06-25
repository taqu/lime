#ifndef INC_LGRAPHICS_DX11_VIEWREF_H__
#define INC_LGRAPHICS_DX11_VIEWREF_H__
/**
@file ViewRef.h
@author t-sakai
@date 2014/10/01 create
*/
#include "lgraphics.h"
#include "Enumerations.h"

struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

namespace lgfx
{
    class ContextRef;

    //--------------------------------------------------------
    struct BufferRTV
    {
        u32 firstElement_;
        u32 numElements_;
    };


    struct Texture1DRTV
    {
        u32 mipSlice_;
    };

    struct Texture1DArrayRTV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture2DRTV
    {
        u32 mipSlice_;
    };

    struct Texture2DArrayRTV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture3DRTV
    {
        u32 mipSlice_;
        u32 firstWSlice_;
        u32 wSize_;
    };

    struct RTVDesc
    {
        DataFormat format_;
        RTVDimension dimension_;
        union
        {
            BufferRTV buffer_;
            Texture1DRTV tex1D_;
            Texture1DArrayRTV tex1DArray_;

            Texture2DRTV tex2D_;
            Texture2DArrayRTV tex2DArray_;

            Texture3DRTV tex3D_;
        };

        static bool copy(D3D11_RENDER_TARGET_VIEW_DESC& viewDesc, const RTVDesc& desc);
    };

    //--------------------------------------------------------
    struct Texture1DDSV
    {
        u32 mipSlice_;
    };

    struct Texture1DArrayDSV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture2DDSV
    {
        u32 mipSlice_;
    };

    struct Texture2DArrayDSV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };


    struct DSVDesc
    {
        DataFormat format_;
        DSVDimension dimension_;
        union
        {
            Texture1DDSV tex1D_;
            Texture1DArrayDSV tex1DArray_;

            Texture2DDSV tex2D_;
            Texture2DArrayDSV tex2DArray_;
        };

        static bool copy(D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc, const DSVDesc& desc);
    };

    //--------------------------------------------------------
    struct BufferSRV
    {
        u32 firstElement_;
        u32 numElements_;
    };

    struct Texture1DSRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
    };

    struct Texture1DArraySRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture2DSRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
    };

    struct Texture2DArraySRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture3DSRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
    };

    struct TextureCubeSRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
    };

    struct TextureCubeArraySRV
    {
        u32 mostDetailedMip_;
        u32 mipLevels_;
        u32 first2DArraySlice_;
        u32 numCubes_;
    };

    struct BufferExSRV
    {
        s32 firstElement_;
        u32 numElements_;
        u32 flags_;
    };

    struct SRVDesc
    {
        DataFormat format_;
        SRVDimension dimension_;
        union
        {
            BufferSRV buffer_;
            Texture1DSRV tex1D_;
            Texture1DArraySRV tex1DArray_;

            Texture2DSRV tex2D_;
            Texture2DArraySRV tex2DArray_;

            Texture3DSRV tex3D_;

            TextureCubeSRV texCube_;
            TextureCubeArraySRV texCubeArray_;

            BufferExSRV bufferEx_;
        };

        static bool copy(D3D11_SHADER_RESOURCE_VIEW_DESC& viewDesc, const SRVDesc& desc);
    };

    //--------------------------------------------------------
    struct BufferUAV
    {
        u32 firstElement_;
        u32 numElements_;
        u32 flags_;
    };

    struct Texture1DUAV
    {
        u32 mipSlice_;
    };

    struct Texture1DArrayUAV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture2DUAV
    {
        u32 mipSlice_;
    };

    struct Texture2DArrayUAV
    {
        u32 mipSlice_;
        u32 firstArraySlice_;
        u32 arraySize_;
    };

    struct Texture3DUAV
    {
        u32 mipSlice_;
        u32 firstWSlice_;
        u32 wsize_;
    };

    struct UAVDesc
    {
        DataFormat format_;
        UAVDimension dimension_;
        union
        {
            BufferUAV buffer_;
            Texture1DUAV tex1D_;
            Texture1DArrayUAV tex1DArray_;

            Texture2DUAV tex2D_;
            Texture2DArrayUAV tex2DArray_;

            Texture3DUAV tex3D_;
        };

        static bool copy(D3D11_UNORDERED_ACCESS_VIEW_DESC& viewDesc, const UAVDesc& desc);
    };

    //--------------------------------------------------------
    struct SubResourceData
    {
        const void* mem_;
        u32 pitch_; //in bytes
        u32 slicePitch_; //in bytes
    };

    //--------------------------------------------------------
    //---
    //--- ViewRefBase
    //---
    //--------------------------------------------------------
    template<class Derived, class T>
    class ViewRefBase
    {
    public:
        typedef ViewRefBase<Derived, T> this_type;
        typedef Derived derived_type;
        typedef T element_type;
        typedef T* pointer_type;
        typedef pointer_type const* pointer_array_type;

        void destroy();

        bool valid() const{ return (NULL != view_);}

        void swap(derived_type& rhs)
        {
            lcore::swap(view_, rhs.view_);
        }

        pointer_type get(){ return view_;}
        operator pointer_type(){ return view_;}
        operator pointer_array_type(){ return &view_; }

    protected:
        inline ViewRefBase();
        ViewRefBase(const this_type& rhs);
        ViewRefBase(this_type&& rhs);
        inline explicit ViewRefBase(pointer_type view);
        inline ~ViewRefBase();

        pointer_type view_;
    };

    template<class Derived, class T>
    inline ViewRefBase<Derived, T>::ViewRefBase()
        :view_(NULL)
    {
    }

    template<class Derived, class T>
    ViewRefBase<Derived, T>::ViewRefBase(const this_type& rhs)
        :view_(rhs.view_)
    {
        if(view_){
            view_->AddRef();
        }
    }

    template<class Derived, class T>
    ViewRefBase<Derived, T>::ViewRefBase(this_type&& rhs)
        :view_(rhs.view_)
    {
        rhs.view_ = NULL;
    }

    template<class Derived, class T>
    inline ViewRefBase<Derived, T>::ViewRefBase(pointer_type view)
        :view_(view)
    {}

    template<class Derived, class T>
    inline ViewRefBase<Derived, T>::~ViewRefBase()
    {
        destroy();
    }

    template<class Derived, class T>
    void ViewRefBase<Derived, T>::destroy()
    {
        LDXSAFE_RELEASE(view_);
    }

    //--------------------------------------------------------
    //---
    //--- ViewRef
    //---
    //--------------------------------------------------------
    class ShaderResourceViewRef;
    class RenderTargetViewRef;
    class DepthStencilViewRef;
    class UnorderedAccessViewRef;

    class ViewRef : public ViewRefBase<ViewRef, ID3D11View>
    {
    public:
        typedef ViewRef this_type;
        typedef ViewRefBase<ViewRef, ID3D11View> parent_type;
        typedef ID3D11View element_type;
        typedef ID3D11View* pointer_type;
        typedef pointer_type const* pointer_array_type;

        ViewRef()
        {}

        ViewRef(const ViewRef& rhs)
            :parent_type(rhs)
        {}
        ViewRef(ViewRef&& rhs)
            :parent_type(rhs)
        {}

        explicit ViewRef(pointer_type view)
            :parent_type(view)
        {}

        ~ViewRef()
        {}

        ViewRef& operator=(const ViewRef& rhs);
        ViewRef& operator=(ViewRef&& rhs);

        ViewRef& operator=(ShaderResourceViewRef&& rhs);
        ViewRef& operator=(RenderTargetViewRef&& rhs);
        ViewRef& operator=(DepthStencilViewRef&& rhs);
        ViewRef& operator=(UnorderedAccessViewRef&& rhs);

        inline operator ID3D11ShaderResourceView*();
        inline operator ID3D11RenderTargetView*();
        inline operator ID3D11DepthStencilView*();
        inline operator ID3D11UnorderedAccessView*();

        inline bool operator==(const this_type& rhs) const
        {
            return (view_ == rhs.view_);
        }

        inline bool operator!=(const this_type& rhs) const
        {
            return (view_ != rhs.view_);
        }
    };

    inline ViewRef::operator ID3D11ShaderResourceView*()
    {
        return reinterpret_cast<ID3D11ShaderResourceView*>(view_);
    }

    inline ViewRef::operator ID3D11RenderTargetView*()
    {
        return reinterpret_cast<ID3D11RenderTargetView*>(view_);
    }

    inline ViewRef::operator ID3D11DepthStencilView*()
    {
        return reinterpret_cast<ID3D11DepthStencilView*>(view_);
    }

    inline ViewRef::operator ID3D11UnorderedAccessView*()
    {
        return reinterpret_cast<ID3D11UnorderedAccessView*>(view_);
    }

    //--------------------------------------------------------
    //---
    //--- ShaderResourceViewRef
    //---
    //--------------------------------------------------------
    class ShaderResourceViewRef : public ViewRefBase<ShaderResourceViewRef, ID3D11ShaderResourceView>
    {
    public:
        typedef ShaderResourceViewRef this_type;
        typedef ViewRefBase<ShaderResourceViewRef, ID3D11ShaderResourceView> parent_type;
        typedef ID3D11ShaderResourceView element_type;
        typedef ID3D11ShaderResourceView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        friend class ViewRef;

        ShaderResourceViewRef()
        {}

        ShaderResourceViewRef(const this_type& rhs)
            :parent_type(rhs)
        {}

        ShaderResourceViewRef(this_type&& rhs)
            :parent_type(rhs)
        {}

        explicit ShaderResourceViewRef(pointer_type view)
            :parent_type(view)
        {}

        ~ShaderResourceViewRef()
        {}

        ShaderResourceViewRef& operator=(const ShaderResourceViewRef& rhs);
        ShaderResourceViewRef& operator=(ShaderResourceViewRef&& rhs);

        void attachVS(lgfx::ContextRef& context, u32 index);
        void attachGS(lgfx::ContextRef& context, u32 index);
        void attachPS(lgfx::ContextRef& context, u32 index);
        void attachCS(lgfx::ContextRef& context, u32 index);
        void attachHS(lgfx::ContextRef& context, u32 index);
        void attachDS(lgfx::ContextRef& context, u32 index);

        inline bool operator==(const ShaderResourceViewRef& rhs) const
        {
            return (view_ == rhs.view_);
        }

        inline bool operator!=(const ShaderResourceViewRef& rhs) const
        {
            return (view_ != rhs.view_);
        }
    };

    //--------------------------------------------------------
    //---
    //--- RenderTargetViewRef
    //---
    //--------------------------------------------------------
    class RenderTargetViewRef : public ViewRefBase<RenderTargetViewRef, ID3D11RenderTargetView>
    {
    public:
        typedef RenderTargetViewRef this_type;
        typedef ViewRefBase<RenderTargetViewRef, ID3D11RenderTargetView> parent_type;
        typedef ID3D11RenderTargetView element_type;
        typedef ID3D11RenderTargetView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        friend class ViewRef;

        RenderTargetViewRef()
            :parent_type(NULL)
        {}

        RenderTargetViewRef(const RenderTargetViewRef& rhs)
            :parent_type(rhs)
        {}

        RenderTargetViewRef(RenderTargetViewRef&& rhs)
            :parent_type(rhs)
        {}

        explicit RenderTargetViewRef(pointer_type view)
            :parent_type(view)
        {}

        ~RenderTargetViewRef()
        {}

        RenderTargetViewRef& operator=(const RenderTargetViewRef& rhs);
        RenderTargetViewRef& operator=(RenderTargetViewRef&& rhs);

        inline bool operator==(const RenderTargetViewRef& rhs) const
        {
            return (view_ == rhs.view_);
        }

        inline bool operator!=(const RenderTargetViewRef& rhs) const
        {
            return (view_ != rhs.view_);
        }
    };

    //--------------------------------------------------------
    //---
    //--- DepthStencilViewRef
    //---
    //--------------------------------------------------------
    class DepthStencilViewRef : public ViewRefBase<DepthStencilViewRef, ID3D11DepthStencilView>
    {
    public:
        typedef DepthStencilViewRef this_type;
        typedef ViewRefBase<DepthStencilViewRef, ID3D11DepthStencilView> parent_type;
        typedef ID3D11DepthStencilView element_type;
        typedef ID3D11DepthStencilView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        friend class ViewRef;

        DepthStencilViewRef()
        {}

        DepthStencilViewRef(const DepthStencilViewRef& rhs)
            :parent_type(rhs)
        {}

        DepthStencilViewRef(DepthStencilViewRef&& rhs)
            :parent_type(rhs)
        {}

        explicit DepthStencilViewRef(pointer_type view)
            :parent_type(view)
        {}

        ~DepthStencilViewRef()
        {}

        DepthStencilViewRef& operator=(const DepthStencilViewRef& rhs);
        DepthStencilViewRef& operator=(DepthStencilViewRef&& rhs);

        inline bool operator==(const DepthStencilViewRef& rhs) const
        {
            return (view_ == rhs.view_);
        }

        inline bool operator!=(const DepthStencilViewRef& rhs) const
        {
            return (view_ != rhs.view_);
        }
    };

    //--------------------------------------------------------
    //---
    //--- UnorderedAccessViewRef
    //---
    //--------------------------------------------------------
    class UnorderedAccessViewRef : public ViewRefBase<UnorderedAccessViewRef, ID3D11UnorderedAccessView>
    {
    public:
        typedef UnorderedAccessViewRef this_type;
        typedef ViewRefBase<UnorderedAccessViewRef, ID3D11UnorderedAccessView> parent_type;
        typedef ID3D11UnorderedAccessView element_type;
        typedef ID3D11UnorderedAccessView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        friend class ViewRef;

        UnorderedAccessViewRef()
        {}

        UnorderedAccessViewRef(const UnorderedAccessViewRef& rhs)
            :parent_type(rhs)
        {}

        UnorderedAccessViewRef(UnorderedAccessViewRef&& rhs)
            :parent_type(rhs)
        {}

        explicit UnorderedAccessViewRef(pointer_type view)
            :parent_type(view)
        {}

        ~UnorderedAccessViewRef()
        {}

        UnorderedAccessViewRef& operator=(const UnorderedAccessViewRef& rhs);
        UnorderedAccessViewRef& operator=(UnorderedAccessViewRef&& rhs);

        inline bool operator==(const UnorderedAccessViewRef& rhs) const
        {
            return (view_ == rhs.view_);
        }

        inline bool operator!=(const UnorderedAccessViewRef& rhs) const
        {
            return (view_ != rhs.view_);
        }
    };

    struct View
    {
        static RenderTargetViewRef View::createRTView(const RTVDesc& desc, ID3D11Resource* resource);
        static DepthStencilViewRef View::createDSView(const DSVDesc& desc, ID3D11Resource* resource);
        static ShaderResourceViewRef View::createSRView(const SRVDesc& desc, ID3D11Resource* resource);
        static UnorderedAccessViewRef View::createUAView(const UAVDesc& desc, ID3D11Resource* resource);
    };
}
#endif //INC_LGRAPHICS_DX11_VIEWREF_H__
