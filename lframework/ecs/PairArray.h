#ifndef INC_LFRAMEWORK_PAIRARRAY_H_
#define INC_LFRAMEWORK_PAIRARRAY_H_
/**
@file PairArray.h
@author t-sakai
@date 2017/01/06 create
*/
#include "../lframework.h"

namespace lfw
{
    struct Nothing
    {
        static const Nothing instance_;
    };

    template<class T, class U>
    class PairArray
    {
    public:
        PairArray();
        ~PairArray();

        inline s32 capacity() const;
        inline s32 size() const;
        void reserve(s32 capacity);

        inline const T& getFirst(s32 index) const;
        inline T& getFirst(s32 index);
        inline const U& getSecond(s32 index) const;
        inline U& getSecond(s32 index);

        inline const T* beginFirst() const;
        inline T* beginFirst();
        inline const T* endFirst() const;
        inline T* endFirst();
        inline const U* beginSecond() const;
        inline U* beginSecond();
        inline const U* endSecond() const;
        inline U* endSecond();

        void add(const T& t, const U& u);
        void removeAt(s32 index);
        void clear();
        void swap(PairArray& rhs);
    private:
        PairArray(const PairArray&);
        PairArray& operator=(const PairArray&);

        s32 capacity_;
        s32 size_;
        T* array0_;
        U* array1_;
    };

    template<class T, class U>
    PairArray<T,U>::PairArray()
        :capacity_(0)
        ,size_(0)
        ,array0_(NULL)
        ,array1_(NULL)
    {
    }

    template<class T, class U>
    PairArray<T,U>::~PairArray()
    {
        for(s32 i=0; i<size_; ++i){
            array1_[i].~U();
            array0_[i].~T();
        }
        array1_ = NULL;
        LALIGNED_FREE(array0_, lcore::SSE_ALIGN);
        size_ = 0;
        capacity_ = 0;
    }

    template<class T, class U>
    inline s32 PairArray<T,U>::capacity() const
    {
        return capacity_;
    }

    template<class T, class U>
    inline s32 PairArray<T,U>::size() const
    {
        return size_;
    }

    template<class T, class U>
    void PairArray<T,U>::reserve(s32 capacity)
    {
        LASSERT(0<=capacity);
        if(capacity<=capacity_){
            return;
        }
        s32 size0 = (sizeof(T)*capacity+lcore::SSE_ALIGN_MASK) & ~lcore::SSE_ALIGN_MASK;
        s32 size1 = sizeof(U)*capacity;

        u8* buffer = reinterpret_cast<u8*>(LALIGNED_MALLOC(size0+size1, lcore::SSE_ALIGN));
        T* array0 = reinterpret_cast<T*>(buffer);
        U* array1 = reinterpret_cast<U*>(buffer+size0);
        for(s32 i=0; i<size_; ++i){
            LPLACEMENT_NEW(&array0[i]) T(array0_[i]);
            LPLACEMENT_NEW(&array1[i]) U(array1_[i]);
            array1_[i].~U();
            array0_[i].~T();
        }
        LALIGNED_FREE(array0_, lcore::SSE_ALIGN);
        array0_ = array0;
        array1_ = array1;
        capacity_ = capacity;
    }

    template<class T, class U>
    inline const T& PairArray<T,U>::getFirst(s32 index) const
    {
        LASSERT(0<=index && index<size_);
        return array0_[index];
    }

    template<class T, class U>
    inline T& PairArray<T,U>::getFirst(s32 index)
    {
        LASSERT(0<=index && index<size_);
        return array0_[index];
    }

    template<class T, class U>
    inline const U& PairArray<T,U>::getSecond(s32 index) const
    {
        LASSERT(0<=index && index<size_);
        return array1_[index];
    }

    template<class T, class U>
    inline U& PairArray<T,U>::getSecond(s32 index)
    {
        LASSERT(0<=index && index<size_);
        return array1_[index];
    }

    template<class T, class U>
    inline const T* PairArray<T,U>::beginFirst() const
    {
        return array0_;
    }

    template<class T, class U>
    inline T* PairArray<T,U>::beginFirst()
    {
        return array0_;
    }

    template<class T, class U>
    inline const T* PairArray<T,U>::endFirst() const
    {
        return array0_ + size_;
    }

    template<class T, class U>
    inline T* PairArray<T,U>::endFirst()
    {
        return array0_ + size_;
    }

    template<class T, class U>
    inline const U* PairArray<T,U>::beginSecond() const
    {
        return array1_;
    }

    template<class T, class U>
    inline U* PairArray<T,U>::beginSecond()
    {
        return array1_;
    }

    template<class T, class U>
    inline const U* PairArray<T,U>::endSecond() const
    {
        return array1_ + size_;
    }

    template<class T, class U>
    inline U* PairArray<T,U>::endSecond()
    {
        return array1_ + size_;
    }

    template<class T, class U>
    void PairArray<T,U>::add(const T& t, const U& u)
    {
        LASSERT(size_<capacity_);
        LPLACEMENT_NEW(&array0_[size_]) T(t);
        LPLACEMENT_NEW(&array1_[size_]) U(u);
        ++size_;
    }

    template<class T, class U>
    void PairArray<T,U>::removeAt(s32 index)
    {
        LASSERT(0<=index && index<size_);
        --size_;
        array0_[index] = array0_[size_];
        array1_[index] = array1_[size_];
        array1_[size_].~U();
        array0_[size_].~T();
    }

    template<class T, class U>
    void PairArray<T,U>::clear()
    {
        for(s32 i=0; i<size_; ++i){
            array1_[i].~U();
            array0_[i].~T();
        }
        size_ = 0;
    }

    template<class T, class U>
    void PairArray<T,U>::swap(PairArray& rhs)
    {
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(array0_, rhs.array0_);
        lcore::swap(array1_, rhs.array1_);
    }
}
#endif //INC_LFRAMEWORK_PAIRARRAY_H_
