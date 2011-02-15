/**
@file IndexBufferRef.cpp
@author t-sakai
@date 2009/05/07 create
@data 2010/01/08 add IndexBufferUPRef
*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

#include "IndexBufferRef.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- IndexBufferRef
    //---
    //------------------------------------------------------------
    IndexBufferRef::IndexBufferRef(const IndexBufferRef& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }

    void IndexBufferRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }

    bool IndexBufferRef::lock(u32 offset, u32 size, void** data, Lock lock)
    {
        LASSERT(buffer_ != NULL);
        HRESULT hr = buffer_->Lock(offset, size, data, lock);
        return SUCCEEDED(hr);
    }

    void IndexBufferRef::unlock()
    {
        LASSERT(buffer_ != NULL);
        buffer_->Unlock();
    }

    // データ転送
    void IndexBufferRef::blit(const void* data, u32 offset, u32 size)
    {
        LASSERT(buffer_ != NULL);

        void* dst = NULL;
        HRESULT hr = buffer_->Lock(offset, size, &dst, Lock_None);

        if(SUCCEEDED(hr)){
            lcore::memcpy(dst, data, size);
            buffer_->Unlock();
        }
    }

    void IndexBufferRef::attach() const
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        device.setIndices( const_cast<IDirect3DIndexBuffer9*>(buffer_) );
    }

    bool IndexBufferRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);
        HRESULT hr = buffer_->GetDesc((D3DINDEXBUFFER_DESC*)&desc);
        return SUCCEEDED(hr);
    }


    //------------------------------------------------------------
    //---
    //--- IndexBuffer
    //---
    //------------------------------------------------------------
    IndexBufferRef IndexBuffer::create(u32 numIndices, Pool poolType, Usage usage)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DIndexBuffer9 *buffer = device.createIndexBuffer(
            numIndices * sizeof(u16),
            usage,
            Buffer_Index16,
            poolType);

        return (buffer != NULL)? IndexBufferRef(buffer) : IndexBufferRef();
    }



    //------------------------------------------------------------
    //---
    //--- IndexBufferUPRef
    //---
    //------------------------------------------------------------
    IndexBufferUPRef::IndexBufferUPRef(const IndexBufferUPRef& rhs)
        :buffer_(rhs.buffer_)
    {
        if(buffer_ != NULL){
            buffer_->AddRef();
        }
    }


    void IndexBufferUPRef::destroy()
    {
        SAFE_RELEASE(buffer_);
    }

    bool IndexBufferUPRef::getDesc(BufferDesc& desc)
    {
        LASSERT(buffer_ != NULL);
        desc.format_ = Buffer_Index16;
        desc.type_ = Resource_IndexBuffer;
        desc.usage_ = 0;
        desc.pool_ = Pool_UserMem;
        desc.size_ = sizeof(u16) * 3 * buffer_->getNumFaces();
        return true;
    }

    bool IndexBufferUPRef::lock(void** data)
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }

    bool IndexBufferUPRef::lock(const void** data) const
    {
        LASSERT(buffer_ != NULL);
        return buffer_->lock(data);
    }


    //------------------------------------------------------------
    //---
    //--- IndexBufferUP
    //---
    //------------------------------------------------------------
    IndexBufferUPRef IndexBufferUP::create(u32 numFaces)
    {
        u8 *buffer = LIME_NEW u8[numFaces * 3 * sizeof(u16)];
        if(buffer == NULL){
            return IndexBufferUPRef();
        }

        IndexBufferUP *ib = LIME_NEW IndexBufferUP(buffer, numFaces);
        if(ib == NULL){
            return IndexBufferUPRef();
        }

        ib->AddRef(); // 参照カウントを増やしておく
        return IndexBufferUPRef(ib);
    }

    IndexBufferUP::~IndexBufferUP()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

}
