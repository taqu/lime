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
    //----------------------------------------------------------------------
    /**
    @brief バッファ管理クラス
    */
    class Buffer
    {
    public:
        inline Buffer();
        inline Buffer(u32 size);

        inline ~Buffer();

        inline bool valid() const;

        u32 size() const{ return size_;}
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
        T* allocate(u32 offset, u32 num);

        template<class T>
        void destruct(u32 offset, u32 num);

        template<class T>
        void destruct(T* t, u32 num);

        template<class T>
        T* get(u32 offset)
        {
            return reinterpret_cast<T*>(buffer_ + offset);
        }

        void swap(Buffer& rhs)
        {
            lcore::swap(size_, rhs.size_);
            lcore::swap(buffer_, rhs.buffer_);
        }

        void destruct();
    private:
        void construct(u32 size);

        u32 size_;
        u8 *buffer_;
    };

    inline Buffer::Buffer()
        :size_(0)
        ,buffer_(NULL)
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

    inline bool Buffer::valid() const
    {
        return (NULL != buffer_);
    }

    template<class T>
    T* Buffer::allocate(u32 offset, u32 num)
    {
        LASSERT(buffer_ != NULL);

        u8* ret = buffer_ + offset;
        u8* t = ret;
        for(u32 i=0; i<num; ++i){
            LIME_PLACEMENT_NEW(t) T();
            t += sizeof(T);
        }
        return reinterpret_cast<T*>(ret);
    }

    template<class T>
    void Buffer::destruct(u32 offset, u32 num)
    {
        T* t = reinterpret_cast<T*>(&buffer_[offset]);
        for(u32 i=0; i<num; ++i, ++t){
            t->~T();
        }
    }

    template<class T>
    void Buffer::destruct(T* t, u32 num)
    {
        LASSERT(t != NULL);
        for(u32 i=0; i<num; ++i, ++t){
            t->~T();
        }
    }

#if 0
    //----------------------------------------------------------------------
    /**
    @brief バッファ管理クラス
    */
    template<class T, s32 INC_SIZE>
    class ObjectBuffer
    {
    public:
        static const s32 IncSize = INC_SIZE;

        ObjectBuffer();
        ~ObjectBuffer();

        inline s32 capacity() const;
        inline s32 size() const;
        inline void clear();

        void resize(s32 size);
        inline void expand()
        {
            resize(capacity_+IncSize);
        }
        //void push_back(const T& t);

        inline T& operator[](s32 index);
        inline const T& operator[](s32 index) const;
    private:
        ObjectBuffer(const ObjectBuffer&);
        ObjectBuffer& operator=(const ObjectBuffer&);

        s32 capacity_;
        s32 size_;
        T* buffer_;
    };

    template<class T, s32 INC_SIZE>
    ObjectBuffer<T, INC_SIZE>::ObjectBuffer()
        :capacity_(0)
        ,size_(0)
        ,buffer_(NULL)
    {
    }

    template<class T, s32 INC_SIZE>
    ObjectBuffer<T, INC_SIZE>::~ObjectBuffer()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

    template<class T, s32 INC_SIZE>
    inline s32 ObjectBuffer<T, INC_SIZE>::capacity() const
    {
        return capacity_;
    }

    template<class T, s32 INC_SIZE>
    inline s32 ObjectBuffer<T, INC_SIZE>::size() const
    {
        return size_;
    }

    template<class T, s32 INC_SIZE>
    inline void ObjectBuffer<T, INC_SIZE>::clear()
    {
        size_ = 0;
    }

    template<class T, s32 INC_SIZE>
    void ObjectBuffer<T, INC_SIZE>::resize(s32 size)
    {
        if(capacity_<size){
            capacity_ = size;
            T* buffer = LIME_NEW T[capacity_];
            for(s32 i=0; i<size_; ++i){
                buffer[i] = buffer_[i];
            }
            LIME_DELETE_ARRAY(buffer_);
            buffer_ = buffer;
        }
        size_ = size;
    }

    template<class T, s32 INC_SIZE>
    inline T& ObjectBuffer<T, INC_SIZE>::operator[](s32 index)
    {
        LASSERT(0<=index && index<size_);
        return buffer_[index];
    }

    template<class T, s32 INC_SIZE>
    inline const T& ObjectBuffer<T, INC_SIZE>::operator[](s32 index) const
    {
        LASSERT(0<=index && index<size_);
        return buffer_[index];
    }

    //template<class T, s32 INC_SIZE>
    //void ObjectBuffer<T, INC_SIZE>::push_back(const T& t)
    //{
    //    if(capacity_<=size_){
    //        capacity_ += IncSize;
    //        T* buffer = LIME_NEW T[capacity_];
    //        for(s32 i=0; i<size_; ++i){
    //            buffer[i] = buffer_[i];
    //        }
    //        LIME_DELETE_ARRAY(buffer_);
    //        buffer_ = buffer;
    //    }
    //    buffer_[size_] = t;
    //    ++size_;
    //}
#endif
}
#endif //INC_LCORE_BUFFER_H__
