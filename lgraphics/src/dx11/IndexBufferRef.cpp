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
    void IndexBufferRef::attach(ContextRef& context, DataFormat format, u32 offsetInBytes)
    {
        context.setIndexBuffer(buffer_, format, offsetInBytes);
    }
}
