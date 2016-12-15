#ifndef INC_LCORE_QUEUE_H__
#define INC_LCORE_QUEUE_H__
/**
@file Queue.h
@author t-sakai
@date 2016/06/21 create
*/
#include "Array.h"

namespace lcore
{
    template<class T, class IncSize=array_static_inc_size<16>, class Allocator=DefaultAllocator>
    class QueuePOD : public IncSize
    {
    public:
        typedef QueuePOD<T, IncSize, Allocator> this_type;
        typedef T value_type;
        typedef T* pointer_type;
        typedef T& reference_type;
        typedef const T const_value_type;
        typedef const T* const_pointer_type;
        typedef const T& const_reference_type;

        typedef s32 size_type;
        typedef Allocator allocator_type;
        typedef IncSize inc_size_type;

        struct Iterator
        {
            Iterator(size_type cursor, size_type count)
                :cursor_(cursor)
                ,count_(count)
            {}

        private:
            template<class T, class IncSize, class Allocator> friend class QueuePOD;

            size_type cursor_;
            size_type count_;
        };

        typedef Iterator iterator;
        typedef Iterator const_iterator;

        QueuePOD();
        ~QueuePOD();

        void print()
        {
            printf("next:%d, size:%d, capacity:%d\n", next_, size_, capacity_);
            for(s32 i=0; i<capacity_; ++i){
                printf("%d, ", items_[i]);
            }
            printf("\n");
        }
        inline size_type size() const;

        void push_front(const_reference_type item);
        void push_back(const_reference_type item);

        const_reference_type pop_front();
        const_reference_type pop_back();

        bool try_pop_front(reference_type item);
        bool try_pop_back(reference_type item);

        inline iterator begin() const;

        inline bool end(const iterator& itr) const;

        iterator next(const iterator& itr) const;

        inline const_reference_type get(const iterator& itr) const;

        inline reference_type get(const iterator& itr);

        void swap(this_type& rhs);
    private:
        QueuePOD(const this_type&);
        this_type& operator=(const this_type& queue);

        void expand();
        size_type beginCursor() const;

        size_type next_;
        size_type size_;
        size_type capacity_;
        size_type* items_;
    };

    template<class T, class IncSize, class Allocator>
    QueuePOD<T, IncSize, Allocator>::QueuePOD()
        :next_(0)
        ,size_(0)
        ,capacity_(0)
        ,items_(NULL)
    {}

    template<class T, class IncSize, class Allocator>
    QueuePOD<T, IncSize, Allocator>::~QueuePOD()
    {
        LALLOCATOR_FREE(allocator_type, items_);
        items_ = NULL;
    }

    template<class T, class IncSize, class Allocator>
    inline typename QueuePOD<T, IncSize, Allocator>::size_type QueuePOD<T, IncSize, Allocator>::size() const
    {
        return size_;
    }

    template<class T, class IncSize, class Allocator>
    void QueuePOD<T, IncSize, Allocator>::push_front(const_reference_type item)
    {
        if(capacity_<=size_){
            expand();
        }

        size_type next_front = next_-size_-1;
        if(next_front<0){
            next_front += capacity_;
        }
        items_[next_front] = item;
        ++size_;
    }

    template<class T, class IncSize, class Allocator>
    void QueuePOD<T, IncSize, Allocator>::push_back(const_reference_type item)
    {
        if(capacity_<=size_){
            expand();
        }
        items_[next_] = item;
        ++next_;
        if(capacity_<=next_){
            next_ = 0;
        }
        ++size_;
    }

    template<class T, class IncSize, class Allocator>
    typename QueuePOD<T, IncSize, Allocator>::const_reference_type QueuePOD<T, IncSize, Allocator>::pop_front()
    {
        size_type front = next_-size_;
        if(front<0){
            front += capacity_;
        }
        --size_;
        return items_[front];
    }

