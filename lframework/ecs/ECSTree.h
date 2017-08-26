#ifndef INC_LFRAMEWORK_ECSTREE_H__
#define INC_LFRAMEWORK_ECSTREE_H__
/**
@file ECSTree.h
@author t-sakai
@date 2016/08/03 create
*/
#include "lecs.h"
#include "Entity.h"

namespace lfw
{
    class ECSTree;

    //----------------------------------------------
    //---
    //--- ECSNode
    //---
    //----------------------------------------------
    struct ECSNode
    {
    public:
        static const u16 Invalid = (u16)-1;
        static const u16 Root = Invalid-1;
        static const u16 MaxChildren = 0xFFFFU-2;

        inline u16 index() const;
        inline u16 parent() const;
        inline u16 numChildren() const;
        inline u16 child() const;
        inline u16 prev() const;
        inline u16 next() const;

        void init(u16 index);
        void reset();

        void setParent(ECSTree& tree, ECSNode& node);
        const ECSNode* getChild(ECSTree& tree, u16 index) const;
        ECSNode* getChild(ECSTree& tree, u16 index);

        void setFirstSibling(ECSTree& tree);
        void setLastSibling(ECSTree& tree);

        void removeFromTree(ECSTree& tree);
    private:
        friend class ECSTree;

        void addChild(ECSTree& tree, ECSNode& node);
        void removeChild(ECSTree& tree, ECSNode& node);
        void linkPrev(ECSTree& tree, ECSNode& node);
        void linkNext(ECSTree& tree, ECSNode& node);
        void unlink(ECSTree& tree);

        u16 index_;
        u16 parent_;
        u16 child_;
        u16 numChildren_;
        u16 prev_;
        u16 next_;
    };

    inline u16 ECSNode::index() const
    {
        return index_;
    }

    inline u16 ECSNode::parent() const
    {
        return parent_;
    }

    inline u16 ECSNode::numChildren() const
    {
        return numChildren_;
    }

    inline u16 ECSNode::child() const
    {
        return child_;
    }

    inline u16 ECSNode::prev() const
    {
        return prev_;
    }

    inline u16 ECSNode::next() const
    {
        return next_;
    }

    //----------------------------------------------
    //---
    //--- ECSTree
    //---
    //----------------------------------------------
    class ECSTree
    {
    public:
        ECSTree();
        ~ECSTree();

        void resize(s32 size, s32 newSize);

        void create(u16 id);
        void destroy(u16 id);

        inline const ECSNode& getRoot() const;
        inline ECSNode& getRoot();

        inline const ECSNode& get(u16 id) const;
        inline ECSNode& get(u16 id);

        inline const ECSNode& getParent(u16 id) const;
        inline ECSNode& getParent(u16 id);

    private:
        ECSTree(const ECSTree&);
        ECSTree& operator=(const ECSTree&);

        ECSNode root_;
        ECSNode* nodes_;
    };

    inline const ECSNode& ECSTree::getRoot() const
    {
        return root_;
    }

    inline ECSNode& ECSTree::getRoot()
    {
        return root_;
    }

    inline const ECSNode& ECSTree::get(u16 id) const
    {
        LASSERT(id<ECSNode::Root);
        return nodes_[id];
    }

    inline ECSNode& ECSTree::get(u16 id)
    {
        LASSERT(id<ECSNode::Root);
        return nodes_[id];
    }

    inline const ECSNode& ECSTree::getParent(u16 id) const
    {
        return (id == ECSNode::Root)? root_ : nodes_[id];
    }

    inline ECSNode& ECSTree::getParent(u16 id)
    {
        return (id == ECSNode::Root)? root_ : nodes_[id];
    }

    //----------------------------------------------
    //---
    //--- ECSEntityTree
    //---
    //----------------------------------------------
    class ECSEntityTree
    {
    public:
        ECSEntityTree();
        ~ECSEntityTree();

        void resize(s32 size, s32 newSize);

        inline void create(u16 id, Entity entity);
        inline const ECSNode& get(u16 id) const;
        inline ECSNode& get(u16 id);
        inline const ECSNode& getRoot() const;
        inline ECSNode& getRoot();

        inline void setParent(ECSNode& node, ECSNode& parent);
        inline void setFirstSibling(ECSNode& node);
        inline void setLastSibling(ECSNode& node);

        inline const Entity& getEntity(u16 id) const;

        void traverse();
        void destroy(u16 id);
        void destroyRecurse(u16 id);

        inline const u16* beginID() const;
        inline const u16* endID() const;

