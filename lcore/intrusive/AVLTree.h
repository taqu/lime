#ifndef INC_LCORE_AVLTREE_H__
#define INC_LCORE_AVLTREE_H__
/**
@file AVLTree.h
@author t-sakai
@date 2008/11/13 create
*/
#include "Container.h"
#include "AllocatorMalloc.h"

namespace lcore
{
namespace intrusive
{
    template<class ValueType, class Predicate, class Allocator> class AVLTree;

    template<class T>
    struct DefaultPredicate
    {
        /**
        @brief 比較関数
        @return 0 = equal, -1 = less, 1 = more
        */
        static s32 operator()(const T& v1, const T& v2)
        {
            return (v1 == v2)? 0 : (v1 < v2) ? -1 : 1;
        }
    };

    //---------------------------------------------------------------
    //---
    //--- AVLNodeBase
    //---
    //---------------------------------------------------------------
    /// ノード基底
    template<class T, class Allocator>
    class AVLNodeBase
    {
    private:
        friend class AVLTree<T, Allocator>;

        /// コンストラクタ
        AVLNodeBase()
            :left_(0),
            right_(0),
            height_(0)
        {
        }

        /// コンストラクタ
        AVLNodeBase(Node* left, Node* right)
            :left_(left),
            right_(right),
            height_(1)
        {
        }

        /// デストラクタ
        virtual ~AVLNodeBase()
        {
        }

        /// 左部分木
        AVLNodeBase *left_;

        /// 右部分木
        AVLNodeBase *right_;

        /// ノードまでの高さ
        s32 height_;
    };


    //---------------------------------------------------------------
    //---
    //--- AVLTree
    //---
    //---------------------------------------------------------------
    /// AVL木
    template<class ValueType,
    class Predicate = DefaultPredicate<ValueType>,
    class Allocator = AllocatorMalloc< AVLNodeBase<ValueType> > >
    class AVLTree : protected Container
    {
    public:
        typedef size_t size_type;
        typedef ValueType* pointer;
        typedef const ValueType* const_pointer;
        typedef ValueType& reference;
        typedef const ValueType& const_reference;
        typedef ValueType value_type;
        typedef AVLTree<ValueType, Allocator> this_type;
        typedef AVLNodeBase<ValueType, Allocator> node_type;

        AVLTree()
            :root_(0)
        {
        }

        ~AVLTree()
        {
            if(root_ != 0){
                eraseAll();
            }
        }

        bool isEmpty() const{ return (root_ == 0);}

        void insert(value_type& value)
        {
            root_ = insert(root_, value);
        }

        void erase(value_type& value)
        {
            root_ = erase(root_, value);
        }

        void eraseAll()
        {
            eraseNode(root_);
            root_ = 0;
        }

        const value_type* search(const value_type& key) const;

        template<class EqualFunc>
        const value_type* search(const EqualFunc& func);

        const value_type& getMin() const
        {
            LASSERT(root_ != 0);
            node_type* node = root_;
            while(node->left_ != 0){
                node = node->left_;
            }
            return *node;
        }

        const value_type& getMax() const
        {
            LASSERT(root_ != 0);
            node_type* node = root_;
            while(node->right_ != 0){
                node = node->right_;
            }
            return *node;
        }


    private:
        value_type* insert(node_type* node, value_type& value);
        node_type* balance(node_type* node);

        value_type* erase(node_type* node, value_type& value);

        void eraseNode(node_type* node);

        node_type* popMaxNode(node_type* node, node_type** maxNode);

        /// 右回転
        node_type* rotateRight(node_type* node);

        /// 左回転
        node_type* rotateLeft(node_type* node);

        inline s32 nodeHeight(node_type* node)
        {
            return (node != 0)? node->height_ : 0;
        }

        inline s32 calcHeight(const node_type* node)
        {
            return maximum( nodeHeight(node->left_), nodeHeight(node->right_) ) + 1;
        }

        static Allocator _allocator;

        node_type *root_;
    };

    //---------------------------------------------------------------
    // アロケータ実体
    template<class ValueType, class Predicate, class Allocator>
    Allocator AVLTree<ValueType, Allocator>::_allocator;


    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::popMaxNode(node_type* node, node_type** maxNode)
    {
        LASSERT(node != NULL);
        if(node->right_ == 0){
            *maxNode = node;
            return node->left_;
        }
        node = popMaxNode(node->right_, maxNode);

        return node;
    }


