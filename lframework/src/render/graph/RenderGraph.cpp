/**
@file RenderGraph.cpp
@author t-sakai
@date 2017/06/21 create
*/
#include "render/graph/RenderGraph.h"
#include <lcore/xxHash.h>

namespace lfw
{
namespace graph
{
namespace
{
    bool operator==(const lgfx::SRVDesc& x0, const lgfx::SRVDesc& x1)
    {
        if(x0.format_ != x1.format_
            || x0.dimension_ != x1.dimension_)
        {
            return false;
        }
        switch(x0.dimension_)
        {
        case lgfx::SRVDimension_Buffer:
            return 0 == lcore::memcmp(&x0.buffer_, &x1.buffer_, sizeof(lgfx::BufferSRV));
        case lgfx::SRVDimension_Texture1D:
            return 0 == lcore::memcmp(&x0.tex1D_, &x1.tex1D_, sizeof(lgfx::Texture1DSRV));
        case lgfx::SRVDimension_Texture1DArray:
            return 0 == lcore::memcmp(&x0.tex1DArray_, &x1.tex1DArray_, sizeof(lgfx::Texture1DArraySRV));
        case lgfx::SRVDimension_Texture2D:
            return 0 == lcore::memcmp(&x0.tex2D_, &x1.tex2D_, sizeof(lgfx::Texture2DSRV));
        case lgfx::SRVDimension_Texture2DArray:
            return 0 == lcore::memcmp(&x0.tex2DArray_, &x1.tex2DArray_, sizeof(lgfx::Texture2DArraySRV));
        case lgfx::SRVDimension_Texture3D:
            return 0 == lcore::memcmp(&x0.tex3D_, &x1.tex3D_, sizeof(lgfx::Texture3DSRV));
        case lgfx::SRVDimension_Cube:
            return 0 == lcore::memcmp(&x0.texCube_, &x1.texCube_, sizeof(lgfx::TextureCubeSRV));
        case lgfx::SRVDimension_CubeArray:
            return 0 == lcore::memcmp(&x0.texCubeArray_, &x1.texCubeArray_, sizeof(lgfx::TextureCubeArraySRV));
        case lgfx::SRVDimension_BufferEx:
            return 0 == lcore::memcmp(&x0.bufferEx_, &x1.bufferEx_, sizeof(lgfx::BufferExSRV));
        }
        return false;
    }

    bool operator==(const lgfx::RTVDesc& x0, const lgfx::RTVDesc& x1)
    {
        if(x0.format_ != x1.format_
            || x0.dimension_ != x1.dimension_)
        {
            return false;
        }
        switch(x0.dimension_)
        {
        case lgfx::RTVDimension_Buffer:
            return 0 == lcore::memcmp(&x0.buffer_, &x1.buffer_, sizeof(lgfx::BufferRTV));
        case lgfx::RTVDimension_Texture1D:
            return 0 == lcore::memcmp(&x0.tex1D_, &x1.tex1D_, sizeof(lgfx::Texture1DRTV));
        case lgfx::RTVDimension_Texture1DArray:
            return 0 == lcore::memcmp(&x0.tex1DArray_, &x1.tex1DArray_, sizeof(lgfx::Texture1DArrayRTV));
        case lgfx::RTVDimension_Texture2D:
            return 0 == lcore::memcmp(&x0.tex2D_, &x1.tex2D_, sizeof(lgfx::Texture2DRTV));
        case lgfx::RTVDimension_Texture2DArray:
            return 0 == lcore::memcmp(&x0.tex2DArray_, &x1.tex2DArray_, sizeof(lgfx::Texture2DArrayRTV));
        case lgfx::RTVDimension_Texture3D:
            return 0 == lcore::memcmp(&x0.tex3D_, &x1.tex3D_, sizeof(lgfx::Texture3DRTV));
        }
        return false;
    }

