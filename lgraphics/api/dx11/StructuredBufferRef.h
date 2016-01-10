#ifndef INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H__
#define INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H__
/**
@file StructuredBufferRef.h
@author t-sakai
@date 2015/12/22 create
*/
#include "BufferRefBase.h"

namespace lgraphics
{
    //------------------------------------------------------------
    //---
    //--- StructuredBufferRef
    //---
    //------------------------------------------------------------
    class StructuredBufferRef : public BufferRefBase
    {
    public:
        StructuredBufferRef()
        {}

        StructuredBufferRef(const StructuredBufferRef& rhs)
            :BufferRefBase(rhs)
        {}

        ~StructuredBufferRef()
        {}

        StructuredBufferRef& operator=(const StructuredBufferRef& rhs)
        {
            StructuredBufferRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void swap(StructuredBufferRef& rhs)
        {
            BufferRefBase::swap(rhs);
        }

        void attach(ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);

        void attachStreamOut(ContextRef& context, u32 offsetInBytes);
    private:
        friend class StructuredBuffer;

        StructuredBufferRef(BufferRefBase::pointer_type buffer)
            :BufferRefBase(buffer)
        {}
    };

    //------------------------------------------------------------
    //---
    //--- StructuredBuffer
    //---
    //------------------------------------------------------------
    class StructuredBuffer : public BufferBase
    {
    public:
        template<class T>
        static StructuredBufferRef create(
            u32 size,
            Usage usage,
            u32 bindFlag,
            CPUAccessFlag accessFlags,
            ResourceMisc miscFlags,
            u32 structureByteStride=sizeof(T),
            const void* data = NULL,
            u32 pitch = 0,
            u32 slicePitch = 0)
        {
            ID3D11Buffer* buffer = BufferBase::create(
                size,
                usage,
                bindFlag,
                accessFlags,
                miscFlags,
                structureByteStride,
                data,
                pitch,
                slicePitch);

            return StructuredBufferRef(buffer);
        }
    };
}

#endif //INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H__
