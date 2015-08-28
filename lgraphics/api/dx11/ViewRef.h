#ifndef INC_LGRAPHICS_DX11_VIEWREF_H__
#define INC_LGRAPHICS_DX11_VIEWREF_H__
/**
@file ViewRef.h
@author t-sakai
@date 2014/10/01 create
*/
#include "../../lgraphics.h"
#include "Enumerations.h"

struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

namespace lgraphics
{
    class ContextRef;

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
        ViewRTVDimension dimension_;
        union
        {
            Texture1DRTV tex1D_;
            Texture1DArrayRTV tex1DArray_;

            Texture2DRTV tex2D_;
            Texture2DArrayRTV tex2DArray_;

            Texture3DRTV tex3D_;
        };
    };


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
        ViewDSVDimension dimension_;
        union
        {
            Texture1DDSV tex1D_;
            Texture1DArrayDSV tex1DArray_;

            Texture2DDSV tex2D_;
            Texture2DArrayDSV tex2DArray_;
        };
    };

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

    struct SRVDesc
    {
        DataFormat format_;
        ViewSRVDimension dimension_;
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
        };

        static bool copy(D3D11_SHADER_RESOURCE_VIEW_DESC& viewDesc, const SRVDesc& desc);
    };

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
    };

    struct SubResourceData
    {
        const void* mem_;
        u32 pitch_; //in bytes
        u32 slicePitch_; //in bytes
    };

    //--------------------------------------------------------
    //---
    //--- ShaderResourceViewRef
    //---
    //--------------------------------------------------------
    class ShaderResourceViewRef
    {
    public:
        typedef ID3D11ShaderResourceView element_type;
        typedef ID3D11ShaderResourceView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        ShaderResourceViewRef()
            :view_(NULL)
        {}

        ShaderResourceViewRef(const ShaderResourceViewRef& rhs);

        explicit ShaderResourceViewRef(pointer_type view)
            :view_(view)
        {}

        ~ShaderResourceViewRef()
        {
            destroy();
        }

        ShaderResourceViewRef& operator=(const ShaderResourceViewRef& rhs)
        {
            ShaderResourceViewRef(rhs).swap(*this);
            return *this;
        }

        void destroy();

        bool valid() const{ return (NULL != view_);}

        void swap(ShaderResourceViewRef& rhs)
        {
            lcore::swap(view_, rhs.view_);
        }

        pointer_type get(){ return view_;}
        operator pointer_type(){ return view_;}
        operator pointer_array_type(){ return &view_; }

        void attachVS(lgraphics::ContextRef& context, u32 index);
        void attachGS(lgraphics::ContextRef& context, u32 index);
        void attachPS(lgraphics::ContextRef& context, u32 index);
        void attachCS(lgraphics::ContextRef& context, u32 index);
        void attachHS(lgraphics::ContextRef& context, u32 index);
        void attachDS(lgraphics::ContextRef& context, u32 index);
    private:
        pointer_type view_;
    };

    //--------------------------------------------------------
    //---
    //--- RenderTargetViewRef
    //---
    //--------------------------------------------------------
    class RenderTargetViewRef
    {
    public:
        typedef ID3D11RenderTargetView element_type;
        typedef ID3D11RenderTargetView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        RenderTargetViewRef()
            :view_(NULL)
        {}

        RenderTargetViewRef(const RenderTargetViewRef& rhs);

        explicit RenderTargetViewRef(pointer_type view)
            :view_(view)
        {}

        ~RenderTargetViewRef()
        {
            destroy();
        }

        RenderTargetViewRef& operator=(const RenderTargetViewRef& rhs)
        {
            RenderTargetViewRef(rhs).swap(*this);
            return *this;
        }

        void destroy();

        bool valid() const{ return (NULL != view_);}

        void swap(RenderTargetViewRef& rhs)
        {
            lcore::swap(view_, rhs.view_);
        }

        pointer_type get(){ return view_;}
        operator pointer_type(){ return view_;}
        operator pointer_array_type(){ return &view_; }

        ShaderResourceViewRef createSRView(const SRVDesc& desc);
    private:
        pointer_type view_;
    };

    //--------------------------------------------------------
    //---
    //--- DepthStencilViewRef
    //---
    //--------------------------------------------------------
    class DepthStencilViewRef
    {
    public:
        typedef ID3D11DepthStencilView element_type;
        typedef ID3D11DepthStencilView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        DepthStencilViewRef()
            :view_(NULL)
        {}

        DepthStencilViewRef(const DepthStencilViewRef& rhs);

        explicit DepthStencilViewRef(pointer_type view)
            :view_(view)
        {}

        ~DepthStencilViewRef()
        {
            destroy();
        }

        DepthStencilViewRef& operator=(const DepthStencilViewRef& rhs)
        {
            DepthStencilViewRef(rhs).swap(*this);
            return *this;
        }

        void destroy();

        bool valid() const{ return (NULL != view_);}

        void swap(DepthStencilViewRef& rhs)
        {
            lcore::swap(view_, rhs.view_);
        }

        pointer_type get(){ return view_;}
        operator pointer_type(){ return view_;}
        operator pointer_array_type(){ return &view_; }

    private:
        pointer_type view_;
    };

    //--------------------------------------------------------
    //---
    //--- UnorderedAccessViewRef
    //---
    //--------------------------------------------------------
    class UnorderedAccessViewRef
    {
    public:
        typedef ID3D11UnorderedAccessView element_type;
        typedef ID3D11UnorderedAccessView* pointer_type;
        typedef pointer_type const* pointer_array_type;

        UnorderedAccessViewRef()
            :view_(NULL)
        {}

        UnorderedAccessViewRef(const UnorderedAccessViewRef& rhs);

        explicit UnorderedAccessViewRef(pointer_type view)
            :view_(view)
        {}

        ~UnorderedAccessViewRef()
        {
            destroy();
        }

        UnorderedAccessViewRef& operator=(const UnorderedAccessViewRef& rhs)
        {
            UnorderedAccessViewRef(rhs).swap(*this);
            return *this;
        }

        void destroy();

        bool valid() const{ return (NULL != view_);}

        void swap(UnorderedAccessViewRef& rhs)
        {
            lcore::swap(view_, rhs.view_);
        }

        pointer_type get(){ return view_;}
        operator pointer_type(){ return view_;}
        operator pointer_array_type(){ return &view_; }
    private:
        pointer_type view_;
    };

    struct View
    {
        static UnorderedAccessViewRef View::createUAView(const UAVDesc& desc, ID3D11Resource* resource);
    };
}
#endif //INC_LGRAPHICS_DX11_VIEWREF_H__
