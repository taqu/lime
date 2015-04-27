﻿#ifndef INC_LGRAPHICS_DX11_BUFFERREFBASE_H__
#define INC_LGRAPHICS_DX11_BUFFERREFBASE_H__
/**
@file BufferRefBase.h
@author t-sakai
@date 2012/07/15 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"
#include "ViewRef.h"

struct ID3D11Buffer;

namespace lgraphics
{
    class ContextRef;
    struct Box;
    //------------------------------------------------------------
    //---
    //--- BufferRefBase
    //---
    //------------------------------------------------------------
    class BufferRefBase
    {
    public:
        typedef ID3D11Buffer element_type;
        typedef ID3D11Buffer* pointer_type;

        void destroy();

        pointer_type getBuffer(){ return buffer_;}
        pointer_type const* get(){ return &buffer_;}

        bool map(ContextRef& context, u32 subresource, MapType type, MappedSubresource& mapped);
        void unmap(ContextRef& context, u32 subresource);
        void updateSubresource(ContextRef& context, u32 index, const Box* box, const void* data, u32 rowPitch, u32 depthPitch);

        void copy(ContextRef& context, BufferRefBase& src);

        UnorderedAccessViewRef createUAView(const UAVDesc& desc)
        {
            return View::createUAView(desc, buffer_);
        }

        bool valid() const{ return buffer_ != NULL;}
    protected:
        BufferRefBase()
            :buffer_(NULL)
        {}

        BufferRefBase(const BufferRefBase& rhs);

        explicit BufferRefBase(pointer_type buffer)
            :buffer_(buffer)
        {}

        ~BufferRefBase()
        {
            destroy();
        }

        void swap(BufferRefBase& rhs)
        {
            lcore::swap(buffer_, rhs.buffer_);
        }

        pointer_type buffer_;
    };

    //------------------------------------------------------------
    //---
    //--- BufferBase
    //---
    //------------------------------------------------------------
    class BufferBase
    {
    protected:
        static ID3D11Buffer* create(
            u32 size,
            Usage usage,
            u32 bindFlags,
            CPUAccessFlag accessFlags,
            ResourceMisc miscFlags,
            u32 structureByteStride =0,
            const void* data = NULL,
            u32 pitch =0,
            u32 slicePitch =0);
    };
}
#endif //INC_LGRAPHICS_DX11_BUFFERREFBASE_H__
