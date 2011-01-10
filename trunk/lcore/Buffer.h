#ifndef INC_LCORE_BUFFER_H__
#define INC_LCORE_BUFFER_H__
/**
@file Buffer.h
@author t-sakai
@date 2009/09/12 create
*/
#include "lcore.h"

namespace lcore
{
    class Buffer
    {
    public:
        inline Buffer();
        inline Buffer(u32 size);

        inline ~Buffer();

        inline void reset(u32 size);

        template<class T>
        T* allocate(u32 offset)
        {
            LASSERT(buffer_ != NULL);
            return LIME_PLACEMENT_NEW(&buffer_[offset]) T();
        }

        template<class T>
        void destruct(u32 offset)
        {
            T* t = reinterpret_cast<T*>(&buffer_[offset]);
            t->~T();
        }

        template<class T>
        T* allocate(u32 offset, u32 num)
        {
            LASSERT(buffer_ != NULL);
            return LIME_PLACEMENT_NEW(&buffer_[offset]) T[num];
        }

        template<class T>
        void destruct(u32 offset, u32 num)
        {
            T* t = reinterpret_cast<T*>(&buffer_[offset]);
            for(u32 i=0; i<num; ++i, ++t){
                t->~T();
            }
        }

        template<class T>
        void destruct(T* t, u32 num)
        {
            LASSERT(t != NULL);
            for(u32 i=0; i<num; ++i, ++t){
                t->~T();
            }
        }

        template<class T>
        T* get(u32 offset)
        {
            return reinterpret_cast<T*>(buffer_ + offset);
        }

        void swap(Buffer& rhs)
        {
            lcore::swap(buffer_, rhs.buffer_);
        }
    private:
        u8 *buffer_;
    };

    inline Buffer::Buffer()
        :buffer_(NULL)
    {
    }

    inline Buffer::Buffer(u32 size)
    {
        buffer_ = LIME_NEW u8[size];
#ifdef _DEBUG
        memset(buffer_, 0, size);
#endif
    }

    inline Buffer::~Buffer()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

    inline void Buffer::reset(u32 size)
    {
        LIME_DELETE_ARRAY(buffer_);
        buffer_ = LIME_NEW u8[size];
#ifdef _DEBUG
        memset(buffer_, 0, size);
#endif
    }
}
#endif //INC_LCORE_BUFFER_H__
