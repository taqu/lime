#ifndef INC_LCORE_INTRUSIVE_LIST_H__
#define INC_LCORE_INTRUSIVE_LIST_H__
/**
@file List.h
@author t-sakai
@date 2011/03/09 create
*/
#include "../lcore.h"

namespace lcore
{
namespace intrusive
{
    /**
    @brief �����N�|�C���^���ߍ��݌^���X�g

    �ėp���Ȍ���D��B�O������n�����|�C���^�����X�g�ɂ��邩�ǂ����A�`�F�b�N���Ȃ��B
    �g���������ƈ��S�łȂ��B
    */
    template<class T>
    class List
    {
    public:
        typedef List<T> this_type;

        inline List();
        inline ~List();

        inline T* begin();
        inline T* rbegin();
        inline T* end();
        inline T* next(T* current);
        inline T* prev(T* current);

        void push_front(T* element);
        void push_back(T* element);
        void erase(T* element);

        void swap(this_type& rhs);
    private:
        void initialize(T* element);

        u32 size_;
        T* top_;
        T* tail_;
    };

    template<class T>
    inline List<T>::List()
        :size_(0)
        ,top_(NULL)
        ,tail_(NULL)
    {
    }

    template<class T>
    inline List<T>::~List()
    {
    }

    template<class T>
    inline T* List<T>::begin()
    {
        return top_;
    }

    template<class T>
    inline T* List<T>::rbegin()
    {
        return tail_;
    }

    template<class T>
    inline T* List<T>::end()
    {
        return NULL;
    }

    template<class T>
    inline T* List<T>::next(T* current)
    {
        LASSERT(current != NULL);
        return current->getNext();
    }


    template<class T>
    inline T* List<T>::prev(T* current)
    {
        LASSERT(current != NULL);
        return current->getPrev();
    }

    template<class T>
    void List<T>::erase(T* element)
    {
        LASSERT(element != NULL);
        LASSERT(size_>0);

        T* prev = element->getPrev();
        T* next = element->getNext();

        if(prev){
            prev->setNext(next);
        }
        if(next){
            next->setPrev(prev);
        }
        --size_;
        if(size_ == 0){
            top_ = NULL;
            tail_ = NULL;
        }
    }

    template<class T>
    void List<T>::push_front(T* element)
    {
        LASSERT(element != NULL);
        if(0 == size_){ //T�̃T�C�Y���傫���Ɣԕ���������
            initialize(element);
            return;
        }
        element->setNext(top_);
        element->setPrev(NULL);
        top_->setPrev(element);
        top_ = element;
        ++size_;
    }

    template<class T>
    void List<T>::push_back(T* element)
    {
        LASSERT(element != NULL);
        if(0 == size_){ //T�̃T�C�Y���傫���Ɣԕ���������
            initialize(element);
            return;
        }
        element->setNext(NULL);
        element->setPrev(tail_);
        tail_->setNext(element);
        tail_ = element;
        ++size_;
    }

    template<class T>
    void List<T>::swap(this_type& rhs)
    {
        lcore::swap(size_, rhs.size_);
        lcore::swap(top_, rhs.top_);
        lcore::swap(tail_, rhs.tail_);
    }

    template<class T>
    void List<T>::initialize(T* element)
    {
        top_ = element;
        tail_ = element;

        element->setNext(NULL);
        element->setPrev(NULL);
        ++size_;
    }
}
}
#endif //INC_LCORE_INTRUSIVE_LIST_H__