    //---------------------------------------------------------------
    // 右回転
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::rotateRight(node_type* node)
    {
        LASSERT(node != 0);

        node_type* left = node->left_;
        LASSERT(left != 0); //右回転する場合必ず存在

        node->_left = left->rigt_;
        left->right_ = node;

        node->height_ = calcHeight(node); //高さを更新
        return left;
    }


    //---------------------------------------------------------------
    // 左回転
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::rotateLeft(node_type* node)
    {
        P_ASSERT(node != 0);

        node_type* right = node->_right;
        P_ASSERT(right != 0); //左回転する場合必ず存在

        node->_right = right->_left;
        right->_left = node;

        node->_height = calcHeight(node); //高さを更新
        return right;
    }


    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::insert(node_type* node, value_type& value)
    {
        if(node == 0){
            // 挿入位置
            return &value;
        }

        value_type &nodeVal = (value_type)*node;

        s32 ret = Predicate(value, nodeVal);

        if( ret==0 ){
            return node;

        }else if( ret<0 ){
            node->left_ = insert(node->left_, value);

        }else{
            node->right_ = insert(node->right_, value);
        }

        node = balance(node); //左右部分木バランスどり
        return node;
    }


    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::balance(node_type* node)
    {
        LASSERT(node != 0);

        s32 diff = nodeHeight(node->right_) - nodeHeight(node->left_);
        node_type *ret = node;

        if(diff < -1){
            node_type *child = node->left_;
            s32 diff2 = nodeHeight(child->right_) - nodeHeight(child->left_);
            if(diff2 > 0){
                //LR２重回転
                node->left_ = rotateLeft(child);
            }
            //LL１重回転
            ret =  rotateRight(node);

        }else if(diff > 1){
            node_type *child = node->right_;
            s32 diff2 = nodeHeight(child->left_) - nodeHeight(child->right_);
            if(diff2 > 0){
                //RL２重回転
                node->right_ = rotateRight(child);
            }
            //RR１重回転
            ret = rotateLeft(node);
        }

        ret->height_ = calcHeight(ret); //高さ更新
        return ret;
    }


    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    typename AVLTree<ValueType, Predicate, Allocator>::node_type*
        AVLTree<ValueType, Predicate, Allocator>::erase(node_type* node, value_type& value)
    {
        if(node == 0){
            // 見つからなかった
            return 0;
        }

        value_type &nodeVal = (value_type)*node;
        s32 ret = Predicate(value, nodeVal);

        if( ret==0 ){
            // 削除ノード
            node_type *oldNode = node;
            //左の木から最大値のノードポップ
            if(oldNode->left_ != 0){
                oldNode->left_ = popMaxNode(oldNode->left_, &node);
                node->left_ = oldNode->left_;
                node->right_ = oldNode->right_;
            }else{
                node = oldNode->right_;
            }
            _allocator.destroy(oldNode);
            _allocator.deallocate(oldNode, sizeof(value_type));

        }else if( ret<0 ){
            node->left_ = erase(node->left_, value);

        }else{
            node->right_ = erase(node->right_, value);
        }

        if(node != 0){
            node = balance(node); //左右部分木バランスどり
        }
        return node;
    }

    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    void AVLTree<ValueType, Predicate, Allocator>::eraseNode(node_type* node)
    {
        if(node != 0){
            eraseNode(node->left_);
            eraseNode(node->right_);
            _allocator.destroy(node);
            _allocator.deallocate(node, sizeof(ValueType));
        }
    }

    //---------------------------------------------------------------
    template<class ValueType, class Predicate, class Allocator>
    const typename AVLTree<ValueType, Predicate, Allocator>::value_type*
        AVLTree<ValueType, Predicate, Allocator>::search(const value_type& key) const
    {
        node_type* node = root_;
        while(node != 0){
            value_type &nodeVal = (value_type)*node;
            s32 ret = Predicate(value, nodeVal);
            if( ret==0 ){
                break;
            }else if( ret<0 ){
                node = node->right_;
            }else{
                node = node->left_;
            }
        }
        return (value_type*)node;
    }

    template<class ValueType, class Predicate, class Allocator, class EqualFunc>
    const typename AVLTree<ValueType, Predicate, Allocator>::value_type*
        AVLTree<ValueType, Predicate, Allocator>::search(const EqualFunc& func)
    {
        node_type* node = root_;
        while(node != 0){
            value_type &nodeVal = (value_type)*node;
            s32 ret = func(nodeVal, key);
            if( ret==0 ){
                break;
            }else if( ret < 0 ){
                node = node->right_;
            }else{
                node = node->left_;
            }
        }
        return (value_type*)node;
    }

}
}

#endif //INC_LCORE_AVLTREE_H__
