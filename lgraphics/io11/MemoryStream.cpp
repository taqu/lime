/**
@file MemoryStream.cpp
@author t-sakai
@date 2012/07/26 create
*/
#include "MemoryStream.h"

namespace lgraphics
{
namespace io
{
    void MemoryStream::read(void* dst, u32 size)
    {
        u32 count = getReadable(size);

        lcore::memcpy(dst, data_+current_, count);
        current_ += count;
    }
}
}
