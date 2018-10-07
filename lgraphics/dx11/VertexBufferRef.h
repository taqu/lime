#ifndef INC_LGRAPHICS_DX11_VERTEXBUFFERREF_H_
#define INC_LGRAPHICS_DX11_VERTEXBUFFERREF_H_
/**
@file VertexBufferRef.h
@author t-sakai
@date 2012/07/15 create
*/
#include "BufferRefBase.h"

namespace lgfx
{
    //------------------------------------------------------------
    //---
    //--- VertexBufferRef
    //---
    //------------------------------------------------------------
    class VertexBufferRef : public BufferRefBase
    {
    public:
        VertexBufferRef()
        {}

        VertexBufferRef(const VertexBufferRef& rhs)
            :BufferRefBase(rhs)
        {}

        VertexBufferRef(VertexBufferRef&& rhs)
            :BufferRefBase(rhs)
        {}

        ~VertexBufferRef()
        {}

        void attach(ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);

        void attachStreamOut(ContextRef& context, u32 offsetInBytes);

        VertexBufferRef& operator=(const VertexBufferRef& rhs);
        VertexBufferRef& operator=(VertexBufferRef&& rhs);

        void swap(VertexBufferRef& rhs);
    private:
        friend class VertexBuffer;

        VertexBufferRef(BufferRefBase::pointer_type buffer)
            :BufferRefBase(buffer)
        {}
    };

    //------------------------------------------------------------
    //---
    //--- VertexBuffer
    //---
    //------------------------------------------------------------
    class VertexBuffer : public BufferBase
    {
    public:
        static VertexBufferRef create(
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
                BindFlag_VertexBuffer,
                accessFlags,
                miscFlags,
                structureByteStride,
                data);

            return VertexBufferRef(buffer);
        }

        static VertexBufferRef create(
            u32 size,
            Usage usage,
            u32 bindFlag,
            CPUAccessFlag accessFlags,
            ResourceMisc miscFlags,
            u32 structureByteStride,
            const void* data,
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

            return VertexBufferRef(buffer);
        }
    };
}
#endif //INC_LGRAPHICS_DX11_VERTEXBUFFERREF_H_
