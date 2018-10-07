#ifndef INC_LCORE_ARRAY_H_
#define INC_LCORE_ARRAY_H_
/**
@file Array.h
@author t-sakai
@date 2017/12/15 create
*/
#include "lcore.h"

namespace lcore
{
    template<s32 size=16>
    struct ArrayStaticCapacityIncrement
    {
        static const s32 IncrementSize = size;

        static s32 getInitCapacity(s32 capacity)
        {
            return (capacity + (IncrementSize-1)) & ~(IncrementSize-1);
        }

        static s32 getNewCapacity(s32 capacity)
        {
            return capacity + IncrementSize;
        }
    };

    //-------------------------------------------------------
    //---
    //---
    //---
    //-------------------------------------------------------
    template<class T, class CapacityIncrement, class isTriviallyCopyable>
    class ArrayBuffer
    {
    protected:
        typedef ArrayBuffer<T, CapacityIncrement, isTriviallyCopyable> this_type;
        typedef T value_type;
        typedef s32 size_type;
        typedef CapacityIncrement capacity_increment_type;

        ArrayBuffer(const this_type&) = delete;
        this_type& operator=(const this_type&) = delete;

        ArrayBuffer()
            :capacity_(0)
            ,size_(0)
            ,items_(NULL)
        {}

        ArrayBuffer(this_type&& rhs)
            :capacity_(rhs.capacity_)
            ,size_(rhs.size_)
            ,items_(rhs.items_)
        {
            rhs.capacity_ = 0;
            rhs.size_ = 0;
            rhs.items_ = NULL;
        }

        explicit ArrayBuffer(size_type capacity)
            :capacity_(capacity_increment_type::getInitCapacity(capacity))
            ,size_(0)
            ,items_(NULL)
        {
            LASSERT(0<=capacity_);
            items_ = reinterpret_cast<T*>(LMALLOC(capacity_*sizeof(T)));
        }

        ~ArrayBuffer()
        {
            for(s32 i = 0; i<size_; ++i){
                items_[i].~value_type();
            }
            LFREE(items_);
        }

        void helper_push_back(const value_type& t)
        {
            if(capacity_<=size_){
                helper_reserve(capacity_increment_type::getNewCapacity(capacity_));
            }
            LPLACEMENT_NEW(&items_[size_]) value_type(t);
            ++size_;
        }
        void helper_push_back(value_type&& t)
        {
            if(capacity_<=size_){
                helper_reserve(capacity_increment_type::getNewCapacity(capacity_));
            }
            LPLACEMENT_NEW(&items_[size_]) value_type(move(t));
            ++size_;
        }

        void helper_pop_back()
        {
            ACC_ASSERT(0<size_);
            --size_;
            items_[size_].~value_type();
        }

        void helper_clear()
        {
            for(s32 i = 0; i<size_; ++i){
                items_[i].~value_type();
            }
            size_ = 0;
        }

        void helper_reserve(size_type capacity)
        {
            if(capacity<=capacity_){
                return;
            }

            capacity = capacity_increment_type::getInitCapacity(capacity);
            value_type* newItems = reinterpret_cast<value_type*>(LMALLOC(capacity*sizeof(value_type)));

            //Copy, and destruct
            for(s32 i = 0; i<size_; ++i){
                LPLACEMENT_NEW(&newItems[i]) value_type(move(items_[i]));
                items_[i].~value_type();
            }

            LFREE(items_);

            items_ = newItems;
            capacity_ = capacity;
        }

        void helper_resize(size_type size)
        {
            if(size < size_){
            //Destruct redundant items
                for(s32 i = size; i<size_; ++i){
                    items_[i].~value_type();
                }

            } else{
                //Construct new items by default constructor
                helper_reserve(size);
                for(s32 i = size_; i<size; ++i){
                    LPLACEMENT_NEW(&items_[i]) value_type;
                }
            }
            size_ = size;
        }

        void helper_removeAt(s32 index)
        {
            LASSERT(0<=index && index<size_);
            for(s32 i = index+1; i<size_; ++i){
                items_[i-1] = move(items_[i]);
            }
            --size_;
            items_[size_].~value_type();
        }

        size_type capacity_;
        size_type size_;
        value_type* items_;
    };


    template<class T, class CapacityIncrement>
    class ArrayBuffer<T, CapacityIncrement, true_type>
    {
    protected:
        typedef ArrayBuffer<T, CapacityIncrement, true_type> this_type;
        typedef T value_type;
        typedef s32 size_type;
        typedef CapacityIncrement capacity_increment_type;

