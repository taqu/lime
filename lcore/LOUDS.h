#ifndef INC_LCORE_LOUDS_H__
#define INC_LCORE_LOUDS_H__
/**
@file LOUDS.h
@author t-sakai
@date 2016/08/02 create
*/
#include "lcore.h"
#include "Array.h"

namespace lcore
{
    //------------------------------------------------------------
    //---
    //--- Node
    //---
    //------------------------------------------------------------
    template<class T>
    class Node
    {
    public:
        Node();
        Node(const Node& node);
        ~Node();

        void add(s32 child);
        inline s32 getNumChildren() const;
        inline s32 getChild(s32 index) const;

        s32 getIndex() const{ return index_;}
        void setIndex(s32 index){ index_ = index;}

        const T& getLabel() const{ return data_;}
        void setLabel(const T& data){ data_ = data;}
    private:
        Node& operator=(const Node&);

        s32* children_;
        s16 numCapacity_;
        s16 numChildren_;
        s32 index_;
        T data_;
    };

    template<class T>
    Node<T>::Node()
        :children_(NULL)
        ,numCapacity_(0)
        ,numChildren_(0)
        ,index_(-1)
    {}

    template<class T>
    Node<T>::Node(const Node& node)
        :numCapacity_(node.numCapacity_)
        ,numChildren_(node.numChildren_)
    {
        children_ = (s32*)LMALLOC(sizeof(s32)*numCapacity_);
        lcore::memcpy(children_, node.children_, sizeof(s32)*numCapacity_);
        index_ = node.index_;
        data_ = node.data_;
    }

    template<class T>
    Node<T>::~Node()
    {
        LFREE(children_);
    }

    template<class T>
    void Node<T>::add(s32 child)
    {
        if(numCapacity_<=numChildren_){
            s16 newCapacity = numCapacity_+4;
            s32* children = (s32*)LMALLOC(sizeof(s32)*newCapacity);
            lcore::memcpy(children, children_, sizeof(s32)*numCapacity_);
            LFREE(children_);
            numCapacity_ = newCapacity;
            children_ = children;
        }
        children_[numChildren_] = child;
        ++numChildren_;
    }

    template<class T>
    inline s32 Node<T>::getNumChildren() const
    {
        return numChildren_;
    }

    template<class T>
    inline s32 Node<T>::getChild(s32 index) const
    {
        return children_[index];
    }

    //------------------------------------------------------------
    //---
    //--- Tree
    //---
    //------------------------------------------------------------
    template<class T>
    class Tree
    {
    public:
        typedef Node<T> node_type;

        Tree();
        ~Tree();

        const node_type* getRoot() const;
        node_type* getRoot();
        node_type* add(node_type* parent);
        const node_type* get(s32 index) const;
        node_type* get(s32 index);

    private:
        Tree(const Tree&);
        Tree& operator=(const Tree&);

        node_type* addRoot();
        void resize();

        s32 capacity_;
        s32 size_;
        node_type* nodes_;
    };

    template<class T>
    Tree<T>::Tree()
        :capacity_(0)
        ,size_(0)
        ,nodes_(NULL)
    {
        addRoot();
    }

    template<class T>
    Tree<T>::~Tree()
    {
        for(s32 i=0; i<capacity_; ++i){
            nodes_[i].~node_type();
        }
        LFREE(nodes_);
    }

    template<class T>
    typename Tree<T>::node_type* Tree<T>::addRoot()
    {
        if(0<size_){
            return getRoot();
        }
        resize();
        size_ = 1;
        return &nodes_[0];
    }

    template<class T>
    const typename Tree<T>::node_type* Tree<T>::getRoot() const
    {
        return (size_<=0)? NULL : &nodes_[0];
    }

    template<class T>
    typename Tree<T>::node_type* Tree<T>::getRoot()
    {
        return (size_<=0)? NULL : &nodes_[0];
    }

    template<class T>
    typename Tree<T>::node_type* Tree<T>::add(node_type* parent)
    {
        LASSERT(NULL != parent);
        if(capacity_<=size_){
            resize();
        }
        parent->add(size_);
        node_type* node = nodes_ + size_;
        ++size_;
        return node;
    }

    template<class T>
    const typename Tree<T>::node_type* Tree<T>::get(s32 index) const
    {
        LASSERT(0<=index && index<size_);
        return &nodes_[index];
    }

    template<class T>
    typename Tree<T>::node_type* Tree<T>::get(s32 index)
    {
        LASSERT(0<=index && index<size_);
        return &nodes_[index];
    }

    template<class T>
    void Tree<T>::resize()
    {
        s32 capacity = capacity_ + 16;
        node_type* nodes = (node_type*)LMALLOC(sizeof(node_type)*capacity);
        for(s32 i=0; i<capacity_; ++i){
            LPLACEMENT_NEW(&nodes[i]) node_type(nodes_[i]);
            nodes_[i].~node_type();
        }
        for(s32 i=capacity_; i<capacity; ++i){
            LPLACEMENT_NEW(&nodes[i]) node_type();
        }
        LFREE(nodes_);
        capacity_ = capacity;
        nodes_ = nodes;
    }

