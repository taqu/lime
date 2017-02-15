#ifndef INC_LCORE_HANDLETABLE_H__
#define INC_LCORE_HANDLETABLE_H__
/**
@file HandleTable.h
@author t-sakai
@date 2016/05/13 create
*/
#include "lcore.h"
#include "Array.h"

namespace lcore
{
    //----------------------------------------------------------------------
    //---
    //--- Handle
    //---
    //----------------------------------------------------------------------
    template<class T=u32, s32 U=24>
    struct Handle
    {
    public:
        typedef Handle<T,U> this_type;
        typedef T value_type;
        static const s32 Shift=U;
        static const value_type Mask=(0x01U<<Shift)-1;
        static const value_type Invalid = 0;
        static const value_type MaxCount = (0x01U<<(sizeof(T)*8-1-U))-1;
        static const value_type UsedFlag = 0x01U<<(sizeof(T)*8-1);

        static Handle construct(value_type value)
        {
            return {value};
        }

        static Handle construct(value_type count, value_type index, value_type flag)
        {
            return {(count<<Shift) | (index&Mask) | flag};
        }

        //Handle()
        //    :value_(0)
        //{}

        //explicit Handle(value_type value)
        //    :value_(value)
        //{}

        //Handle(value_type count, value_type index, value_type flag)
        //{
        //    value_ = (count<<Shift) | (index&Mask) | flag;
        //}

        inline value_type count() const
        {
            return (value_>>Shift) & MaxCount;
        }

        inline value_type index() const
        {
            return value_&Mask;
        }

        inline bool isNull() const
        {
            return 0==value_;
        }

        inline bool isUsed() const
        {
            return 0 != (value_ & UsedFlag);
        }

        inline void clear()
        {
            value_ = 0;
        }

        inline void setUsed()
        {
            value_ |= UsedFlag;
        }

        inline void resetUsed()
        {
            value_ &= ~UsedFlag;
        }

        inline bool operator==(const this_type& handle) const
        {
            return value_ == handle.value_;
        }

        inline bool operator!=(const this_type& handle) const
        {
            return value_ != handle.value_;
        }

        value_type value_;
    };

    //----------------------------------------------------------------------
    //---
    //--- HandleTable
    //---
    //----------------------------------------------------------------------
    template<class T=u32, s32 U=24>
    class HandleTable
    {
    public:
        typedef HandleTable<T,U> this_type;
        typedef Handle<T,U> handle_type;
        typedef typename handle_type::value_type value_type;

        HandleTable();
        HandleTable(s32 capacity, s32 expandSize);
        ~HandleTable();

        s32 capacity() const{ return capacity_;}
        s32 size() const{ return size_;}
        handle_type create();
        void destroy(handle_type handle);
        bool isValid(handle_type handle) const;
        void clear();

        void swap(this_type& rhs);
    private:
        HandleTable(const this_type&);
        this_type& operator=(const this_type&);

        static const s32 Shift = handle_type::Shift;
        static const value_type MaxCount = handle_type::MaxCount;
        static const value_type UsedFlag = handle_type::UsedFlag;
        static const value_type InvalidID = handle_type::Mask;
        static const s32 MaxCapacity = handle_type::Mask;

        struct Entry
        {
            inline value_type count() const
            {
                return (value_>>Shift) & MaxCount;
            }

            inline value_type next() const
            {
                return value_&handle_type::Mask;
            }

            inline bool isUsed() const
            {
                return 0 != (value_ & UsedFlag);
            }

            inline void set(value_type count, value_type next, value_type flag)
            {
                value_ = (count<<Shift) | (next & handle_type::Mask) | flag;
            }

            inline void setUsed()
            {
                value_ |= UsedFlag;
            }

            inline void resetUsed()
            {
                value_ &= UsedFlag;
            }

            value_type value_;
        };

        Entry* entries_;
        s32 capacity_;
        s32 size_;
        s32 expandSize_;
        value_type nextId_;
        value_type freeList_;
    };

    template<class T, s32 U>
    HandleTable<T,U>::HandleTable()
        :entries_(NULL)
        ,capacity_(0)
        ,size_(0)
        ,expandSize_(128)
        ,nextId_(0)
        ,freeList_(InvalidID)
    {
    }

    template<class T, s32 U>
    HandleTable<T,U>::HandleTable(s32 capacity, s32 expandSize)
        :entries_(NULL)
        ,capacity_(capacity)
        ,size_(0)
        ,expandSize_(expandSize)
        ,nextId_(0)
        ,freeList_(InvalidID)
    {
        LASSERT(0<=capacity_);
        LASSERT(0<expandSize_);
        entries_ = LNEW Entry[capacity_];
        lcore::memset(entries_, 0, sizeof(Entry)*capacity_);
    }

    template<class T, s32 U>
    HandleTable<T,U>::~HandleTable()
    {
        LDELETE_ARRAY(entries_);
    }

    template<class T, s32 U>
    typename HandleTable<T,U>::handle_type HandleTable<T,U>::create()
    {
        value_type index;
        value_type count;
        if(freeList_!=InvalidID){
            index = freeList_;
            count = entries_[index].count();
            freeList_ = entries_[index].next();
        }else{
            if(static_cast<u32>(capacity_)<=nextId_){
                //Expand reserved entries
                s32 newCapacity = capacity_ + expandSize_;
                LASSERT(newCapacity<MaxCapacity);//Check max entries
                Entry* entries = LNEW Entry[newCapacity];

                lcore::memcpy(entries, entries_, sizeof(Entry)*capacity_);
                lcore::memset(entries+capacity_, 0, sizeof(Entry)*(newCapacity-capacity_));

                LDELETE_ARRAY(entries_);
                capacity_ = newCapacity;
                entries_ = entries;
            }
            index = nextId_;
            count = 0;
            ++nextId_;
        }
        ++count;
        if(MaxCount<count){
            count = 1;
        }
        ++size_;

        //Set used flag
        entries_[index].set(count, index, UsedFlag);
        return handle_type::construct(count, index, UsedFlag);
    }

