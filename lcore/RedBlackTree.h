#ifndef INC_LCORE_REDBLACKTREE_H_
#define INC_LCORE_REDBLACKTREE_H_
/**
@file RedBlackTree.h
@author t-sakai
@date 2008/11/13 create
*/
#include "lcore.h"

namespace lcore
{
    enum RBColor
    {
        RBColor_Black = 0,
        RBColor_Red = 1,
    };

    //---------------------------------------------------------------
    //---
    //--- RedBlackTreeNodeBase
    //---
    //---------------------------------------------------------------
    /// ノード基底
    template<class T>
    class RedBlackTreeNodeBase
    {
    public:
        typedef RedBlackTreeNodeBase<T> this_type;

        RedBlackTreeNodeBase()
        {}

        /// コンストラクタ
        RedBlackTreeNodeBase(this_type* left, this_type* right, s32 color, const T& value)
            :left_(left)
            ,right_(right)
            ,color_(color)
            ,value_(value)
        {}

        /// デストラクタ
        ~RedBlackTreeNodeBase()
        {}

        const T& getValue() const{ return value_;}
        T& getValue(){ return value_;}
        void setValue(const T& value){ value_ = value;}

        bool isRed() const{ return color_ == RBColor_Red;}
        bool isBlack() const{ return color_ == RBColor_Black;}

        void setRed(){ color_ = RBColor_Red;}
        void setBlack(){ color_ = RBColor_Black;}

        /// 左部分木
        this_type* left_;

        /// 右部分木
        this_type* right_;

        /// 色
        s32 color_;

        T value_;
    };

    template<class T>
    struct DefaultRedBlackTreeAllocator
    {
        typedef RedBlackTreeNodeBase<T> node_type;

        DefaultRedBlackTreeAllocator(node_type* ptr)
            :nullptr_(ptr)
        {}

        node_type* nullptr_;

        inline node_type* create(const T& value)
        {
            return TNEW node_type(nullptr_, nullptr_, RBColor_Red, value);
        }

        inline void destroy(node_type*& node)
        {
            TDELETE(node);
            node = nullptr_;
        }
    };

    //---------------------------------------------------------------
    //---
    //--- RedBlackTree
    //---
    //---------------------------------------------------------------
    /// 赤黒木
    template<class T, class Allocator=DefaultRedBlackTreeAllocator<T>, class Comparator=DefaultComparator<T>, class Traversal=DefaultTraversal<T> >
    class RedBlackTree
    {
    public:
        typedef u32 size_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;
        typedef RedBlackTree this_type;
        typedef RedBlackTreeNodeBase<T> node_type;

        typedef Allocator allocator_type;
        typedef Comparator comparator_type;
        typedef Traversal traversal_type;

        RedBlackTree();
        ~RedBlackTree();

        bool isEmpty() const{ return (NULL == root_);}

        pointer find(const value_type& value);
        const_pointer find(const value_type& value) const;

        /**
        \brief x<=valueを返す
        */
        pointer findLessEqual(const value_type& value);

        /**
        \brief x<=valueを返す
        */
        const_pointer findLessEqual(const value_type& value) const;

        /**
        \brief value<=xを返す
        */
        pointer findGreaterEqual(const value_type& value);

        /**
        \brief value<=xを返す
        */
        const_pointer findGreaterEqual(const value_type& value) const;

        void insert(const value_type& value);
        void remove(const value_type& value);
        void clear();

        void swap(RedBlackTree& rhs);

        inline void traverse(traversal_type& func);

        void initAllocator(){ allocator_.initialize();}
        void termAllocator(){ allocator_.terminate();}

        s32 check() const;
        void print();
    private:
        RedBlackTree(const RedBlackTree&);
        RedBlackTree& operator=(const RedBlackTree&);

        bool insertInternal(node_type*& node, const value_type& value);

        bool balanceInsertLeft(node_type*& node);
        bool balanceInsertRight(node_type*& node);
        void setRed(node_type* node);

        node_type* removeInternal(bool& result, node_type* node, const value_type& value);

        node_type* balanceRemoveLeft(bool& result, node_type* node, bool flag);
        node_type* balanceRemoveRight(bool& result, node_type* node, bool flag);
        const value_type& findMin(node_type* node) const;

        void clearInternal(node_type*& node);
        void traverseInternal(node_type* node, traversal_type& func);

        s32 checkInternal(const node_type* node) const;
        void printInternal(const node_type* node, s32 level) const;

        /// 右回転
        node_type* rotateRight(node_type* node);

        /// 左回転
        node_type* rotateLeft(node_type* node);

