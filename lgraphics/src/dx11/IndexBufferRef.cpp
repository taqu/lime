/**
@file IndexBufferRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "IndexBufferRef.h"

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgfx
{
    IndexBufferRef& IndexBufferRef::operator=(const IndexBufferRef& rhs)
    {
        IndexBufferRef(rhs).swap(*this);
        return *this;
    }

    IndexBufferRef& IndexBufferRef::operator=(IndexBufferRef&& rhs)
    {
        if(this != &rhs){
            destroy();
            buffer_ = rhs.buffer_;
            rhs.buffer_ = NULL;
        }
        return *this;
    }

    void IndexBufferRef::attach(ContextRef& context, DataFormat format, u32 offsetInBytes)
    {
        context.setIndexBuffer(buffer_, format, offsetInBytes);
    }
}