    template<class T, class IncSize, class Allocator>
    typename QueuePOD<T, IncSize, Allocator>::const_reference_type QueuePOD<T, IncSize, Allocator>::pop_back()
    {
        --next_;
        if(next_<0){
            next_ = capacity_-1;
        }
        --size_;
        return items_[next_];
    }

    template<class T, class IncSize, class Allocator>
    bool QueuePOD<T, IncSize, Allocator>::try_pop_front(reference_type item)
    {
        if(size_<=0){
            return false;
        }
        size_type front = next_-size_;
        if(front<0){
            front += capacity_;
        }
        item = items_[front];
        --size_;
        return true;
    }

    template<class T, class IncSize, class Allocator>
    bool QueuePOD<T, IncSize, Allocator>::try_pop_back(reference_type item)
    {
        if(size_<=0){
            return false;
        }
        --next_;
        if(next_<0){
            next_ = capacity_-1;
        }
        item = items_[next_];
        --size_;
        return true;
    }

    template<class T, class IncSize, class Allocator>
    inline typename QueuePOD<T, IncSize, Allocator>::iterator QueuePOD<T, IncSize, Allocator>::begin() const
    {
        return iterator(beginCursor(), 0);
    }

    template<class T, class IncSize, class Allocator>
    inline bool QueuePOD<T, IncSize, Allocator>::end(const iterator& itr) const
    {
        return size_<=itr.count_;
    }

    template<class T, class IncSize, class Allocator>
    typename QueuePOD<T, IncSize, Allocator>::iterator QueuePOD<T, IncSize, Allocator>::next(const iterator& itr) const
    {
        size_type cursor = itr.cursor_;
        if(capacity_<=++cursor){
            cursor = 0;
        }
        return iterator(cursor, itr.count_+1);
    }

    template<class T, class IncSize, class Allocator>
    inline typename QueuePOD<T, IncSize, Allocator>::const_reference_type QueuePOD<T, IncSize, Allocator>::get(const iterator& itr) const
    {
        return items_[itr.cursor_];
    }

    template<class T, class IncSize, class Allocator>
    inline typename QueuePOD<T, IncSize, Allocator>::reference_type QueuePOD<T, IncSize, Allocator>::get(const iterator& itr)
    {
        return items_[itr.cursor_];
    }

    template<class T, class IncSize, class Allocator>
    void QueuePOD<T, IncSize, Allocator>::swap(this_type& rhs)
    {
        lcore::swap(next_, rhs.next_);
        lcore::swap(size_, rhs.size_);
        lcore::swap(capacity_, rhs.capacity_);
        lcore::swap(items_, rhs.items_);
    }


    template<class T, class IncSize, class Allocator>
    void QueuePOD<T, IncSize, Allocator>::expand()
    {
        size_type next_capacity = inc_size_type::getNewCapacity(capacity_);
        pointer_type items = (pointer_type)LALLOCATOR_MALLOC(allocator_type, sizeof(value_type)*next_capacity);
#if _DEBUG
        lcore::memset(items, 0xFF, sizeof(value_type)*next_capacity);
#endif
        size_type src = next_-size_;
        size_type dst = src;
        if(src<0){
            src += capacity_;
        }
        if(dst<0){
            dst += next_capacity;
        }

        for(size_type i=0; i<size_; ++i){
            items[dst] = items_[src];
            if(capacity_<=++src){
                src = 0;
            }
            if(next_capacity<=++dst){
                dst = 0;
            }
        }
        LALLOCATOR_FREE(allocator_type, items_);
        items_ = items;
        capacity_ = next_capacity;
        next_ = dst;
    }

    template<class T, class IncSize, class Allocator>
    typename QueuePOD<T, IncSize, Allocator>::size_type QueuePOD<T, IncSize, Allocator>::beginCursor() const
    {
        size_type cursor = next_-size_;
        if(cursor<0){
            cursor += capacity_;
        }
        return cursor;
    }
}
#endif //INC_LCORE_QUEUE_H__
