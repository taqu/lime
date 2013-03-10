/**
@file IndexBufferRef.cpp
@author t-sakai
@date 2012/07/15 create
*/
#include "IndexBufferRef.h"

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    void IndexBufferRef::attach(DataFormat format, u32 offset)
    {
        lgraphics::Graphics::getDevice().setIndexBuffer(buffer_, format, offset);
    }
}
