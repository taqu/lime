#ifndef INC_LCORE_ARRAY_H__
#define INC_LCORE_ARRAY_H__
/**
@file Array.h
@author t-sakai
@date 2010/01/04 create
*/
#include "lcore.h"

namespace lcore
{
    class array_dynamic_inc_size
    {
    public:
        static const s32 DEFAULT_INCREMENT_SIZE = 16;

        void setIncSize(s32 size)
        {
            LASSERT(0<size);
            incSize_ = size;
        }

        s32 getInitCapacity(s32 initCapacity) const
        {
            return initCapacity;
        }

        s32 getNewCapacity(s32 prevCapacity) const
        {
            return prevCapacity + incSize_;
        }

    protected:
        array_dynamic_inc_size()
            :incSize_(DEFAULT_INCREMENT_SIZE)
        {}

        array_dynamic_inc_size(const array_dynamic_inc_size& rhs)
            :incSize_(rhs.incSize_)
        {}

        explicit array_dynamic_inc_size(s32 incSize)
            :incSize_(incSize)
        {}

        void swap(array_dynamic_inc_size& rhs)
        {
            lcore::swap(incSize_, rhs.incSize_);
        }

        s32 incSize_;
    };

    template<s32 INC_SIZE>
    class array_static_inc_size
    {
    public:
        void setIncSize(s32 /*size*/)
        {
        }

        s32 getInitCapacity(s32 initCapacity) const
        {
            return (initCapacity + (INC_SIZE-1)) & ~(INC_SIZE-1);
        }

        s32 getNewCapacity(s32 prevCapacity) const
        {
            return prevCapacity + INC_SIZE;
        }
    protected:
        array_static_inc_size()
        {}

        array_static_inc_size(const array_static_inc_size& /*rhs*/)
        {}

        explicit array_static_inc_size(s32 /*incSize*/)
        {}

        void swap(array_static_inc_size& /*rhs*/)
        {
        }
    };

    template<class T, class IncSize=array_static_inc_size<16>, class Allocator=DefaultAllocator>
    class Array : public IncSize
    {
    public:
        /**
        @brief lhs<rhsならばtrueを返す
        */
        typedef bool(*SortCmp)(const T& lhs, const T& rhs);

        typedef Array<T, IncSize, Allocator> this_type;
        typedef s32 size_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef Allocator allocator_type;
        typedef IncSize inc_size_type;

        Array();
        Array(const this_type& rhs);
        explicit Array(s32 capacity);
        ~Array();

        s32 size() const{ return size_;}
        s32 capacity() const{ return capacity_;}

        T& operator[](s32 index)
        {
            LASSERT(0<=index && index<size_);
            return items_[index];
        }

        const T& operator[](s32 index) const
        {
            LASSERT(0<=index && index<size_);
            return items_[index];
        }

        T& front()
        {
            LASSERT(0<size_);
            return items_[0];
        }

        const T& front() const
        {
            LASSERT(0<size_);
            return items_[0];
        }

        T& back()
        {
            LASSERT(0<size_);
            return items_[size_-1];
        }

        const T& back() const
        {
            LASSERT(0<size_);
            return items_[size_-1];
        }


        void push_back(const T& t);
        void push_back(T&& t);
        void pop_back();

        iterator begin(){ return items_;}
        const_iterator begin() const{ return items_;}

        iterator end(){ return items_ + size_;}
        const_iterator end() const{ return items_ + size_;}

        void clear();
        void swap(this_type& rhs);
        void reserve(s32 capacity);
        void resize(s32 size);

        void removeAt(s32 index);
        s32 find(const T& ptr) const;
        void insertionsort(const T& t, SortCmp cmp);
    private:
        this_type& operator=(const this_type&);

        s32 capacity_;
        s32 size_;
        T *items_;
    };

    template<class T, class IncSize, class Allocator>
    Array<T, IncSize, Allocator>::Array()
        :capacity_(0)
        ,size_(0)
        ,items_(NULL)
    {
    }

