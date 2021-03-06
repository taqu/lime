#ifndef INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H_
#define INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H_
/**
@file StructuredBufferRef.h
@author t-sakai
@date 2015/12/22 create
*/
#include "BufferRefBase.h"

namespace lgfx
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

        StructuredBufferRef(StructuredBufferRef&& rhs)
            :BufferRefBase(rhs)
        {}

        ~StructuredBufferRef()
        {}

        void attach(ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);

        void attachStreamOut(ContextRef& context, u32 offsetInBytes);

        StructuredBufferRef& operator=(const StructuredBufferRef& rhs);
        StructuredBufferRef& operator=(StructuredBufferRef&& rhs);

        void swap(StructuredBufferRef& rhs);
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

#endif //INC_LGRAPHICS_DX11_STRUCTUREDBUFFERREF_H_
