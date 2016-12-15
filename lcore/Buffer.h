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
    template<class T>
    class Buffer
    {
    public:
        typedef Buffer<T> this_type;
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;

        Buffer();
        Buffer(s32 size);

        ~Buffer();

        inline bool valid() const;

        inline s32 size() const;

        /**
        @brief リサイズ, 古いバッファをコピー.
        */
        void resize(s32 num);

        /**
        @brief リサイズ, 古いバッファをコピーしない.
        */
        void reserve(s32 num);

        inline T* construct(s32 index);

        inline void destruct(s32 index);

        T* construct(s32 index, s32 num);

        void destruct(s32 index, s32 num);

        inline T* get(s32 index);

        inline const T& operator[](s32 index) const;

        inline T& operator[](s32 index);

        inline void swap(this_type& rhs);

        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;
    private:
        Buffer(const this_type&);
        this_type& operator=(const this_type&);

        void allocate(s32 num);

        s32 size_;
        u8 *buffer_;
    };

    template<class T>
    Buffer<T>::Buffer()
        :size_(0)
        ,buffer_(NULL)
    {
    }

    template<class T>
    Buffer<T>::Buffer(s32 size)
        :buffer_(NULL)
    {
        allocate(size);
    }

    template<class T>
    Buffer<T>::~Buffer()
    {
        LDELETE_ARRAY(buffer_);
    }

    template<class T>
    inline bool Buffer<T>::valid() const
    {
        return (NULL != buffer_);
    }

    template<class T>
    inline s32 Buffer<T>::size() const
    {
        return size_;
    }

    template<class T>
    void Buffer<T>::resize(s32 num)
    {
        if(size_<num){
            s32 oldSize = sizeof(T)*size_;
            s32 newSize = sizeof(T)*num;
            u8* buffer = LNEW u8[newSize];
            lcore::memcpy(buffer, buffer_, oldSize);
#ifdef _DEBUG
            lcore::memset(buffer+oldSize, 0, newSize-oldSize);
#endif
            LDELETE_ARRAY(buffer_);
            size_ = num;
            buffer_ = buffer;
        }
    }

    template<class T>
    void Buffer<T>::reserve(s32 num)
    {
        if(size_<num){
            LDELETE_ARRAY(buffer_);
            allocate(num);
        }
    }

    template<class T>
    inline T* Buffer<T>::construct(s32 index)
    {
        LASSERT(NULL != buffer_);
        T* ptr = get(index);
        return LPLACEMENT_NEW(ptr) T();
    }

    template<class T>
    inline void Buffer<T>::destruct(s32 index)
    {
        LASSERT(NULL != buffer_);
        T* ptr = get(index);
        ptr->~T();
    }

    template<class T>
    T* Buffer<T>::construct(s32 index, s32 num)
    {
        LASSERT(NULL != buffer_);
        T* ptr = get(index);
        for(s32 i=0; i<num; ++i){
            LPLACEMENT_NEW(&ptr[i]) T();
        }
        return ptr;
    }

    template<class T>
    void Buffer<T>::destruct(s32 index, s32 num)
    {
        T* ptr = get(index);
        for(s32 i=0; i<num; ++i, ++ptr){
            ptr->~T();
        }
    }

    template<class T>
    inline T* Buffer<T>::get(s32 index)
    {
        return reinterpret_cast<T*>(buffer_) + index;
    }

    template<class T>
    inline const T& Buffer<T>::operator[](s32 index) const
    {
        return reinterpret_cast<T*>(buffer_)[index];
    }

    template<class T>
    inline T& Buffer<T>::operator[](s32 index)
    {
        return reinterpret_cast<T*>(buffer_)[index];
    }

    template<class T>
    inline void Buffer<T>::swap(this_type& rhs)
    {
        lcore::swap(size_, rhs.size_);
        lcore::swap(buffer_, rhs.buffer_);
    }

    template<class T>
    void Buffer<T>::allocate(s32 num)
    {
        LASSERT(NULL==buffer_);
        s32 newSize = sizeof(T)*num;
        size_ = num;
        buffer_ = LNEW u8[newSize];
#ifdef _DEBUG
        lcore::memset(buffer_, 0, newSize);
#endif
    }

    template<class T>
    inline typename Buffer<T>::iterator Buffer<T>::begin()
    {
        return static_cast<iterator>(buffer_);
    }

    template<class T>
    inline typename Buffer<T>::const_iterator Buffer<T>::begin() const
    {
        return static_cast<const_iterator>(buffer_);
    }

    template<class T>
    inline typename Buffer<T>::iterator Buffer<T>::end()
    {
        return static_cast<iterator>(buffer_) + size_;
    }

    template<class T>
    inline typename Buffer<T>::const_iterator Buffer<T>::end() const
    {
        return static_cast<const_iterator>(buffer_) + size_;
    }
}
#endif //INC_LCORE_BUFFER_H__
