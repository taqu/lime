#ifndef INC_LCORE_AVLTREE_H_
#define INC_LCORE_AVLTREE_H_
/**
@file AVLTree.h
@author t-sakai
@date 2008/11/13 create
*/
#include "lcore.h"

namespace lcore
{
    enum AVLSub
    {
        AVLSub_Left=0,
        AVLSub_Right=1,
    };

    //---------------------------------------------------------------
    //---
    //--- AVLNodeBase
    //---
    //---------------------------------------------------------------
    template<class Derived>
    class AVLNodeRoot
    {
    public:
        typedef Derived this_type;
        /// コンストラクタ
        AVLNodeRoot()
            :left_(NULL)
            ,right_(NULL)
        {}

        /// コンストラクタ
        AVLNodeRoot(this_type* left, this_type* right)
            :left_(left)
            ,right_(right)
        {}

        /// デストラクタ
        ~AVLNodeRoot()
        {}

        this_type*& sub(s32 s){ return (s==AVLSub_Left)? left_ : right_;}

        /// 左部分木
        this_type* left_;

        /// 右部分木
        this_type* right_;
    };

    /// ノード基底
    template<class T>
    class AVLNodeBase : public AVLNodeRoot<AVLNodeBase<T> >
    {
    public:
        typedef  AVLNodeRoot<AVLNodeBase<T> > base_type;
        typedef AVLNodeBase<T> this_type;

        /// コンストラクタ
        AVLNodeBase()
            :balance_(0)
        {}

        /// コンストラクタ
        AVLNodeBase(this_type* left, this_type* right, const T& value)
            :AVLNodeRoot(left, right)
            ,balance_(0)
            ,value_(value)
        {}

        /// デストラクタ
        ~AVLNodeBase()
        {}

        const T& getValue() const{ return value_;}
        T& getValue(){ return value_;}
        void setValue(const T& value){ value_ = value;}

        /// バランス
        s32 balance_;

        T value_;
    };

    template<class T>
    struct DefaultAVLAllocator
    {
        typedef AVLNodeBase<T> node_type;

        DefaultAVLAllocator()
        {}

        inline node_type* create(const T& value)
        {
            return LNEW node_type(NULL, NULL, value);
        }

        inline void destroy(node_type*& node)
        {
            LDELETE(node);
        }
    };

    //---------------------------------------------------------------
    //---
    //--- AVLTree
    //---
    //---------------------------------------------------------------
    /// AVL木
    template<class T, class Allocator=DefaultAVLAllocator<T>, class Comparator=DefaultComparator<T> >
    class AVLTree
    {
    public:
        static const s32 MaxLevels = 32;

        typedef u32 size_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;
        typedef AVLTree this_type;
        typedef AVLNodeBase<T> node_type;

        typedef Allocator allocator_type;
        typedef Comparator comparator_type;

        AVLTree();
        ~AVLTree();

        bool isEmpty() const{ return (NULL == root_);}

        pointer find(const value_type& value);
        const_pointer find(const value_type& value) const;

        void insert(const value_type& value);
        void remove(const value_type& value);
        void clear();

        void swap(AVLTree& rhs);

        s32 check() const;
        void print();
    private:
        AVLTree(const AVLTree&);
        AVLTree& operator=(const AVLTree&);

        struct Step
        {
            node_type* node_;
            s32 which_;
        };

        void updateBalance(node_type* node);

        node_type* insertInternal(node_type* node, const value_type& value);
        node_type* balanceInsert(node_type* node, Step* path, s32 numLevels);

        node_type* findInternal(node_type* node, Step* path, s32& level, const value_type& value);

        void balanceRemove(Step* path, s32 numLevels);

        void clearInternal(node_type*& node);

        void printInternal(const node_type* node, s32 level) const;

        /// 右回転
        node_type* rotateRight(node_type* node);

        /// 左回転
        node_type* rotateLeft(node_type* node);

