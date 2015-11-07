#ifndef INC_LRENDER_BUFFER_H__
#define INC_LRENDER_BUFFER_H__
/**
@file Buffer.h
@author t-sakai
@date 2015/09/11 create
*/
#include "../lrender.h"

namespace lrender
{
    template<class T>
    class Buffer
    {
    public:
        typedef T element_type;
        static bool create(Buffer<T>& buffer, s32 num);

        Buffer();
        ~Buffer();

        inline s32 size() const;
        inline const T& operator[](s32 index) const;
        inline T& operator[](s32 index);

        void swap(Buffer<T>& rhs);
    private:
        Buffer(const Buffer&);
        const Buffer& operator=(const Buffer&);

        Buffer(u32 size, T* values);

        s32 size_;
        T* values_;
    };

    template<class T>
    bool Buffer<T>::create(Buffer<T>& buffer, s32 num)
    {
        T* values = LIME_NEW T[num];
        if(NULL == values){
            return false;
        }

        Buffer<T> ret(num, values);
        buffer.swap(ret);
        return true;
    }

    template<class T>
    Buffer<T>::Buffer()
        :size_(0)
        ,values_(NULL)
    {
    }

    template<class T>
    Buffer<T>::~Buffer()
    {
        LIME_DELETE_ARRAY(values_);
    }

    template<class T>
    Buffer<T>::Buffer(u32 size, T* values)
        :size_(size)
        ,values_(values)
    {
    }

    template<class T>
    inline s32 Buffer<T>::size() const
    {
        return size_;
    }

    template<class T>
    inline const T& Buffer<T>::operator[](s32 index) const
    {
        return values_[index];
    }

    template<class T>
    inline T& Buffer<T>::operator[](s32 index)
    {
        return values_[index];
    }

    template<class T>
    void Buffer<T>::swap(Buffer<T>& rhs)
    {
        lcore::swap(size_, rhs.size_);
        lcore::swap(values_, rhs.values_);
    }

    typedef Buffer<lmath::Vector2> BufferVector2;
    typedef Buffer<lmath::Vector3> BufferVector3;
    typedef Buffer<lmath::Vector4> BufferVector4;
}
#endif //INC_LRENDER_BUFFER_H__