    template<class T, s32 U>
    void HandleTable<T,U>::destroy(handle_type handle)
    {
        value_type index = handle.index();
        value_type count = handle.count();
        LASSERT(entries_[index].count() == count);
        LASSERT(count<=MaxCount);

        --size_;

        //Reset used flag
        entries_[index].set(count, freeList_, 0);
        freeList_ = index;
    }

    template<class T, s32 U>
    bool HandleTable<T,U>::isValid(handle_type handle) const
    {
        LASSERT(0<=handle.index() && handle.index()<nextId_);
        return (entries_[handle.index()].isUsed() && entries_[handle.index()].value_ == handle.value_);
    }

    template<class T, s32 U>
    void HandleTable<T,U>::clear()
    {
        size_ = 0;
        nextId_ = 0;
        freeList_ = InvalidID;
        lcore::memset(entries_, 0, sizeof(Entry)*capacity_);
    }

    template<class T, s32 U>
    void HandleTable<T,U>::swap(this_type& rhs)
    {
        lcore::swap(entries_, rhs.entries_);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(expandSize_, rhs.expandSize_);
        lcore::swap(nextId_, rhs.nextId_);
        lcore::swap(freeList_, rhs.freeList_);
    }

    //----------------------------------------------------------------------
    //---
    //---
    //---
    //----------------------------------------------------------------------
    /**
    */
    template<class T=s32>
    class InsecureHandleTable
    {
    public:
        typedef InsecureHandleTable<T> this_type;
        typedef T handle_type;
        typedef T value_type;
        static const handle_type InvalidID = (handle_type)-1;
        static const s32 MaxCapacity = 0x7FFFFFFF;

        InsecureHandleTable();
        InsecureHandleTable(s32 capacity, s32 expandSize);
        ~InsecureHandleTable();

        s32 capacity() const{ return capacity_;}
        s32 size() const{ return size_;}
        handle_type create();
        void destroy(handle_type handle);
        void clear();

        void swap(this_type& rhs);
    private:
        InsecureHandleTable(const this_type&);
        this_type& operator=(const this_type&);

        handle_type* entries_;
        s32 capacity_;
        s32 size_;
        s32 expandSize_;
        value_type nextId_;
        value_type freeList_;
    };

    template<class T>
    InsecureHandleTable<T>::InsecureHandleTable()
        :entries_(NULL)
        ,capacity_(0)
        ,size_(0)
        ,expandSize_(128)
        ,nextId_(0)
        ,freeList_(InvalidID)
    {
    }

    template<class T>
    InsecureHandleTable<T>::InsecureHandleTable(s32 capacity, s32 expandSize)
        :entries_(NULL)
        ,capacity_(capacity)
        ,size_(0)
        ,expandSize_(expandSize)
        ,nextId_(0)
        ,freeList_(InvalidID)
    {
        LASSERT(0<=capacity_);
        LASSERT(0<expandSize_);
        entries_ = LNEW handle_type[capacity_];
        lcore::memset(entries_, 0, sizeof(handle_type)*capacity_);
    }

    template<class T>
    InsecureHandleTable<T>::~InsecureHandleTable()
    {
        LDELETE_ARRAY(entries_);
    }

    template<class T>
    typename InsecureHandleTable<T>::handle_type InsecureHandleTable<T>::create()
    {
        handle_type index;
        if(freeList_!=InvalidID){
            index = freeList_;
            freeList_ = entries_[index];
        }else{
            if(capacity_<=nextId_){
                //Expand reserved entries
                s32 newCapacity = capacity_ + expandSize_;
                LASSERT(newCapacity<MaxCapacity);//Check max entries
                handle_type* entries = LNEW handle_type[newCapacity];

                lcore::memcpy(entries, entries_, sizeof(handle_type)*capacity_);
                lcore::memset(entries+capacity_, 0, sizeof(handle_type)*(newCapacity-capacity_));

                LDELETE_ARRAY(entries_);
                capacity_ = newCapacity;
                entries_ = entries;
            }
            index = nextId_;
            ++nextId_;
        }
        ++size_;
        entries_[index] = index;
        return index;
    }

    template<class T>
    void InsecureHandleTable<T>::destroy(handle_type handle)
    {
        value_type index = handle;

        --size_;
        entries_[index] = freeList_;
        freeList_ = index;
    }

    template<class T>
    void InsecureHandleTable<T>::clear()
    {
        size_ = 0;
        nextId_ = 0;
        freeList_ = InvalidID;
        lcore::memset(entries_, 0, sizeof(handle_type)*capacity_);
    }

    template<class T>
    void InsecureHandleTable<T>::swap(this_type& rhs)
    {
        lcore::swap(entries_, rhs.entries_);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(expandSize_, rhs.expandSize_);
        lcore::swap(nextId_, rhs.nextId_);
        lcore::swap(freeList_, rhs.freeList_);
    }
}
#endif //INC_LCORE_HANDLETABLE_H__
