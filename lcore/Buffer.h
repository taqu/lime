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
    /**
    @brief �o�b�t�@�Ǘ��N���X
    */
    class Buffer
    {
    public:
        static const u32 MemAlignSize = sizeof(f64);

        inline Buffer();
        inline Buffer(u32 size);

        inline ~Buffer();

        u32 getSize() const;
        void resize(u32 size);

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
        void construct(u32 size);
        void destruct();

        u8 *buffer_;
    };

    inline Buffer::Buffer()
        :buffer_(NULL)
    {
    }

    inline Buffer::Buffer(u32 size)
        :buffer_(NULL)
    {
        construct(size);
    }

    inline Buffer::~Buffer()
    {
        destruct();
    }
}
#endif //INC_LCORE_BUFFER_H__