        ArrayBuffer(const this_type&) = delete;
        this_type& operator=(const this_type&) = delete;

        ArrayBuffer()
            :capacity_(0)
            ,size_(0)
            ,items_(NULL)
        {}

        ArrayBuffer(this_type&& rhs)
            :capacity_(rhs.capacity_)
            ,size_(rhs.size_)
            ,items_(rhs.items_)
        {
            rhs.capacity_ = 0;
            rhs.size_ = 0;
            rhs.items_ = NULL;
        }

        explicit ArrayBuffer(size_type capacity)
            :capacity_(capacity_increment_type::getInitCapacity(capacity))
            ,size_(0)
            ,items_(NULL)
        {
            LASSERT(0<=capacity_);
            items_ = reinterpret_cast<T*>(LMALLOC(capacity_*sizeof(T)));
        }

        ~ArrayBuffer()
        {
            for(s32 i = 0; i<size_; ++i){
                items_[i].~value_type();
            }
            LFREE(items_);
        }

        void helper_push_back(const value_type& t)
        {
            if(capacity_<=size_){
                helper_reserve(capacity_increment_type::getNewCapacity(capacity_));
            }
            LPLACEMENT_NEW(&items_[size_]) value_type(t);
            ++size_;
        }
        void helper_push_back(value_type&& t)
        {
            if(capacity_<=size_){
                helper_reserve(capacity_increment_type::getNewCapacity(capacity_));
            }
            LPLACEMENT_NEW(&items_[size_]) value_type(move(t));
            ++size_;
        }

        void helper_pop_back()
        {
            ACC_ASSERT(0<size_);
            --size_;
            items_[size_].~value_type();
        }

        void helper_clear()
        {
            for(s32 i = 0; i<size_; ++i){
                items_[i].~value_type();
            }
            size_ = 0;
        }

        void helper_reserve(size_type capacity)
        {
            if(capacity<=capacity_){
                return;
            }

            capacity = capacity_increment_type::getInitCapacity(capacity);
            value_type* newItems = reinterpret_cast<value_type*>(LMALLOC(capacity*sizeof(value_type)));

            //Copy, and destruct
            for(s32 i = 0; i<size_; ++i){
                LPLACEMENT_NEW(&newItems[i]) value_type(items_[i]);
                items_[i].~value_type();
            }

            LFREE(items_);

            items_ = newItems;
            capacity_ = capacity;
        }

        void helper_resize(size_type size)
        {
            if(size < size_){
                //Destruct redundant items
                for(s32 i = size; i<size_; ++i){
                    items_[i].~value_type();
                }

            } else{
                //Construct new items by default constructor
                helper_reserve(size);
                for(s32 i = size_; i<size; ++i){
                    LPLACEMENT_NEW(&items_[i]) value_type;
                }
            }
            size_ = size;
        }

        void helper_removeAt(s32 index)
        {
            LASSERT(0<=index && index<size_);
            for(s32 i = index+1; i<size_; ++i){
                items_[i-1] = move(items_[i]);
            }
            --size_;
            items_[size_].~value_type();
        }

        size_type capacity_;
        size_type size_;
        value_type* items_;
    };

    //-------------------------------------------------------
    //---
    //---
    //---
    //-------------------------------------------------------
    template<class T, class CapacityIncrement=ArrayStaticCapacityIncrement<> >
    class Array : public ArrayBuffer<T, CapacityIncrement, typename std::is_trivially_copyable<T>::type>
    {
    public:
        typedef Array<T, CapacityIncrement> this_type;
        typedef ArrayBuffer<T, CapacityIncrement, typename std::is_trivially_copyable<T>::type> parent_type;
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef s32 size_type;
        typedef CapacityIncrement capacity_increment_type;

        /**
        @return if lhs<rhs then true else false
        */
        typedef bool(*SortCmp)(const T& lhs, const T& rhs);

        Array();
        Array(this_type&& rhs);
        explicit Array(size_type capacity);
        ~Array();

        inline size_type capacity() const;
        inline size_type size() const;

        inline T& operator[](s32 index);
        inline const T& operator[](s32 index) const;

        inline T& front();
        inline const T& front() const;
        inline T& back();
        inline const T& back() const;

        void push_back(const T& t);
        void push_back(T&& t);
        void pop_back();

        inline iterator begin();
        inline const_iterator begin() const;

        inline iterator end();
        inline const_iterator end() const;

