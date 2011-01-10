#ifndef INC_LCORE_STACK_H__
#define INC_LCORE_STACK_H__
/**
@file Stack.h
@author t-sakai
@date 2010/11/08 create
*/
#include "lcore.h"

namespace lcore
{
    /**
    @brief スタック

    足りなくなれば、固定長で拡張するスタック
    */
    template<class T>
    class Stack
    {
    public:
        Stack(u32 size);
        ~Stack();

        T* pop();
        void push(T* ptr);

        void swap(Stack<T>& rhs)
        {
            lcore::swap(size_, rhs.size_);
            lcore::swap(numBuffers_, rhs.numBuffers_);
            lcore::swap(top_, rhs.top_);
            lcore::swap(buffers_, rhs.buffers_);
        }
    private:
        union Entry
        {
            T element_;
            Entry *next_;
        };

        Entry* initialize(Entry* buffer);

        void incBuffers();

        bool check(T* ptr);

        u32 size_;
        u32 numBuffers_;
        Entry *top_;
        Entry **buffers_;
    };


    template<class T>
    Stack<T>::Stack(u32 size)
        :size_(size)
        ,numBuffers_(1)
    {
        buffers_= LIME_NEW Entry*[1];
        buffers_[0] = LIME_NEW Entry[size_];
        top_ = initialize(buffers_[0]);
    }

    template<class T>
    Stack<T>::~Stack()
    {
        for(u32 i=0; i<numBuffers_; ++i){
            LIME_DELETE_ARRAY(buffers_[i]);
        }
        LIME_DELETE_ARRAY(buffers_);
    }

    template<class T>
    T* Stack<T>::pop()
    {
        Entry *next = top_->next_;
        if(next == NULL){
            incBuffers();
        }
        Entry *ret = top_;
        top_ = top_->next_;
        return (T*)ret;
    }

    template<class T>
    void Stack<T>::push(T* ptr)
    {
        if(ptr == NULL){
            return;
        }

        LASSERT(check(ptr));
        Entry *entry = (Entry*)ptr;
        entry->next_ = top_;
        top_ = entry;
    }

    template<class T>
    typename Stack<T>::Entry* Stack<T>::initialize(Entry* buffer)
    {
        u32 end = size_-1;
        for(u32 i=0; i<end; ++i){
            buffer[i].next_ = &(buffer[i+1]);
        }
        buffer[size_-1].next_ = NULL;
        return &(buffer[0]);
    }

    template<class T>
    void Stack<T>::incBuffers()
    {
        Entry **newBuffers = LIME_NEW Entry*[numBuffers_+1];
        for(u32 i=0; i<numBuffers_; ++i){
            newBuffers[i] = buffers_[i];
        }
        newBuffers[numBuffers_] = LIME_NEW Entry[size_];
        top_->next_ = initialize(newBuffers[numBuffers_]);
        ++numBuffers_;

        LIME_DELETE_ARRAY(buffers_);
        buffers_ = newBuffers;
    }

    template<class T>
    bool Stack<T>::check(T* ptr)
    {
        bool ret = false;
        for(u32 i=0; i<numBuffers_; ++i){
            T *begin = &(buffers_[i]->element_);
            T *end = &((buffers_[i]+size_-1)->element_);

            ret |= (begin <= ptr && ptr<=end);
        }
        return ret;
    }
}
#endif //INC_LCORE_STACK_H__
