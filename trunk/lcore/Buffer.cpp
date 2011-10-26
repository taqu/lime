/**
@file Buffer.cpp
@author t-sakai
@date 2011/02/08 create

*/
#include "lcore.h"
#include "clibrary.h"
#include "Buffer.h"

namespace lcore
{
    void Buffer::resize(u32 size)
    {
        if(size_<size){
            destruct();
            construct(size);
        }
    }

    void Buffer::construct(u32 size)
    {
        LASSERT(buffer_ == NULL);
        size_ = size;
        buffer_ = LIME_NEW u8[size_];
#ifdef _DEBUG
        memset(buffer_, 0, size_);
#endif
    }

    void Buffer::destruct()
    {
        LIME_DELETE_ARRAY(buffer_);
    }
}
