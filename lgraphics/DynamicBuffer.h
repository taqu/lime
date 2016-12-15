#ifndef INC_LGRAPHICS_DYNAMICBUFFER_H__
#define INC_LGRAPHICS_DYNAMICBUFFER_H__
/**
@file DynamicBuffer.h
@author t-sakai
@date 2016/11/09 create
*/
#include "RingBuffer.h"
#include "BufferCreator.h"

namespace lgfx
{
    //--------------------------------------------------
    //---
    //--- DynamicBuffer
    //---
    //--------------------------------------------------
    template<class Buffer, class Creator>
    class DynamicBuffer
    {
    public:
        typedef Buffer buffer_type;
        typedef Creator creator_type;

        DynamicBuffer();
        ~DynamicBuffer();

        void initialize(s32 totalSize);
        void terminate();

        void begin();
        void end();

        bool map(lgfx::ContextRef& context, u32 subresource, u32 size, lgfx::MappedSubresourceTransientBuffer& mapped);
        void unmap(lgfx::ContextRef& context, u32 subresource);

        void attach(lgfx::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);
        void attach(lgfx::ContextRef& context, lgfx::DataFormat format, u32 offsetInBytes);

        buffer_type& getBuffer(){ return buffer_;}
    private:
        RingBuffer ringBuffer_;
        buffer_type buffer_;
    };

    template<class Buffer, class Creator>
    DynamicBuffer<Buffer, Creator>::DynamicBuffer()
    {
    }

    template<class Buffer, class Creator>
    DynamicBuffer<Buffer, Creator>::~DynamicBuffer()
    {
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::initialize(s32 totalSize)
    {
        ringBuffer_.initialize(totalSize);
        buffer_ = creator_type::create(totalSize);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::terminate()
    {
        buffer_.destroy();
        ringBuffer_.terminate();
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::begin()
    {
        ringBuffer_.begin();
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::end()
    {
        ringBuffer_.end();
    }

    template<class Buffer, class Creator>
    bool DynamicBuffer<Buffer, Creator>::map(lgfx::ContextRef& context, u32 subresource, u32 size, lgfx::MappedSubresourceTransientBuffer& mapped)
    {
        RingBuffer::Resource resource = ringBuffer_.allocate(size);
        if(resource.sizeInBytes_<=0){
            return false;
        }
        lgfx::MappedSubresource m;
        if(buffer_.map(context, subresource, lgfx::MapType_WriteNoOverwrite, m)){
            mapped.offsetData_ = ((u8*)m.data_ + resource.offsetInBytes_);
            mapped.offsetInBytes_ = resource.offsetInBytes_;
            mapped.sizeInBytes_ = resource.sizeInBytes_;
            return true;
        }
        return false;
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::unmap(lgfx::ContextRef& context, u32 subresource)
    {
        buffer_.unmap(context, subresource);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::attach(lgfx::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        context.setVertexBuffer(startSlot, buffer_.get(), stride, offsetInBytes);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::attach(lgfx::ContextRef& context, lgfx::DataFormat format, u32 offsetInBytes)
    {
        context.setIndexBuffer(buffer_.get(), format, offsetInBytes);
    }
}
#endif //INC_LGRAPHICS_DYNAMICBUFFER_H__