        node_type null_;
        node_type* nullptr_;
        node_type* root_;
        allocator_type allocator_;
        comparator_type comparator_;
    };

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    RedBlackTree<T,Allocator,Comparator, Traversal>::RedBlackTree()
        :null_(NULL, NULL, false, 0)
        ,nullptr_(&null_)
        ,root_(nullptr_)
        ,allocator_(nullptr_)
    {
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    RedBlackTree<T,Allocator,Comparator, Traversal>::~RedBlackTree()
    {
        clear();
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::find(const value_type& value)
    {
        node_type* node = root_;
        while(nullptr_ != node){
            s32 cmp = comparator_(node->getValue(), value);
            if(cmp == 0){
                return &node->getValue();
            }else if(cmp<0){
                node = node->right_;
            }else{
                node = node->left_;
            }
        }
        return NULL;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::const_pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::find(const value_type& value) const
    {
        return const_cast<this_type*>(this)->find(value);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::findLessEqual(const value_type& value)
    {
        node_type* node = root_;
        while(nullptr_ != node){
            s32 cmp = comparator_(node->getValue(), value);
            if(cmp == 0){
                return &node->getValue();
            }else if(cmp<0){
                while(nullptr_ != node->right_){
                    node = node->right_;
                }
                return &node->getValue();
            }else{
                node = node->left_;
            }
        }
        return NULL;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::const_pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::findLessEqual(const value_type& value) const
    {
        return const_cast<this_type*>(this)->findLessEqual(value);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::findGreaterEqual(const value_type& value)
    {
        node_type* node = root_;
        while(nullptr_ != node){
            s32 cmp = comparator_(node->getValue(), value);
            if(cmp == 0){
                return &node->getValue();
            }else if(cmp<0){
                node = node->right_;
            }else{
                while(nullptr_ != node->left_){
                    node = node->left_;
                }
                return &node->getValue();
            }
        }
        return NULL;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::const_pointer
        RedBlackTree<T,Allocator,Comparator, Traversal>::findGreaterEqual(const value_type& value) const
    {
        return const_cast<this_type*>(this)->findGreaterEqual(value);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::insert(const value_type& value)
    {
        insertInternal(root_, value);
        root_->setBlack();
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::remove(const value_type& value)
    {
        bool result;
        root_ = removeInternal(result, root_, value);
        if(root_){
            root_->setBlack();
        }
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::clear()
    {
        clearInternal(root_);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::swap(RedBlackTree& rhs)
    {
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    inline void RedBlackTree<T,Allocator,Comparator, Traversal>::traverse(traversal_type& func)
    {
        traverseInternal(root_, func);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::clearInternal(node_type*& node)
    {
        if(nullptr_ == node){
            return;
        }
        clearInternal(node->left_);
        clearInternal(node->right_);
        allocator_.destroy(node);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::traverseInternal(node_type* node, traversal_type& func)
    {
        if(nullptr_ == node){
            return;
        }
        traverseInternal(node->left_, func);
        func(node->getValue());
        traverseInternal(node->right_, func);
    }


    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    bool RedBlackTree<T,Allocator,Comparator, Traversal>::insertInternal(node_type*& node, const value_type& value)
    {
        if(nullptr_ == node){
            node = allocator_.create(value);
            return false;
        }

        s32 cmp = comparator_(node->getValue(), value);

        if(0<cmp){
            if(insertInternal(node->left_, value)){
                return true;
            }
            if(node->isRed()){
                return false;
            }
            return balanceInsertLeft(node);
        }else if(cmp<0){
            if(insertInternal(node->right_, value)){
                return true;
            }
            if(node->isRed()){
                return false;
            }
            return balanceInsertRight(node);
        }
        return true;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    bool RedBlackTree<T,Allocator,Comparator, Traversal>::balanceInsertLeft(node_type*& node)
    {
        LASSERT(nullptr_ != node);
        LASSERT(node->isBlack());
        if(node->left_->right_->isRed()){
            node->left_ = rotateLeft(node->left_);
        }
        if(node->left_->left_->isRed()){
            if(node->right_->isRed()){
                setRed(node);
                return false;
            }else{
                node = rotateRight(node);
            }
        }
        return true;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    bool RedBlackTree<T,Allocator,Comparator, Traversal>::balanceInsertRight(node_type*& node)
    {
        LASSERT(nullptr_ != node);
        LASSERT(node->isBlack());
        if(node->right_->left_->isRed()){
            node->right_ = rotateRight(node->right_);
        }
        if(node->right_->right_->isRed()){
            if(node->left_->isRed()){
                setRed(node);
                return false;
            }else{
                node = rotateLeft(node);
            }
        }
        return true;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::setRed(node_type* node)
    {
        node->setRed();
        node->left_->setBlack();
        node->right_->setBlack();
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::node_type*
        RedBlackTree<T,Allocator,Comparator, Traversal>::removeInternal(bool& result, node_type* node, const value_type& value)
    {
        result = true;
        if(nullptr_ == node){
            return nullptr_;
        }

        s32 cmp = comparator_(node->getValue(), value);
        if(0<cmp){
            node->left_ = removeInternal(result, node->left_, value);
            return balanceRemoveLeft(result, node, result);

        }else if(cmp<0){
            node->right_ = removeInternal(result, node->right_, value);
            return balanceRemoveRight(result, node, result);
        }

        node_type* current = node;
        if(nullptr_ == current->left_ && nullptr_ == current->right_){
            result = current->isRed();
            allocator_.destroy(current);
            return nullptr_;

        }else if(nullptr_ == current->right_){
            current->left_->setBlack();
            node = node->left_;
            allocator_.destroy(current);
            return node;

        }else if(nullptr_ == current->left_){
            current->right_->setBlack();
            node = node->right_;
            allocator_.destroy(current);
            return node;

        }

        node->value_ = findMin(node->right_);
        node->right_ = removeInternal(result, node->right_, node->getValue());
        return balanceRemoveRight(result, node, result);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::node_type*
        RedBlackTree<T,Allocator,Comparator, Traversal>::balanceRemoveLeft(bool& result, node_type* node, bool flag)
    {
        if(flag){
            result = flag;
            return node;
        }
        if(node->right_->left_->isBlack() && node->right_->right_->isBlack()){
            if(node->right_->isBlack()){
                node->right_->setRed();
                if(node->isBlack()){
                    result = false;
                    return node;
                }
                node->setBlack();
            }else{
                node = rotateLeft(node);
                bool tmp;
                node->left_ = balanceRemoveLeft(tmp, node->left_, false);
            }
        }else{
            if(node->right_->left_->isRed()){
                node->right_ = rotateRight(node->right_);
            }
            node = rotateLeft(node);
            node->left_->setBlack();
            node->right_->setBlack();
        }
        result = true;
        return node;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::node_type*
        RedBlackTree<T,Allocator,Comparator, Traversal>::balanceRemoveRight(bool& result, node_type* node, bool flag)
    {
        if(flag){
            result = flag;
            return node;
        }

        if(node->left_->left_->isBlack() && node->left_->right_->isBlack()){
            if(node->left_->isBlack()){
                node->left_->setRed();
                if(node->isBlack()){
                    result = false;
                    return node;
                }
                node->setBlack();
            }else{
                node = rotateRight(node);
                bool tmp;
                node->right_ = balanceRemoveRight(tmp, node->right_, false);
            }
        }else{
            if(node->left_->right_->isRed()){
                node->left_ = rotateLeft(node->left_);
            }
            node = rotateRight(node);
            node->left_->setBlack();
            node->right_->setBlack();
        }
        result = true;
        return node;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    typename const RedBlackTree<T,Allocator,Comparator, Traversal>::value_type&
        RedBlackTree<T,Allocator,Comparator, Traversal>::findMin(node_type* node) const
    {
        while(nullptr_ != node->left_){
            node = node->left_;
        }
        return node->value_;
    }

    //---------------------------------------------------------------
    // 右回転
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::node_type*
        RedBlackTree<T,Allocator,Comparator, Traversal>::rotateRight(node_type* node)
    {
        LASSERT(nullptr_ != node);

        node_type* left = node->left_;
        LASSERT(nullptr_ != left); //右回転する場合必ず存在

        node->left_ = left->right_;
        
        left->right_ = node;
        left->color_ = node->color_;
        node->setRed();
        return left;
    }


    //---------------------------------------------------------------
    // 左回転
    template<class T, class Allocator, class Comparator, class Traversal>
    typename RedBlackTree<T,Allocator,Comparator, Traversal>::node_type*
        RedBlackTree<T,Allocator,Comparator, Traversal>::rotateLeft(node_type* node)
    {
        LASSERT(nullptr_ != node);

        node_type* right = node->right_;
        LASSERT(nullptr_ != right); //左回転する場合必ず存在

        node->right_ = right->left_;

        right->left_ = node;
        right->color_ = node->color_;
        node->setRed();
        return right;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    s32 RedBlackTree<T,Allocator,Comparator, Traversal>::check() const
    {
        return checkInternal(root_);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    s32 RedBlackTree<T,Allocator,Comparator, Traversal>::checkInternal(const node_type* node) const
    {
        if(nullptr_ == node){
            return 0;
        }

        if(node->isRed()){
            if(node->left_->isRed() || node->right_->isRed()){
                return -1;
            }
        }
        s32 l = checkInternal(node->left_);
        s32 r = checkInternal(node->right_);
        if(l != r){
            return -1;
        }
        if(l<0){
            return -1;
        }

        if(node->isBlack()){
            ++l;
        }
        return l;
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::print()
    {
        printInternal(root_, 0);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator, class Traversal>
    void RedBlackTree<T,Allocator,Comparator, Traversal>::printInternal(const node_type* node, s32 level) const
    {
        if(nullptr_ == node){
            return;
        }
        printInternal(node->left_, level+1);
        for(s32 i=0; i<level; ++i){
            std::cout << ' ';
        }
        std::cout << node->value_ << std::endl;
        printInternal(node->right_, level+1);
    }

}
#endif //INC_LCORE_REDBLACKTREE_H_
