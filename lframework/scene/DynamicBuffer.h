#ifndef INC_LSCENE_DYNAMICBUFFER_H__
#define INC_LSCENE_DYNAMICBUFFER_H__
/**
@file DynamicBuffer.h
@author t-sakai
@date 2014/10/16 create
*/
#include "RingBuffer.h"
#include "BufferCreator.h"

namespace lscene
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

        bool map(lgraphics::ContextRef& context, u32 subresource, u32 size, lscene::MappedSubresource& mapped);
        void unmap(lgraphics::ContextRef& context, u32 subresource);

        void attach(lgraphics::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);
        void attach(lgraphics::ContextRef& context, lgraphics::DataFormat format, u32 offsetInBytes);

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
    bool DynamicBuffer<Buffer, Creator>::map(lgraphics::ContextRef& context, u32 subresource, u32 size, lscene::MappedSubresource& mapped)
    {
        RingBuffer::Resource resource = ringBuffer_.allocate(size);
        if(resource.sizeInBytes_<=0){
            return false;
        }
        lgraphics::MappedSubresource m;
        if(buffer_.map(context, subresource, lgraphics::MapType_WriteNoOverwrite, m)){
            mapped.offsetData_ = ((u8*)m.data_ + resource.offsetInBytes_);
            mapped.offsetInBytes_ = resource.offsetInBytes_;
            mapped.sizeInBytes_ = resource.sizeInBytes_;
            return true;
        }
        return false;
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::unmap(lgraphics::ContextRef& context, u32 subresource)
    {
        buffer_.unmap(context, subresource);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::attach(lgraphics::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        context.setVertexBuffer(startSlot, buffer_.get(), stride, offsetInBytes);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::attach(lgraphics::ContextRef& context, lgraphics::DataFormat format, u32 offsetInBytes)
    {
        context.setIndexBuffer(buffer_.get(), format, offsetInBytes);
    }
}
#endif //INC_LSCENE_DYNAMICBUFFER_H__