    bool operator==(const lgfx::DSVDesc& x0, const lgfx::DSVDesc& x1)
    {
        if(x0.format_ != x1.format_
            || x0.dimension_ != x1.dimension_)
        {
            return false;
        }
        switch(x0.dimension_)
        {
        case lgfx::DSVDimension_Texture1D:
            return 0 == lcore::memcmp(&x0.tex1D_, &x1.tex1D_, sizeof(lgfx::Texture1DDSV));
        case lgfx::DSVDimension_Texture1DArray:
            return 0 == lcore::memcmp(&x0.tex1DArray_, &x1.tex1DArray_, sizeof(lgfx::Texture1DArrayDSV));
        case lgfx::DSVDimension_Texture2D:
            return 0 == lcore::memcmp(&x0.tex2D_, &x1.tex2D_, sizeof(lgfx::Texture2DDSV));
        case lgfx::DSVDimension_Texture2DArray:
            return 0 == lcore::memcmp(&x0.tex2DArray_, &x1.tex2DArray_, sizeof(lgfx::Texture2DArrayDSV));
        }
        return false;
    }

    bool operator==(const lgfx::UAVDesc& x0, const lgfx::UAVDesc& x1)
    {
        if(x0.format_ != x1.format_
            || x0.dimension_ != x1.dimension_)
        {
            return false;
        }
        switch(x0.dimension_)
        {
        case lgfx::UAVDimension_Buffer:
            return 0 == lcore::memcmp(&x0.buffer_, &x1.buffer_, sizeof(lgfx::BufferUAV));
        case lgfx::UAVDimension_Texture1D:
            return 0 == lcore::memcmp(&x0.tex1D_, &x1.tex1D_, sizeof(lgfx::Texture1DUAV));
        case lgfx::UAVDimension_Texture1DArray:
            return 0 == lcore::memcmp(&x0.tex1DArray_, &x1.tex1DArray_, sizeof(lgfx::Texture1DArrayUAV));
        case lgfx::UAVDimension_Texture2D:
            return 0 == lcore::memcmp(&x0.tex2D_, &x1.tex2D_, sizeof(lgfx::Texture2DUAV));
        case lgfx::UAVDimension_Texture2DArray:
            return 0 == lcore::memcmp(&x0.tex2DArray_, &x1.tex2DArray_, sizeof(lgfx::Texture2DArrayUAV));
        case lgfx::UAVDimension_Texture3D:
            return 0 == lcore::memcmp(&x0.tex3D_, &x1.tex3D_, sizeof(lgfx::Texture3DUAV));
        }
        return false;
    }
}

    //------------------------------------------------------
    //---
    //--- ResourceParam
    //---
    //------------------------------------------------------
    u32 ResourceParam::calcHash() const
    {
        return lcore::xxHash32(reinterpret_cast<const u8*>(this), sizeof(ResourceParam));
    }

    bool operator==(const ResourceParam& x0, const ResourceParam& x1)
    {
        return 0 == lcore::memcmp(&x0, &x1, sizeof(ResourceParam));
    }

    ResourceParam ResourceParam::create2D(
        u16 width,
        u16 height,
        u16 mipLevels,
        u16 arraySize,
        u32 format,
        u32 bind,
        u32 misc)
    {
        return
        {
            ResourceType_2D,
            1,
            static_cast<u16>(width),
            static_cast<u16>(height),
            static_cast<u16>(mipLevels),
            static_cast<u16>(arraySize),
            format,
            bind,
            0,
            misc,
        };
    }

    ResourceParam ResourceParam::create3D(
        u16 width,
        u16 height,
        u16 depth,
        u16 mipLevels,
        u16 arraySize,
        u32 format,
        u32 bind,
        u32 misc)
    {
        return
        {
            ResourceType_3D,
            static_cast<u16>(depth),
            static_cast<u16>(width),
            static_cast<u16>(height),
            static_cast<u16>(mipLevels),
            static_cast<u16>(arraySize),
            format,
            bind,
            0,
            misc,
        };
    }

