#ifndef INC_LCORE_TREE_H__
#define INC_LCORE_TREE_H__
/**
@file Tree.h
@author t-sakai
@date 2009/09/15 create
*/
#include "lcore.h"

namespace lcore
{
    template<class T, class NodeType>
    struct TreeNodeOp
    {
        typedef NodeType element_type;
        typedef NodeType* pointer;
        typedef const NodeType* const_pointer;
        typedef NodeType& reference;
        typedef const NodeType& const_reference;

        static u32 getNumChildren(pointer ptr)
        {
            return ptr->getNumChildren();
        }

        static reference getChild(pointer ptr, u32 index)
        {
            return ptr->getChild(index);
        }

        static pointer getParent(pointer ptr)
        {
            return ptr->getParent();
        }

        static T& getElement(pointer ptr)
        {
            return ptr->getElement();
        }
    };

    template<class T, class NodeType, class NodeOp = TreeNodeOp<T, NodeType> >
    class TreeIterator
    {
    public:
        typedef NodeOp node_op;
        typedef TreeIterator<T, NodeType, NodeOp> this_type;
        typedef u32 size_type;

        typedef NodeType node_type;

        typedef T element_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;

        TreeIterator(node_type* ptr)
            :ptr_(ptr)
        {
        }

        ~TreeIterator()
        {
        }

        u32 getChildNum() const
        {
            return node_op::getNumChildren(ptr_);
        }

        TreeIterator getChild(u32 index)
        {
            return TreeIterator( &node_op::getChild(ptr_, index) );
        }

        TreeIterator getParent()
        {
            return TreeIterator( node_op::getParent(ptr_) );
        }

        node_type* getNode()
        {
            return ptr_;
        }

        reference operator*()
        {
            LASSERT(ptr_ != NULL);
            return node_op::getElement(ptr_);
        }

        pointer operator->()
        {
            LASSERT(ptr_ != NULL);
            return &(node_op::getElement(ptr_));
        }

        operator bool() const
        {
            return (ptr_ != NULL);
        }

        bool operator!=(const TreeIterator& rhs) const
        {
            return ptr_ != rhs.ptr_;
        }

        bool operator==(const TreeIterator& rhs) const
        {
            return ptr_ == rhs.ptr_;
        }

    private:
        pointer ptr_;
    };


    //----------------------------------------------------------
    //---
    //--- Node
    //---
    //----------------------------------------------------------
    template<class T>
    class Node
    {
    public:
        typedef Node<T> this_type;
        typedef TreeIterator<T, this_type> iterator;

        Node()
            :parent_(NULL),
            numChildren_(0),
            children_(NULL)
        {
        }

        Node(u32 numChildren);
        ~Node();

        void setNumChildren(u32 numChildren);

        Node* getParent(){ return parent_;}
        const Node* getParent() const{ return parent_;}
        void setParent(Node* parent){ parent_ = parent;}

        u32 getNumChildren() const{ return numChildren_;}
        Node& getChild(u32 index);
        const Node& getChild(u32 index) const;

        T& getElement()
        {
            return element_;
        }

        const T& getElement() const
        {
            return element_;
        }

        void swap(this_type& rhs)
        {
            swap(parent_, rhs.parent_);
            swap(numChildren_, rhs.numChildren_);
            swap(children_, rhs.children_);
            element_.swap(rhs.element_);
        }

        iterator begin()
        {
            return iterator(this);
        }

        iterator end()
        {
            return iterator(NULL);
        }
        
    private:
        Node *parent_;

        u32 numChildren_;
        Node *children_;

        T element_;
    };

    template<class T>
    Node<T>::Node(u32 numChildren)
        :numChildren_(numChildren)
    {
        children_ = LIME_NEW Node[numChildren_];
    }

    template<class T>
    Node<T>::~Node()
    {
        LIME_DELETE_ARRAY(children_);
    }

    template<class T>
    void Node<T>::setNumChildren(u32 numChildren)
    {
        numChildren_ = numChildren;
        LIME_DELETE_ARRAY(children_);
        children_ = (numChildren_ > 0)? LIME_NEW Node[numChildren_] : NULL;
    }

    template<class T>
    Node<T>& Node<T>::getChild(u32 index)
    {
        LASSERT(0<= index && index < numChildren_);
        return children_[index];
    }

    template<class T>
    const Node<T>& Node<T>::getChild(u32 index) const
    {
        LASSERT(0<= index && index < numChildren_);
        return children_[index];
    }


    //----------------------------------------------------------
    //---
    //--- IONode
    //---
    //----------------------------------------------------------
}

#endif //INC_LCORE_TREE_H__
