/**
@file ECSTree.cpp
@author t-sakai
@date 2016/08/03 create
*/
#include "ecs/ECSTree.h"

namespace lfw
{
    //----------------------------------------------
    //---
    //--- ECSNode
    //---
    //----------------------------------------------
    void ECSNode::init(u16 index)
    {
        index_ = index;
        parent_ = Invalid;
        reset();
    }

    void ECSNode::reset()
    {
        child_ = Invalid;
        numChildren_ = 0;
        prev_ = index_;
        next_ = index_;
    }

    void ECSNode::addChild(ECSTree& tree, ECSNode& node)
    {
        LASSERT(numChildren_<MaxChildren);
        if(Invalid == child_){
            LASSERT(numChildren_<=0);
            ++numChildren_;
            child_ = node.index();
        }else{
            ++numChildren_;
            ECSNode& childNode = tree.get(child_);
            childNode.linkPrev(tree, node);
        }
        node.parent_ = index();
    }

    void ECSNode::removeChild(ECSTree& tree, ECSNode& node)
    {
        LASSERT(0<numChildren_);
        if(child_ == node.index()){
            child_ = (node.index() != node.next())? node.next() : Invalid;
            //child_ = node.next();
            //if(child_ == node.index()){
            //    child_ = Invalid;
            //}
        }
        node.unlink(tree);
        node.parent_ = Invalid;
        --numChildren_;
    }

    void ECSNode::setParent(ECSTree& tree, ECSNode& node)
    {
        if(Invalid != parent()){
            tree.getParent(parent()).removeChild(tree, *this);
        }
        node.addChild(tree, *this);
    }

    const ECSNode* ECSNode::getChild(ECSTree& tree, u16 index) const
    {
        LASSERT(0<=index && index<numChildren_);
        LASSERT(Invalid != child_);

        const ECSNode& node = tree.get(child_);
        u16 end = node.index();
        u16 next = node.next();
        u16 count = 0;
        do{
            if(count == index){
                return &node;
            }
            next = node.next();
            ++count;
        }while(end != next);
        return NULL;
    }

    ECSNode* ECSNode::getChild(ECSTree& tree, u16 index)
    {
        LASSERT(0<=index && index<numChildren_);
        LASSERT(Invalid != child_);

        ECSNode& node = tree.get(child_);
        u16 end = node.index();
        u16 next = node.next();
        u16 count = 0;
        do{
            if(count == index){
                return &node;
            }
            next = node.next();
            ++count;
        }while(end != next);
        return NULL;
    }

    void ECSNode::removeFromTree(ECSTree& tree)
    {
        if(Invalid != parent()){
            tree.getParent(parent()).removeChild(tree, *this);
        }
    }

    void ECSNode::setFirstSibling(ECSTree& tree)
    {
        ECSNode& parent = tree.getParent(this->parent());
        if(index() == parent.child()){
            return;
        }
        unlink(tree);
        ECSNode& child = tree.get(parent.child());
        child.linkPrev(tree, *this);
        parent.child_ = index();
    }

    void ECSNode::setLastSibling(ECSTree& tree)
    {
        ECSNode& parent = tree.getParent(this->parent());
        if(index() == parent.child()){
            parent.child_ = next();
            return;
        }
        unlink(tree);
        ECSNode& child = tree.get(this->child());
        child.linkPrev(tree, *this);
    }

    void ECSNode::linkPrev(ECSTree& tree, ECSNode& node)
    {
        LASSERT(this != &node);
        LASSERT(node.index() == node.next() && node.index() == node.prev());

        node.prev_ = prev_;
        node.next_ = index();

        ECSNode& prev = tree.get(prev_);
        prev.next_ = node.index();
        prev_ = node.index();
    }

    void ECSNode::linkNext(ECSTree& tree, ECSNode& node)
    {
        LASSERT(this != &node);
        LASSERT(node.index() == node.next() && node.index() == node.prev());

        node.prev_ = index();
        node.next_ = next_;

        ECSNode& next = tree.get(next_);
        next.prev_ = node.index();
        next_ = node.index();
    }

    void ECSNode::unlink(ECSTree& tree)
    {
        ECSNode& next = tree.get(next_);
        ECSNode& prev = tree.get(prev_);
        next.prev_ = this->prev();
        prev.next_ = this->next();
        prev_ = next_ = index();
    }

    //----------------------------------------------
    //---
    //--- ECSTree
    //---
    //----------------------------------------------
    ECSTree::ECSTree()
        :nodes_(NULL)
    {
        root_.reset();
        root_.init(ECSNode::Root);
    }

    ECSTree::~ECSTree()
    {
        LDELETE_ARRAY(nodes_);
    }

    void ECSTree::resize(s32 size, s32 newSize)
    {
        expand(nodes_, size, newSize);
    }

    void ECSTree::create(u16 id)
    {
        LASSERT(id<ECSNode::Root);
        nodes_[id].init(id);
        nodes_[id].setParent(*this, getRoot());
    }

    void ECSTree::destroy(u16 id)
    {
        LASSERT(ECSNode::Root != id);
        nodes_[id].removeFromTree(*this);
        nodes_[id].init(ECSNode::Invalid);
    }

    //----------------------------------------------
    //---
    //--- ECSEntityTree
    //---
    //----------------------------------------------
    ECSEntityTree::ECSEntityTree()
        :entities_(NULL)
    {
    }

    ECSEntityTree::~ECSEntityTree()
    {
        LDELETE_ARRAY(entities_);
    }

    void ECSEntityTree::resize(s32 size, s32 newSize)
    {
        expand(entities_, size, newSize);
        tree_.resize(size, newSize);
        processIDOrder_.clear();
        processEntityOrder_.clear();
        processIDOrder_.reserve(newSize);
        processEntityOrder_.reserve(newSize);
    }

    void ECSEntityTree::traverse()
    {
        processIDOrder_.clear();
        processEntityOrder_.clear();

        ECSNode* node = &tree_.getRoot();
        u16 child = node->child();
        for(u16 i=0; i<node->numChildren(); ++i){
            processIDOrder_.push_back(child);
            processEntityOrder_.push_back(entities_[child]);
            child = tree_.get(child).next();
        }

        s32 current=0;
        while(current<processIDOrder_.size()){
            u16 n = processIDOrder_[current];
            ++current;
            node = &tree_.get(n);
            child = node->child();
            for(u16 i=0; i<node->numChildren(); ++i){
                processIDOrder_.push_back(child);
                processEntityOrder_.push_back(entities_[child]);
                child = tree_.get(child).next();
            }
        }
    }

    void ECSEntityTree::destroy(u16 id)
    {
        tree_.destroy(id);
        entities_[id].clear();
    }

    void ECSEntityTree::destroyRecurse(u16 id)
    {
        ECSNode& node = tree_.get(id);
        while(0<node.numChildren()){
            destroyRecurse(node.child());
        }
        tree_.destroy(id);
        entities_[id].clear();
    }
}
