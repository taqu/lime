#ifndef INC_LGRAPHICS_DYNAMICBUFFER_H_
#define INC_LGRAPHICS_DYNAMICBUFFER_H_
/**
@file DynamicBuffer.h
@author t-sakai
@date 2016/11/09 create
*/
#include "NBuffer.h"
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

        void initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize);
        void terminate();

        void begin();
        void end();

        bool map(lgfx::ContextRef& context, u32 subresource, u32 size, lgfx::MappedSubresourceTransientBuffer& mapped);
        void unmap(lgfx::ContextRef& context, u32 subresource);

        void attach(lgfx::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);
        void attach(lgfx::ContextRef& context, lgfx::DataFormat format, u32 offsetInBytes);

        buffer_type& getBuffer(){ return buffer_;}
    private:
        NBuffer nbuffer_;
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
    void DynamicBuffer<Buffer, Creator>::initialize(FrameSyncQuery* frameSyncQuery, s32 totalSize)
    {
        nbuffer_.initialize(frameSyncQuery, totalSize, frameSyncQuery->getFrames());
        buffer_ = creator_type::create(totalSize);
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::terminate()
    {
        buffer_.destroy();
        nbuffer_.terminate();
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::begin()
    {
        nbuffer_.begin();
    }

    template<class Buffer, class Creator>
    void DynamicBuffer<Buffer, Creator>::end()
    {
        nbuffer_.end();
    }

    template<class Buffer, class Creator>
    bool DynamicBuffer<Buffer, Creator>::map(lgfx::ContextRef& context, u32 subresource, u32 size, lgfx::MappedSubresourceTransientBuffer& mapped)
    {
        NBuffer::Resource resource = nbuffer_.allocate(size);
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
#endif //INC_LGRAPHICS_DYNAMICBUFFER_H_