        inline s32 getNumEntities() const;
        inline const Entity* beginEntity() const;
        inline const Entity* endEntity() const;
    private:
        ECSEntityTree(const ECSEntityTree&);
        ECSEntityTree& operator=(const ECSEntityTree&);
        
        s32 size_;
        Entity* entities_;
        ECSTree tree_;
        lcore::Array<u16> processIDOrder_;
        lcore::Array<Entity> processEntityOrder_;
    };

    inline void ECSEntityTree::create(u16 id, Entity entity)
    {
        entities_[id] = entity;
        tree_.create(id);
    }

    inline const ECSNode& ECSEntityTree::get(u16 id) const
    {
        return tree_.get(id);
    }

    inline ECSNode& ECSEntityTree::get(u16 id)
    {
        return tree_.get(id);
    }

    inline const ECSNode& ECSEntityTree::getRoot() const
    {
        return tree_.getRoot();
    }

    inline ECSNode& ECSEntityTree::getRoot()
    {
        return tree_.getRoot();
    }

    inline void ECSEntityTree::setParent(ECSNode& node, ECSNode& parent)
    {
        node.setParent(tree_, parent);
    }

    inline void ECSEntityTree::setFirstSibling(ECSNode& node)
    {
        node.setFirstSibling(tree_);
    }

    inline void ECSEntityTree::setLastSibling(ECSNode& node)
    {
        node.setLastSibling(tree_);
    }

    inline const Entity& ECSEntityTree::getEntity(u16 id) const
    {
        return entities_[id];
    }

    inline const u16* ECSEntityTree::beginID() const
    {
        return processIDOrder_.begin();
    }

    inline const u16* ECSEntityTree::endID() const
    {
        return processIDOrder_.end();
    }

    inline s32 ECSEntityTree::getNumEntities() const
    {
        return processEntityOrder_.size();
    }

    inline const Entity* ECSEntityTree::beginEntity() const
    {
        return processEntityOrder_.begin();
    }

    inline const Entity* ECSEntityTree::endEntity() const
    {
        return processEntityOrder_.end();
    }

    //----------------------------------------------
    //---
    //--- ECSNodeIterator
    //---
    //----------------------------------------------
    template<class T>
    class ECSNodeIterator
    {
    public:
        ECSNodeIterator(u16 numChildren, T** components, ECSEntityTree* tree, ECSNode* node)
            :components_(components)
            ,tree_(tree)
            ,node_(node)
            ,index_(0)
            ,numChildren_(numChildren)
        {}

        ECSNodeIterator& operator++();

        u16 size() const
        {
            return numChildren_;
        }

        bool end() const
        {
            return numChildren_<=index_;
        }

        T* current()
        {
            return components_[node_->index()];
        }

    private:
        template<class T> friend class ECSNodeConstIterator;

        T** components_;
        ECSEntityTree* tree_;
        ECSNode* node_;
        u16 index_;
        u16 numChildren_;
    };

    template<class T>
    ECSNodeIterator<T>& ECSNodeIterator<T>::operator++()
    {
        node_ = &tree_->get(node_->next());
        ++index_;
        return *this;
    }

    template<class T>
    class ECSNodeConstIterator
    {
    public:
        ECSNodeConstIterator(u16 numChildren, T**const components, const ECSEntityTree* tree, const ECSNode* node)
            :components_(components)
            ,tree_(tree)
            ,node_(node)
            ,index_(0)
            ,numChildren_(numChildren)
        {}

        ECSNodeConstIterator(const ECSNodeIterator<T>& iterator)
            :components_(iterator.components_)
            ,tree_(iterator.tree_)
            ,node_(iterator.node_)
            ,index_(iterator.index_)
            ,numChildren_(iterator.numChildren_)
        {}

        ECSNodeConstIterator<T>& operator++();

        u16 size() const
        {
            return numChildren_;
        }

        bool end() const
        {
            return numChildren_<=index_;
        }

        const T* current()
        {
            return components_[node_->index()];
        }

        ECSNodeConstIterator<T>& operator=(const ECSNodeIterator<T>& iterator)
        {
            components_ = iterator.components_;
            tree_ = iterator.tree_;
            node_ = iterator.node_;
            index_ = iterator.index_;
            numChildren_ = iterator.numChildren_;
            return *this;
        }

    private:
        T**const components_;
        const ECSEntityTree* tree_;
        const ECSNode* node_;
        u16 index_;
        u16 numChildren_;
    };

    template<class T>
    ECSNodeConstIterator<T>& ECSNodeConstIterator<T>::operator++()
    {
        node_ = &tree_->get(node_->next());
        ++index_;
        return *this;
    }
}
#endif //INC_LFRAMEWORK_ECSTREE_H__
