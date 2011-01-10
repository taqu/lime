#ifndef INC_LRENDER_LIST_H__
#define INC_LRENDER_LIST_H__
/**
@file List.h
@author t-sakai
@date 2010/11/22 create

*/
#include <lcore/Stack.h>

namespace lrender
{
    class ListNode
    {
    public:
        ListNode()
            :prev_(NULL)
            ,next_(NULL)
            ,element_(NULL)
        {}

        ListNode(
            ListNode* prev,
            ListNode* next,
            void* element)
            :prev_(prev)
            ,next_(next)
            ,element_(element)
        {}

        ~ListNode()
        {
            prev_ = NULL;
            next_ = NULL;
            element_ = NULL;
        }

        ListNode* prev_;
        ListNode* next_;
        void* element_;
    };

    template<class T>
    class List
    {
    public:
        static const u32 DEFAULT_STACK_SIZE = 32;

        List();
        ~List();

        void push_back(T* t);
        void push_front(T* t);

        void pop_back();
        void pop_front();

        inline ListNode* front();
        inline ListNode* back();

        ListNode* next(ListNode* node);
        ListNode* prev(ListNode* node);

        void erase(ListNode* node);

        inline ListNode* end();

        inline T* get(ListNode* node);

        ListNode* find(T* element);
    private:
        typedef lcore::Stack<ListNode> stack_type;

        ListNode* front_;
        ListNode* back_;
        ListNode* end_;
        stack_type stackAllocator_;
    };

    template<class T>
    List<T>::List()
    {
        end_ = stackAllocator_.pop();
        front_ = end_;
        back_ = end_;
    }

    template<class T>
    List<T>::~List()
    {
    }

    template<class T>
    void List<T>::push_back(T* t)
    {
        ListNode* node = stackAllocator_.pop();
        node->element_ = reinterpret_cast<void*>(t);
        node->prev_ = back_;
        node->next_ = end_;

        back_->next_ = node;
        back_ = node;
    }

    template<class T>
    void List<T>::push_front(T* t)
    {
        ListNode* node = stackAllocator_.pop();
        node->element_ = reinterpret_cast<void*>(t);
        node->prev_ = end_;
        node->next_ = front_;

        front_->prev_ = node;
        front_ = node;
    }

    template<class T>
    void List<T>::pop_back()
    {
        LASSERT(end_ != back_);

        back_->prev_->next_ = back_->next_;
        back_->next_->prev_ = back_->prev_;

        ListNode* node = back_;
        back_ = back_->prev_;

        node->element_ = NULL;
        node->prev_ = NULL;
        node->next_ = NULL;

        stackAllocator_.push(node);
    }

    template<class T>
    void List<T>::pop_front()
    {
        LASSERT(end_ != front_);

        front_->prev_->next_ = front_->next_;
        front_->next_->prev_ = front_->prev_;

        ListNode* node = front_;
        front_ = front_->next_;

        node->element_ = NULL;
        node->prev_ = NULL;
        node->next_ = NULL;

        stackAllocator_.push(node);
    }

    template<class T>
    inline ListNode* List<T>::front()
    {
        return front_;
    }

    template<class T>
    inline ListNode* List<T>::back()
    {
        return back_;
    }

    template<class T>
    ListNode* List<T>::next(ListNode* node)
    {
        LASSERT(node != NULL);
        return node->next_;
    }

    template<class T>
    ListNode* List<T>::prev(ListNode* node)
    {
        LASSERT(node != NULL);
        return node->prev_;
    }

    template<class T>
    void List<T>::erase(ListNode* node)
    {
        LASSERT(node != NULL);
        LASSERT(node != end_);

        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;

        node->element_ = NULL;
        node->prev_ = NULL;
        node->next_ = NULL;

        stackAllocator_.push(node);
    }

    template<class T>
    inline ListNode* List<T>::end()
    {
        return end_;
    }

    template<class T>
    inline T* List<T>::get(ListNode* node)
    {
        LASSERT(node != NULL);
        return reinterpret_cast<T*>(node->element_);
    }

    template<class T>
    ListNode* List<T>::find(T* element)
    {
        ListNode* node = front_;
        while(node != end_){
            if(node->element_ == element){
                return node;
            }
            node = node->next_;
        }
        return NULL;
    }
}
#endif //INC_LRENDER_LIST_H__