    ResourceParam ResourceParam::createBuffer(
        u32 size,
        u32 bind,
        u32 structureByteStride,
        u32 misc)
    {
        return
        {
            ResourceType_Buffer,
            1,
            static_cast<u16>(size&0xFFFFU),
            static_cast<u16>((size>>16)&0xFFFFU),
            1,
            1,
            lgfx::Data_R8_UInt,
            bind,
            structureByteStride,
            misc,
        };
    }

    //ResourceParam ResourceParam::createDepthStencil(
    //    u32 width,
    //    u32 height,
    //    u32 format,
    //    u32 bind,
    //    u32 misc)
    //{
    //    return
    //    {
    //        ResourceType_DepthStencil,
    //        1,
    //        static_cast<u16>(width),
    //        static_cast<u16>(height),
    //        1,
    //        1,
    //        format,
    //        bind,
    //        0,
    //        misc,
    //    };
    //}

    //------------------------------------------------------
    //---
    //--- ViewParam
    //---
    //------------------------------------------------------
    u32 ViewParam::calcHash() const
    {
        s32 size = sizeof(s32);
        switch(type_)
        {
        case ViewType_SRV:
            size += sizeof(lgfx::SRVDesc);
            break;
        case ViewType_RTV:
            size += sizeof(lgfx::RTVDesc);
            break;
        case ViewType_DSV:
            size += sizeof(lgfx::DSVDesc);
            break;
        case ViewType_UAV:
            size += sizeof(lgfx::UAVDesc);
            break;
        }
        return lcore::xxHash32(reinterpret_cast<const u8*>(this), size);
    }

    bool operator==(const ViewParam& x0, const ViewParam& x1)
    {
        if(x0.type_ != x1.type_){
            return false;
        }
        switch(x0.type_)
        {
        case ViewType_SRV:
            return x0.srv_ == x1.srv_;
        case ViewType_RTV:
            return x0.rtv_ == x1.rtv_;
        case ViewType_DSV:
            return x0.dsv_ == x1.dsv_;
        case ViewType_UAV:
            return x0.uav_ == x1.uav_;
        }
        return false;
    }

    //---------------------------------------------------------
    //---
    //--- RenderGraph
    //---
    //---------------------------------------------------------
    u32 RenderGraph::hashID(const Char* name)
    {
        return lcore::xxHash32(reinterpret_cast<const u8*>(name), lcore::strlen_s32(name));
    }

    RenderGraph::RenderGraph()
    {
    }

    RenderGraph::RenderGraph(RenderGraph&& rhs)
        :resourceMap_(lcore::move(rhs.resourceMap_))
        ,viewMap_(lcore::move(rhs.viewMap_))
    {
    }

    RenderGraph::~RenderGraph()
    {
    }

    void RenderGraph::clear()
    {
        resourceMap_.clear();
        viewMap_.clear();
    }

    void RenderGraph::clearShared()
    {
        viewMap_.clear();
    }

    bool RenderGraph::getTemporary(lgfx::ViewRef& view, const ResourceParam& resParam, const ViewParam& viewParam)
    {
        u32 resId = resParam.calcHash();
        s16 pos = resourceMap_.find(resId);
        if(pos != resourceMap_.end()){
            Resource& resource = resourceMap_.get(pos);
            LASSERT(resource.resource_.valid());

            for(s32 i=0; i<resource.numViews_; ++i){
                if(viewParam == resource.viewParams_[i]){
                    view = resource.views_[i];
                    LASSERT(view.valid());
                    return true;
                }
            }
            if(resource.numViews_ < MaxViews
                && createView(resource.views_[resource.numViews_], resource.resource_, viewParam))
            {
                view = resource.views_[resource.numViews_];
                resource.viewParams_[resource.numViews_] = viewParam;
                ++resource.numViews_;
                return true;
            }
            return false;
        }

        //無かったので追加
        pos = resourceMap_.add(resId);
        Resource& resource = resourceMap_.get(pos);
        LASSERT(!resource.resource_.valid());
        resource.clear();
        resource.param_ = resParam;
        if(view.valid()){
            //すでにあるリソース取得
            resource.resource_ = lgfx::ResourceRef::getResource(view);
            if(!resource.resource_.valid()){
                resourceMap_.remove(resId);
                return false;
            }
            resource.views_[resource.numViews_] = view;
        } else{
            //新しく作成する
            if(!createResource(resource.resource_, resParam)){
                resourceMap_.remove(resId);
                return false;
            }
            if(!createView(resource.views_[resource.numViews_], resource.resource_, viewParam)){
                resourceMap_.remove(resId);
                return false;
            }
            view = resource.views_[resource.numViews_];
        }
        resource.viewParams_[resource.numViews_] = viewParam;
        ++resource.numViews_;
        return true;
    }