        void clear();
        void reserve(size_type capacity);
        void resize(size_type size);
        void removeAt(s32 index);
        void swap(this_type& rhs);

        this_type& operator=(this_type&& rhs);

        s32 find(const T& ptr) const;
        void insertionsort(const T& t, SortCmp cmp);
    private:
        Array(const this_type&) = delete;
        this_type& operator=(const this_type&) = delete;
    };

    template<class T, class CapacityIncrement>
    Array<T, CapacityIncrement>::Array()
    {
    }

    template<class T, class CapacityIncrement>
    Array<T, CapacityIncrement>::Array(this_type&& rhs)
        :parent_type(lcore::move(rhs))
    {
    }

    template<class T, class CapacityIncrement>
    Array<T, CapacityIncrement>::Array(size_type capacity)
        :parent_type(capacity)
    {
    }

    template<class T, class CapacityIncrement>
    Array<T, CapacityIncrement>::~Array()
    {
    }

    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::size_type
        Array<T, CapacityIncrement>::capacity() const
    {
        return capacity_;
    }

    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::size_type
        Array<T, CapacityIncrement>::size() const
    {
        return size_;
    }

    template<class T, class CapacityIncrement>
    inline T& Array<T, CapacityIncrement>::operator[](s32 index)
    {
        LASSERT(0<=index && index<size_);
        return items_[index];
    }

    template<class T, class CapacityIncrement>
    inline const T& Array<T, CapacityIncrement>::operator[](s32 index) const
    {
        LASSERT(0<=index && index<size_);
        return items_[index];
    }

    template<class T, class CapacityIncrement>
    inline T& Array<T, CapacityIncrement>::front()
    {
        LASSERT(0<size_);
        return items_[0];
    }

    template<class T, class CapacityIncrement>
    inline const T& Array<T, CapacityIncrement>::front() const
    {
        LASSERT(0<size_);
        return items_[0];
    }

    template<class T, class CapacityIncrement>
    inline T& Array<T, CapacityIncrement>::back()
    {
        LASSERT(0<size_);
        return items_[size_-1];
    }

    template<class T, class CapacityIncrement>
    inline const T& Array<T, CapacityIncrement>::back() const
    {
        LASSERT(0<size_);
        return items_[size_-1];
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::push_back(const T& t)
    {
        helper_push_back(t);
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::push_back(T&& t)
    {
        helper_push_back(move(t));
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::pop_back()
    {
        helper_pop_back();
    }

    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::iterator Array<T, CapacityIncrement>::begin()
    {
        return items_;
    }
    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::const_iterator Array<T, CapacityIncrement>::begin() const
    {
        return items_;
    }

    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::iterator Array<T, CapacityIncrement>::end()
    {
        return items_ + size_;
    }
    template<class T, class CapacityIncrement>
    inline typename Array<T, CapacityIncrement>::const_iterator Array<T, CapacityIncrement>::end() const
    {
        return items_ + size_;
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::clear()
    {
        helper_clear();
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::reserve(size_type capacity)
    {
        helper_reserve(capacity);
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::resize(size_type size)
    {
        helper_resize(size);
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::removeAt(s32 index)
    {
        helper_removeAt(index);
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::swap(this_type& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(items_, rhs.items_);
    }

    template<class T, class CapacityIncrement>
    typename Array<T, CapacityIncrement>::this_type& Array<T, CapacityIncrement>::operator=(this_type&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        for(s32 i = 0; i<size_; ++i){
            items_[i].~value_type();
        }
        LFREE(items_);

        capacity_ = rhs.capacity_;
        size_ = rhs.size_;
        items_ = rhs.items_;

        rhs.capacity_ = 0;
        rhs.size_ = 0;
        rhs.items_ = NULL;
        return *this;
    }

    template<class T, class CapacityIncrement>
    s32 Array<T, CapacityIncrement>::find(const T& ptr) const
    {
        for(s32 i=0; i<size_; ++i){
            if(ptr == items_[i]){
                return i;
            }
        }
        return -1;
    }

    template<class T, class CapacityIncrement>
    void Array<T, CapacityIncrement>::insertionsort(const T& t, SortCmp cmp)
    {
        s32 size = size_;
        push_back(t);
        for(s32 i=size-1; 0<=i; --i){
            if(cmp(items_[i+1], items_[i])){
                lray::swap(items_[i+1], items[i]);
                continue;
            }
            break;
        }
    }

}
#endif //INC_LCORE_ARRAY_H_