    //------------------------------------------------------------
    //---
    //---
    //---
    //------------------------------------------------------------
    template<class T, class U>
    void traverse(Tree<T>& tree, U& proc)
    {
        typedef typename Tree<T>::node_type node_type;
        node_type* node = tree.getRoot();
        if(NULL == node){
            return ;
        }

        ArrayPOD<s32> indices;
        indices.push_front(0);
        while(0<indices.size()){
            s32 index = indices[indices.size()-1];
            indices.pop_back();
            node = tree.get(index);
            for(s32 i=0; i<node->getNumChildren(); ++i){
                indices.push_front(node->getChild(i));
            }
            if(false == proc(index, *node)){
                break;
            }
        }
    }

    //------------------------------------------------------------
    //---
    //--- ByteTrie
    //---
    //------------------------------------------------------------
    class ByteTrie
    {
    public:
        typedef Tree<Char> tree_type;
        typedef Tree<Char>::node_type node_type;

        ByteTrie();
        ~ByteTrie();

        bool add(const Char* key, const Char* value);
        const Char* find(const Char* key) const;
        Char getLabel(const node_type& node) const
        {
            return node.getLabel();
        }

        const Char* getValue(const node_type& node) const
        {
            return (0<=node.getIndex())? values_[node.getIndex()] : NULL;
        }

        template<class T>
        void traverse(T& proc) const;
    private:
        ByteTrie(const ByteTrie&);
        ByteTrie& operator=(const ByteTrie&);

        tree_type tree_;
        ArrayPOD<Char*> values_;
    };

    template<class T>
    void ByteTrie::traverse(T& proc) const
    {
        const node_type* node = tree_.getRoot();

        ArrayPOD<s32> indices;
        indices.push_front(0);
        s32 count = 0;
        while(0<indices.size()){
            s32 index = indices[indices.size()-1];
            indices.pop_back();
            node = tree_.get(index);
            for(s32 i=0; i<node->getNumChildren(); ++i){
                indices.push_front(node->getChild(i));
            }
            if(false == proc(count, *node)){
                break;
            }
            ++count;
        }
    }

    //------------------------------------------------------------
    //---
    //--- FullyIndexableDictionary
    //---
    //------------------------------------------------------------
    class FullyIndexableDictionary
    {
    public:
        static const u32 LShift = 8;
        static const u32 LSize = 256;
        static const u32 SShift = 3;
        static const u32 SSize = 8;
        static const u32 NumSInL = LSize/SSize;

        FullyIndexableDictionary();
        explicit FullyIndexableDictionary(u32 sizeInBits);
        ~FullyIndexableDictionary();

        void clear();
        inline u32 sizeInBits() const;

        void build();

        u32 access(u32 index) const;

        void set(u32 index);
        void reset(u32 index);

        void add(u32 index, bool flag);
        void add(bool flag)
        {
            add(sizeInBits_, flag);
        }

        u32 rank_one(s32 i) const;
        u32 rank_zero(s32 i) const;
        u32 select_one(s32 i) const;
        u32 select_zero(s32 i) const;

        u32 rank_one_brute_force(s32 i) const;
        u32 rank_zero_brute_force(s32 i) const;
        u32 select_one_brute_force(s32 i) const;
        u32 select_zero_brute_force(s32 i) const;
    private:
        FullyIndexableDictionary(const FullyIndexableDictionary&);
        FullyIndexableDictionary& operator=(const FullyIndexableDictionary&);

        static const u32 UnitBytes = 16;
        static const u32 UnitBits = UnitBytes<<3;

        u32 popcount(const u32 v[8]) const;

        u32 sizeInBits_;
        u32 size_;
        u8* bits_;
        u32 sizeL_;
        u32* L1_;
        u32 sizeS_;
        u8* S1_;
    };

    inline u32 FullyIndexableDictionary::sizeInBits() const
    {
        return sizeInBits_;
    }

    //------------------------------------------------------------
    //---
    //--- LOUDS
    //---
    //------------------------------------------------------------
    class LOUDS
    {
    public:
        LOUDS();
        ~LOUDS();

        void build(const ByteTrie& trie);

        const Char* find(const Char* key) const;
    private:
        LOUDS(const LOUDS&);
        LOUDS& operator=(const LOUDS&);

        struct TraverseTrie
        {
            TraverseTrie(LOUDS& louds, const ByteTrie& trie)
                :louds_(louds)
                ,trie_(trie)
            {}

            bool operator()(s32 index, const ByteTrie::node_type& node);

            LOUDS& louds_;
            const ByteTrie& trie_;

        private:
            TraverseTrie(const TraverseTrie&);
            TraverseTrie& operator=(const TraverseTrie&);
        };

        friend struct TraverseTrie;

        void clear();

        FullyIndexableDictionary fid_;
        ArrayPOD<Char> labels_;
        ArrayPOD<Char*> values_;
    };
}
#endif //INC_LCORE_LOUDS_H__
