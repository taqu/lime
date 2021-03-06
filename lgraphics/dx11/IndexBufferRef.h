﻿#ifndef INC_LGRAPHICS_DX11_INDEXBUFFERREF_H_
#define INC_LGRAPHICS_DX11_INDEXBUFFERREF_H_
/**
@file IndexBufferRef.h
@author t-sakai
@date 2012/07/15 create
*/
#include "BufferRefBase.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- IndexBufferRef
    //---
    //------------------------------------------------------------
    class IndexBufferRef : public BufferRefBase
    {
    public:
        IndexBufferRef()
        {}

        IndexBufferRef(const IndexBufferRef& rhs)
            :BufferRefBase(rhs)
        {}

        IndexBufferRef(IndexBufferRef&& rhs)
            :BufferRefBase(rhs)
        {}

        ~IndexBufferRef()
        {}

        IndexBufferRef& operator=(const IndexBufferRef& rhs);
        IndexBufferRef& operator=(IndexBufferRef&& rhs);

        void swap(IndexBufferRef& rhs)
        {
            BufferRefBase::swap(rhs);
        }

        void attach(ContextRef& context, DataFormat format, u32 offsetInBytes);
    private:
        friend class IndexBuffer;

        IndexBufferRef(BufferRefBase::pointer_type buffer)
            :BufferRefBase(buffer)
        {}
    };

    //------------------------------------------------------------
    //---
    //--- IndexBuffer
    //---
    //------------------------------------------------------------
    class IndexBuffer : public BufferBase
    {
    public:
        static IndexBufferRef create(
            u32 size,
            Usage usage,
            CPUAccessFlag accessFlags,
            ResourceMisc miscFlags,
            u32 structureByteStride =0,
            const void* data = NULL)
        {
            ID3D11Buffer* buffer = BufferBase::create(
                size,
                usage,
                BindFlag_IndexBuffer,
                accessFlags,
                miscFlags,
                structureByteStride,
                data);

            return IndexBufferRef(buffer);
        }
    };
}
#endif //INC_LGRAPHICS_DX11_INDEXBUFFERREF_H_