        typename node_type::base_type root_;
        allocator_type allocator_;
        comparator_type comparator_;
    };

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    AVLTree<T,Allocator,Comparator>::AVLTree()
        :root_(NULL, NULL)
    {
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    AVLTree<T,Allocator,Comparator>::~AVLTree()
    {
        clear();
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::pointer
        AVLTree<T,Allocator,Comparator>::find(const value_type& value)
    {
        node_type* node = root_.right_;
        while(NULL != node){
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
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::const_pointer
        AVLTree<T,Allocator,Comparator>::find(const value_type& value) const
    {
        const node_type* node = root_.right_;
        while(NULL != node){
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

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::updateBalance(node_type* node)
    {
        LASSERT(NULL != node);
        if(1 == node->balance_){
            node->left_->balance_ = 0;
            node->right_->balance_ = -1;
        }else if( -1 == node->balance_){
            node->left_->balance_ = 1;
            node->right_->balance_ = 0;
        }else{
            node->left_->balance_ = 0;
            node->right_->balance_ = 0;
        }
        node->balance_ = 0;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::insert(const value_type& value)
    {
        root_.right_ = insertInternal(root_.right_, value);
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::remove(const value_type& value)
    {
        s32 numLevels = 1;
        Step path[MaxLevels];

        path[0].node_ = static_cast<node_type*>(&root_);
        path[0].which_ = AVLSub_Right;

        node_type* n = findInternal(root_.right_, path, numLevels, value);
        if(NULL == n){
            return;
        }

        if(NULL == n->right_){
            path[numLevels-1].node_->sub(path[numLevels-1].which_) = n->left_;
        }else{
            node_type* r = n->right_;
            if(NULL == r->left_){
                r->left_ = n->left_;
                r->balance_ = n->balance_;
                path[numLevels-1].node_->sub(path[numLevels-1].which_) = r;
                path[numLevels].node_ = r;
                path[numLevels].which_ = AVLSub_Right;
                ++numLevels;
            }else{
                node_type* left;
                s32 l = numLevels++;
                for(;;){
                    path[numLevels].which_ = AVLSub_Left;
                    path[numLevels].node_ = r;
                    ++numLevels;
                    left = r->left_;
                    if(NULL == left->left_){
                        break;
                    }
                    r = left;
                }
                left->left_ = n->left_;
                r->left_ = left->right_;
                left->right_ = n->right_;
                left->balance_ = n->balance_;

                path[l-1].node_->sub(path[l-1].which_) = left;
                path[l].which_ = AVLSub_Right;
                path[l].node_ = left;
            }
        }
        allocator_.destroy(n);

        balanceRemove(path, numLevels);
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::clear()
    {
        clearInternal(root_.right_);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::clearInternal(node_type*& node)
    {
        if(NULL == node){
            return;
        }
        clearInternal(node->left_);
        clearInternal(node->right_);
        allocator_.destroy(node);
    }

    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::node_type*
        AVLTree<T,Allocator,Comparator>::insertInternal(node_type* node, const value_type& value)
    {
        if(NULL == node){
            // 挿入位置
            return allocator_.create(value);
        }
        Step path[MaxLevels];

        node_type* n = node;
        s32 level = 0;
        for(;;){
            s32 cmp = comparator_(n->getValue(), value);

            if(0 == cmp){
                return node;

            }else if(0<cmp){
                LASSERT(level<MaxLevels);
                path[level].node_ = n;
                path[level].which_ = AVLSub_Left;
                ++level;
                if(NULL == n->left_){
                    n->left_ = allocator_.create(value);
                    break;
                }
                n = n->left_;

            }else{
                LASSERT(level<MaxLevels);
                path[level].node_ = n;
                path[level].which_ = AVLSub_Right;
                ++level;
                if(NULL == n->right_){
                    n->right_ = allocator_.create(value);
                    break;
                }
                n = n->right_;
            }
        }
        return balanceInsert(node, path, level);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::node_type*
        AVLTree<T,Allocator,Comparator>::balanceInsert(node_type* node, Step* path, s32 numLevels)
    {
        node_type* newNode = NULL;
        while(0<numLevels){
            --numLevels;
            node_type* n = path[numLevels].node_;
            s32 which = path[numLevels].which_;
            if(AVLSub_Left == which){
                ++n->balance_;
            }else{
                --n->balance_;
            }
            s32 balance = n->balance_;
            if(0==balance){
                return node;
            }
            if(1<balance){
                if(n->left_->balance_<0){
                    //LR
                    n->left_ = rotateLeft(n->left_);
                    newNode = rotateRight(n);
                    updateBalance(newNode);
                }else{
                    //LL
                    newNode = rotateRight(n);
                    newNode->balance_ = 0;
                    n->balance_ = 0;
                }
                break;

            }else if(balance<-1){
                if(0<n->right_->balance_){
                    //RL
                    n->right_ = rotateRight(n->right_);
                    newNode = rotateLeft(n);
                    updateBalance(newNode);
                }else{
                    //RR
                    newNode = rotateLeft(n);
                    newNode->balance_ = 0;
                    n->balance_ = 0;
                }
                break;
            }
        }//while(0<numLevels)

        if(0<numLevels){
            path[numLevels-1].node_->sub(path[numLevels-1].which_) = newNode;

        }else if(NULL != newNode){
            return newNode;
        }
        return node;
    }

    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::node_type*
        AVLTree<T,Allocator,Comparator>::findInternal(node_type* node, Step* path, s32& level, const value_type& value)
    {
        while(NULL != node){
            s32 cmp = comparator_(node->getValue(), value);

            if(0 == cmp){
                return node;
            }
            if(0<cmp){
                LASSERT(level<MaxLevels);
                path[level].node_ = node;
                path[level].which_ = AVLSub_Left;
                ++level;
                node = node->left_;
            }else{
                LASSERT(level<MaxLevels);
                path[level].node_ = node;
                path[level].which_ = AVLSub_Right;
                ++level;
                node = node->right_;
            }
        }
        return NULL;
    }

    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::balanceRemove(Step* path, s32 numLevels)
    {
        while(0<--numLevels){
            node_type* newNode = NULL;
            node_type* n = path[numLevels].node_;
            s32 which = path[numLevels].which_;
            if(AVLSub_Left == which){
                --n->balance_;
            }else{
                ++n->balance_;
            }
            s32 balance = n->balance_;
            if(1<balance){
                if(n->left_->balance_<0){
                    //LR
                    n->left_ = rotateLeft(n->left_);
                    newNode = rotateRight(n);
                    updateBalance(newNode);
                    path[numLevels-1].node_->sub( path[numLevels-1].which_ ) = newNode;
                }else{
                    //LL
                    newNode = rotateRight(n);
                    path[numLevels-1].node_->sub( path[numLevels-1].which_ ) = newNode;
                    if(0==newNode->balance_){
                        newNode->balance_ = -1;
                        n->balance_ = 1;
                        break;
                    }else{
                        newNode->balance_ = 0;
                        n->balance_ = 0;
                    }
                }

            }else if(balance<-1){
                if(0<n->right_->balance_){
                    //RL
                    n->right_ = rotateRight(n->right_);
                    newNode = rotateLeft(n);
                    updateBalance(newNode);
                    path[numLevels-1].node_->sub( path[numLevels-1].which_ ) = newNode;
                }else{
                    //RR
                    newNode = rotateLeft(n);
                    path[numLevels-1].node_->sub( path[numLevels-1].which_ ) = newNode;
                    if(0 == newNode->balance_){
                        newNode->balance_ = 1;
                        n->balance_ = -1;
                        break;
                    }else{
                        newNode->balance_ = 0;
                        n->balance_ = 0;
                    }
                }

            }else if(0 != balance){
                break;
            }
        }//while(0<numLevels)
    }

    //---------------------------------------------------------------
    // 右回転
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::node_type*
        AVLTree<T,Allocator,Comparator>::rotateRight(node_type* node)
    {
        LASSERT(NULL != node);

        node_type* left = node->left_;
        LASSERT(NULL != left); //右回転する場合必ず存在

        node->left_ = left->right_;
        left->right_ = node;

        return left;
    }


    //---------------------------------------------------------------
    // 左回転
    template<class T, class Allocator, class Comparator>
    typename AVLTree<T,Allocator,Comparator>::node_type*
        AVLTree<T,Allocator,Comparator>::rotateLeft(node_type* node)
    {
        LASSERT(NULL != node);

        node_type* right = node->right_;
        LASSERT(NULL != right); //左回転する場合必ず存在

        node->right_ = right->left_;
        right->left_ = node;

        return right;
    }


    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::print()
    {
        printInternal(root_.right_, 0);
    }

    //---------------------------------------------------------------
    template<class T, class Allocator, class Comparator>
    void AVLTree<T,Allocator,Comparator>::printInternal(const node_type* node, s32 level) const
    {
        if(NULL == node){
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
#endif //INC_LCORE_AVLTREE_H_
