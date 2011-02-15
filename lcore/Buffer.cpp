/**
@file Buffer.cpp
@author t-sakai
@date 2011/02/08 create

*/
#include "lcore.h"
#include "Buffer.h"

namespace lcore
{
    u32 Buffer::getSize() const
    {
        if(buffer_ == NULL){
            return 0;
        }
        const u8* mem = buffer_ - MemAlignSize;
        return *(reinterpret_cast<const u32*>(mem));
    }


    void Buffer::resize(u32 size)
    {
        u32 currentSize = getSize();
        if(currentSize<size){
            destruct();
            construct(size);
        }
    }

    void Buffer::construct(u32 size)
    {
        LASSERT(buffer_ == NULL);

        //先頭にdouble分のバイトを余分に確保。そこにサイズを収める。
        //TODO: メモリアラインメントがdoubleより大きいとだめだが
        u8* mem = LIME_NEW u8[size + MemAlignSize];
        (*(reinterpret_cast<u32*>(mem))) = size;
        buffer_ = mem + MemAlignSize;
#ifdef _DEBUG
        memset(buffer_, 0, size);
#endif
    }

    void Buffer::destruct()
    {
        if(NULL != buffer_){
            u8* mem = buffer_ - MemAlignSize;
            LIME_DELETE_ARRAY(mem);
            buffer_ = NULL;
        }
    }
}