    template<class T, class IncSize, class Allocator>
    Array<T, IncSize, Allocator>::Array(const this_type& rhs)
        :inc_size_type(rhs)
        ,capacity_(getInitCapacity(rhs.capacity_))
        ,size_(rhs.size_)
    {
        items_ = static_cast<T*>( LALLOCATOR_MALLOC(allocator_type, capacity_*sizeof(T)) );
        for(s32 i=0; i<size_; ++i){
            LPLACEMENT_NEW(&items_[i]) T(rhs.items_[i]);
        }
    }

    template<class T, class IncSize, class Allocator>
    Array<T, IncSize, Allocator>::Array(s32 capacity)
        :capacity_(getInitCapacity(capacity))
        ,size_(0)
        ,items_(NULL)
    {
        LASSERT(0<=capacity_);

        items_ = static_cast<T*>( LALLOCATOR_MALLOC(allocator_type, capacity_*sizeof(T)) );
    }

    template<class T, class IncSize, class Allocator>
    Array<T, IncSize, Allocator>::~Array()
    {
        for(s32 i=0; i<size_; ++i){
            items_[i].~T();
        }
        LALLOCATOR_FREE(allocator_type, items_);
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::push_back(const T& t)
    {
        if(capacity_<=size_){
            //新しいバッファ確保
            s32 newCapacity = inc_size_type::getNewCapacity(capacity_);
            T *newItems = static_cast<T*>( LALLOCATOR_MALLOC(allocator_type, newCapacity*sizeof(T)) );

            //コピーコンストラクタでコピー。古い要素のデストラクト
            for(s32 i=0; i<size_; ++i){
                LPLACEMENT_NEW(&newItems[i]) T(lcore::move(items_[i]));
                items_[i].~T();
            }
            LPLACEMENT_NEW(&newItems[size_]) T(t);

            //古いバッファ破棄
            LALLOCATOR_FREE(allocator_type, items_);

            items_ = newItems;
            ++size_;
            capacity_ = newCapacity;

        }else{
            LPLACEMENT_NEW(&items_[size_]) T(t);
            ++size_;
        }
    }


    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::push_back(T&& t)
    {
        if(capacity_<=size_){
            //新しいバッファ確保
            s32 newCapacity = inc_size_type::getNewCapacity(capacity_);
            T *newItems = static_cast<T*>( LALLOCATOR_MALLOC(allocator_type, newCapacity*sizeof(T)) );

            //コピーコンストラクタでコピー。古い要素のデストラクト
            for(s32 i=0; i<size_; ++i){
                LPLACEMENT_NEW(&newItems[i]) T(lcore::move(items_[i]));
                items_[i].~T();
            }
            LPLACEMENT_NEW(&newItems[size_]) T(t);

            //古いバッファ破棄
            LALLOCATOR_FREE(allocator_type, items_);

            items_ = newItems;
            ++size_;
            capacity_ = newCapacity;

        }else{
            LPLACEMENT_NEW(&items_[size_]) T(t);
            ++size_;
        }
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::pop_back()
    {
        LASSERT(0<size_);
        --size_;
        items_[size_].~T();
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::clear()
    {
        for(s32 i=0; i<size_; ++i){
            items_[i].~T();
        }
        size_ = 0;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::swap(this_type& rhs)
    {
        inc_size_type::swap(rhs);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(items_, rhs.items_);
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::reserve(s32 capacity)
    {
        if(capacity<=capacity_){
            return;
        }

        capacity = getInitCapacity(capacity);

        //新しいバッファ確保
        T* newItems = static_cast<T*>( LALLOCATOR_MALLOC(allocator_type, capacity*sizeof(T)) );

        //コピーコンストラクタでコピー。古い要素のデストラクト
        for(s32 i=0; i<size_; ++i){
            LPLACEMENT_NEW(&newItems[i]) T(items_[i]);
            items_[i].~T();
        }

        //古いバッファ破棄
        LALLOCATOR_FREE(allocator_type, items_);

        items_ = newItems;
        capacity_ = capacity;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::resize(s32 size)
    {
        if(size < size_){
            //デストラクト
            for(s32 i=size; i<size_; ++i){
                items_[i].~T();
            }

        }else{
            reserve(size);
            for(s32 i=size_; i<size; ++i){
                LPLACEMENT_NEW(&items_[i]) T;
            }
        }
        size_ = size;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::removeAt(s32 index)
    {
        LASSERT(0<=index && index<size_);
        for(s32 i=index+1; i<size_; ++i){
            items_[i-1] = lcore::move(items_[i]);
        }
        --size_;
        items_[size_].~T();
    }

    template<class T, class IncSize, class Allocator>
    s32 Array<T, IncSize, Allocator>::find(const T& ptr) const
    {
        for(s32 i=0; i<size_; ++i){
            if(ptr == items_[i]){
                return i;
            }
        }
        return -1;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T, IncSize, Allocator>::insertionsort(const T& t, SortCmp cmp)
    {
        s32 size = size_;
        push_back(t);
        for(s32 i=size-1; 0<=i; --i){
            if(cmp(items_[i+1], items_[i])){
                lcore::swap(items_[i+1], items[i]);
                continue;
            }
            break;
        }
    }

    //-----------------------------------------------------------------
    //---
    //--- Array ポインタ特殊化
    //---
    //-----------------------------------------------------------------
    template<class T, class IncSize, class Allocator>
    class Array<T*, IncSize, Allocator> : public IncSize
    {
    public:
        /**
        @brief lhs<rhsならばtrueを返す
        */
        typedef bool(*SortCmp)(const T*& lhs, const T*& rhs);

        typedef Array<T*, IncSize, Allocator> this_type;
        typedef s32 size_type;
        typedef T** iterator;
        typedef const T** const_iterator;
        typedef Allocator allocator_type;
        typedef IncSize inc_size_type;

        Array();
        Array(const this_type& rhs);
        explicit Array(s32 capacity);
        ~Array();

        s32 size() const{ return size_;}
        s32 capacity() const{ return capacity_;}

        T*& operator[](s32 index)
        {
            LASSERT(0<=index && index<size_);
            return items_[index];
        }

        const T* operator[](s32 index) const
        {
            LASSERT(0<=index && index<size_);
            return items_[index];
        }

        T*& front()
        {
            LASSERT(0<size_);
            return items_[0];
        }

        const T*& front() const
        {
            LASSERT(0<size_);
            return items_[0];
        }

        T*& back()
        {
            LASSERT(0<size_);
            return items_[size_-1];
        }

        const T*& back() const
        {
            LASSERT(0<size_);
            return items_[size_-1];
        }


        void push_back(T* const t);
        void pop_back();

        iterator begin(){ return items_;}
        const_iterator begin() const{ return (const_iterator)(items_);}

        iterator end(){ return items_ + size_;}
        const_iterator end() const{ return (const_iterator)(items_ + size_);}

        void clear();
        void swap(this_type& rhs);
        void reserve(s32 capacity);
        void resize(s32 size);

        void removeAt(s32 index);
        s32 find(const T* ptr) const;
        void insertionsort(T* const t, SortCmp cmp);
    private:
        this_type& operator=(const this_type&);

        s32 capacity_;
        s32 size_;
        T** items_;
    };

    template<class T, class IncSize, class Allocator>
    Array<T*, IncSize, Allocator>::Array()
        :capacity_(0)
        ,size_(0)
        ,items_(NULL)
    {
    }

    template<class T, class IncSize, class Allocator>
    Array<T*, IncSize, Allocator>::Array(const this_type& rhs)
        :inc_size_type(rhs)
        ,capacity_(getInitCapacity(rhs.capacity_))
        ,size_(rhs.size_)
        ,items_(NULL)
    {
        items_ = static_cast<T**>( LALLOCATOR_MALLOC(allocator_type, sizeof(T*)*capacity_) );

        for(s32 i=0; i<size_; ++i){
            items_[i] = NULL;
        }
    }

    template<class T, class IncSize, class Allocator>
    Array<T*, IncSize, Allocator>::Array(s32 capacity)
        :capacity_(getInitCapacity(capacity))
        ,size_(0)
        ,items_(NULL)
    {
        LASSERT(0<=capacity_);
        items_ = static_cast<T**>( LALLOCATOR_MALLOC(allocator_type, sizeof(T*)*capacity_) );
    }

    template<class T, class IncSize, class Allocator>
    Array<T*, IncSize, Allocator>::~Array()
    {
        LALLOCATOR_FREE(allocator_type, items_);
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::push_back(T* const t)
    {
        if(capacity_<=size_){
            //新しいバッファ確保
            s32 newCapacity = inc_size_type::getNewCapacity(capacity_);
            T** newItems = static_cast<T**>( LALLOCATOR_MALLOC(allocator_type, sizeof(T*)*newCapacity) );

            //コピー
            for(s32 i=0; i<size_; ++i){
                newItems[i] = items_[i];
            }
            //古いバッファ破棄
            LALLOCATOR_FREE(allocator_type, items_);

            items_ = newItems;
            capacity_ = newCapacity;

        }
        items_[size_] = t;
        ++size_;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::pop_back()
    {
        LASSERT(0<size_);
        --size_;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::clear()
    {
        size_ = 0;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::swap(this_type& rhs)
    {
        inc_size_type::swap(rhs);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(items_, rhs.items_);
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::reserve(s32 capacity)
    {
        if(capacity<=capacity_){
            return;
        }
        capacity = getInitCapacity(capacity);

        //新しいバッファ確保
        T** newItems = static_cast<T**>( LALLOCATOR_MALLOC(allocator_type, sizeof(T*)*capacity) );

        //コピー
        for(s32 i=0; i<size_; ++i){
            newItems[i] = items_[i];
        }

        //古いバッファ破棄
        LALLOCATOR_FREE(allocator_type, items_);

        items_ = newItems;
        capacity_ = capacity;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::resize(s32 size)
    {
        if(size > size_){
            reserve(size);
        }
        size_ = size;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::removeAt(s32 index)
    {
        LASSERT(0<=index && index<size_);
        for(s32 i=index+1; i<size_; ++i){
            items_[i-1] = items_[i];
        }
        --size_;
        items_[size_] = NULL;
    }

    template<class T, class IncSize, class Allocator>
    s32 Array<T*, IncSize, Allocator>::find(const T* ptr) const
    {
        for(s32 i=0; i<size_; ++i){
            if(ptr == items_[i]){
                return i;
            }
        }
        return -1;
    }

    template<class T, class IncSize, class Allocator>
    void Array<T*, IncSize, Allocator>::insertionsort(T* const t, SortCmp cmp)
    {
        s32 size = size_;
        push_back(t);
        for(s32 i=size-1; 0<=i; --i){
            if(cmp(items_[i+1], items_[i])){
                lcore::swap(items_[i+1], items[i]);
                continue;
            }
            break;
        }
    }

    //------------------------------------------------------------
    //---
    //--- ArrayPOD
    //---
    //------------------------------------------------------------
    template<class T, class IncSize=array_static_inc_size<16>, class Allocator=DefaultAllocator>
    class ArrayPOD : public IncSize
    {
    public:
        typedef ArrayPOD<T, IncSize, Allocator> this_type;
        typedef s32 size_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef Allocator allocator_type;
        typedef IncSize inc_size_type;
        typedef T value_type;

        ArrayPOD();
        ArrayPOD(this_type&& rhs); 
        explicit ArrayPOD(s32 capacity);
        ~ArrayPOD();

        s32 size() const{ return size_;}
        s32 capacity() const{ return capacity_;}
        void clear(){ size_ = 0;}

        void push_front(const value_type& x);
        void push_back(const value_type& x);
        void pop_front();
        void pop_back();

        const value_type& operator[](s32 index) const{ return data_[index];}
        value_type& operator[](s32 index){ return data_[index];}

        iterator begin(){ return data_;}
        const_iterator begin() const{ return (const_iterator)(data_);}

        iterator end(){ return data_ + size_;}
        const_iterator end() const{ return (const_iterator)(data_ + size_);}

        void swap(this_type& rhs);
        void reserve(s32 capacity);
        void resize(s32 size);

        void removeAt(s32 index);

        this_type& operator=(this_type&& rhs);
    private:
        ArrayPOD(const this_type&) = delete;
        ArrayPOD& operator=(const this_type&) = delete;

        void expand();

        s32 capacity_;
        s32 size_;
        value_type* data_;
    };

    template<class T, class IncSize, class Allocator>
    ArrayPOD<T, IncSize, Allocator>::ArrayPOD()
        :capacity_(0)
        ,size_(0)
        ,data_(NULL)
    {
    }

    template<class T, class IncSize, class Allocator>
    ArrayPOD<T, IncSize, Allocator>::ArrayPOD(this_type&& rhs)
        :capacity_(rhs.capacity_)
        ,size_(rhs.size_)
        ,data_(rhs.data_)
    {
        rhs.capacity_ = 0;
        rhs.size_ = 0;
        rhs.data_ = NULL;
    }

    template<class T, class IncSize, class Allocator>
    ArrayPOD<T, IncSize, Allocator>::ArrayPOD(s32 capacity)
        :capacity_(getInitCapacity(capacity))
        ,size_(0)
    {
        LASSERT(0<=capacity_);
        data_ = static_cast<value_type*>( LALLOCATOR_MALLOC(allocator_type, sizeof(value_type)*capacity_) );
    }

    template<class T, class IncSize, class Allocator>
    ArrayPOD<T, IncSize, Allocator>::~ArrayPOD()
    {
        LALLOCATOR_FREE(allocator_type, data_);
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::expand()
    {
        s32 capacity = inc_size_type::getNewCapacity(capacity_);
        LASSERT(size_<capacity);

        //新しいバッファ確保
        value_type* data = static_cast<value_type*>( LALLOCATOR_MALLOC(allocator_type, sizeof(value_type)*capacity) );
        lcore::memcpy(data, data_, sizeof(value_type)*size_);
        LALLOCATOR_FREE(allocator_type, data_);
        capacity_ = capacity;
        data_ = data;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::push_front(const value_type& x)
    {
        if(capacity_<=size_){
            expand();
        }
        for(s32 i=size_; 1<=i; --i){
            data_[i] = data_[i-1];
        }
        ++size_;
        data_[0] = x;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::push_back(const value_type& x)
    {
        if(capacity_<=size_){
            expand();
        }
        data_[size_] = x;
        ++size_;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::pop_front()
    {
        if(size_<=0){
            return;
        }
        for(s32 i=1; i<size_; ++i){
            data_[i-1] = data_[i];
        }
        --size_;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::pop_back()
    {
        if(size_<=0){
            return;
        }
        --size_;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::swap(this_type& rhs)
    {
        inc_size_type::swap(rhs);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(data_, rhs.data_);
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::reserve(s32 capacity)
    {
        if(capacity<=capacity_){
            return;
        }
        capacity = getInitCapacity(capacity);

        //新しいバッファ確保
        value_type* data = static_cast<value_type*>( LALLOCATOR_MALLOC(allocator_type, sizeof(value_type)*capacity) );
        lcore::memcpy(data, data_, sizeof(value_type)*size_);
        LALLOCATOR_FREE(allocator_type, data_);
        capacity_ = capacity;
        data_ = data;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::resize(s32 size)
    {
        if(size_ < size){
            reserve(size);
        }
        size_ = size;
    }

    template<class T, class IncSize, class Allocator>
    void ArrayPOD<T, IncSize, Allocator>::removeAt(s32 index)
    {
        LASSERT(0<=index && index<size_);
        for(s32 i=index+1; i<size_; ++i){
            data_[i-1] = data_[i];
        }
        --size_;
    }

    template<class T, class IncSize, class Allocator>
    typename ArrayPOD<T, IncSize, Allocator>::this_type&
        ArrayPOD<T, IncSize, Allocator>::operator=(this_type&& rhs)
    {
        if(this != &rhs){
            LALLOCATOR_FREE(allocator_type, data_);
            capacity_ = rhs.capacity_;
            data_ = rhs.data_;
            size_ = rhs.size_;
            rhs.capacity_ = 0;
            rhs.size_ = 0;
            rhs.data_ = NULL;
        }
        return *this;
    }
}

#endif //INC_LCORE_ARRAY_H__
