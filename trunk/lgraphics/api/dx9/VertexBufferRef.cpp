/**
@file VertexBufferRef.cpp
@author t-sakai
@date 2009/04/30
@data 2010/01/08 add VertexBufferUP
*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

#include "VertexBufferRef.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- VertexBufferRef
    //---
    //------------------------------------------------------------
    VertexBufferRef::VertexBufferRef(const VertexBufferRef& rhs)
        :vertexSize_(rhs.vertexSize_)
        ,vertexNum_(rhs.vertexNum_)
        ,offset_(rhs.offset_)
        ,buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }

    bool VertexBufferRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);

        D3DVERTEXBUFFER_DESC dxdesc;
        if(FAILED(buffer_->GetDesc(&dxdesc))){
            return false;
        }

        desc.format_ = static_cast<BufferFormat>(dxdesc.Format);
        desc.type_ = static_cast<ResourceType>(dxdesc.Type);
        desc.usage_ = static_cast<DeclUsage>(dxdesc.Usage);
        desc.pool_ = static_cast<Pool>(dxdesc.Pool);
        desc.size_ = dxdesc.Size;
        desc.fvf_ = dxdesc.FVF;
        return true;
    }

    void VertexBufferRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }


    bool VertexBufferRef::lock(u32 offset, u32 size, void** data, Lock lock)
    {
        LASSERT(buffer_ != NULL);
        HRESULT hr = buffer_->Lock(offset, size, data, lock);
        return SUCCEEDED(hr);
    }

    void VertexBufferRef::unlock()
    {
        LASSERT(buffer_ != NULL);
        buffer_->Unlock();
    }

    // データ転送
    void VertexBufferRef::blit(const void* data, u32 offset, u32 size)
    {
        LASSERT((offset+size) <= (vertexSize_*vertexNum_));
        LASSERT(buffer_ != NULL);

        void* dst = NULL;
        HRESULT hr = buffer_->Lock(offset, size, &dst, Lock_None);

        if(SUCCEEDED(hr)){
            lcore::memcpy(dst, data, size);
            buffer_->Unlock();
        }
    }


    void VertexBufferRef::attach(u32 stride) const
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->SetStreamSource(0, const_cast<IDirect3DVertexBuffer9*>(buffer_), offset_, stride);
    }

    void VertexBufferRef::attach(u32 stride, u32 stream) const
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->SetStreamSource(stream, const_cast<IDirect3DVertexBuffer9*>(buffer_), offset_, stride);
    }


    //------------------------------------------------------------
    //---
    //--- VertexBuffer
    //---
    //------------------------------------------------------------
    VertexBufferRef VertexBuffer::create(u32 vertexSize, u32 vertexNum, Pool poolType, Usage usage)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DVertexBuffer9 *buffer = device.createVertexBuffer(
            vertexSize * vertexNum,
            usage,
            0,
            poolType);

        return (buffer != NULL)? VertexBufferRef(vertexSize, vertexNum, buffer) : VertexBufferRef();
    }


    //------------------------------------------------------------
    //---
    //--- VertexBufferUPRef
    //---
    //------------------------------------------------------------
    VertexBufferUPRef::VertexBufferUPRef(const VertexBufferUPRef& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }


    void VertexBufferUPRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }

    bool VertexBufferUPRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);
        desc.format_ = Buffer_Vertex;
        desc.type_ = Resource_VertexBuffer;
        desc.usage_ = 0;
        desc.pool_ = Pool_UserMem;
        desc.size_ = buffer_->getVertexSize() * buffer_->getVertexNum();
        desc.fvf_ = 0;
        return true;
    }

    bool VertexBufferUPRef::lock(void** data)
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    bool VertexBufferUPRef::lock(const void** data) const
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    //------------------------------------------------------------
    //---
    //--- VertexBufferUP
    //---
    //------------------------------------------------------------
    VertexBufferUPRef VertexBufferUP::create(u32 vertexSize, u32 vertexNum)
    {
        u8 *buffer = LIME_NEW u8[vertexSize * vertexNum];
        if(buffer == NULL){
            return VertexBufferUPRef();
        }

        VertexBufferUP *vb = LIME_NEW VertexBufferUP(buffer, vertexSize, vertexNum);
        if(vb == NULL){
            return VertexBufferUPRef();
        }

        vb->AddRef(); // 参照カウントを増やしておく
        return VertexBufferUPRef(vb);
    }

    VertexBufferUP::~VertexBufferUP()
    {
        LIME_DELETE_ARRAY(buffer_);
    }
}