    bool RenderGraph::setShared(u32 id, lgfx::ViewRef& view)
    {
        s16 pos = viewMap_.add(id);
        viewMap_.get(pos) = view;
        return true;
    }

    bool RenderGraph::setShared(u32 id, lgfx::ViewRef&& view)
    {
        s16 pos = viewMap_.add(id);
        viewMap_.get(pos) = lcore::move(view);
        return true;
    }

    bool RenderGraph::existShared(u32 id)
    {
        return viewMap_.end() != viewMap_.find(id);
    }

    lgfx::ViewRef RenderGraph::getShared(u32 id)
    {
        s16 pos = viewMap_.find(id);
        return pos != viewMap_.end()? viewMap_.get(pos) : lgfx::ViewRef();
    }

    RenderGraph& RenderGraph::operator=(RenderGraph&& rhs)
    {
        resourceMap_ = lcore::move(rhs.resourceMap_);
        viewMap_ = lcore::move(rhs.viewMap_);
        return *this;
    }

    bool RenderGraph::createResource(lgfx::ResourceRef& resource, const ResourceParam& resParam)
    {
        switch(resParam.type_)
        {
        case ResourceType_2D:
            resource = lgfx::Texture::create2D(
                resParam.width_,
                resParam.height_,
                resParam.mipLevels_,
                resParam.arraySize_,
                (lgfx::DataFormat)resParam.format_,
                lgfx::Usage_Default,
                resParam.bind_,
                lgfx::CPUAccessFlag_None,
                (lgfx::ResourceMisc)resParam.misc_,
                NULL);
            break;

        case ResourceType_3D:
            resource = lgfx::Texture::create3D(
                resParam.width_,
                resParam.height_,
                resParam.depth_,
                resParam.mipLevels_,
                (lgfx::DataFormat)resParam.format_,
                lgfx::Usage_Default,
                resParam.bind_,
                lgfx::CPUAccessFlag_None,
                (lgfx::ResourceMisc)resParam.misc_,
                NULL);
            break;

        case ResourceType_Buffer:
            resource = lgfx::Texture::createBuffer(
                resParam.width_ | (static_cast<u32>(resParam.height_)<<16),
                lgfx::Usage_Default,
                resParam.bind_,
                lgfx::CPUAccessFlag_None,
                (lgfx::ResourceMisc)resParam.misc_,
                resParam.structureByteStride_,
                NULL);
            break;
        default:
            resource = lgfx::ResourceRef();
            break;
        }
        return resource.valid();
    }

    bool RenderGraph::createView(lgfx::ViewRef& view, lgfx::ResourceRef& resource, const ViewParam& viewParam)
    {
        switch(viewParam.type_)
        {
        case ViewType_SRV:
            view = resource.createSRView(viewParam.srv_);
            break;
        case ViewType_RTV:
            view = resource.createRTView(viewParam.rtv_);
            break;
        case ViewType_DSV:
            view = resource.createDSView(viewParam.dsv_);
            break;
        case ViewType_UAV:
            view = resource.createUAView(viewParam.uav_);
            break;
        default:
            view = lgfx::ViewRef();
            break;
        }
        return view.valid();
    }
}
}
