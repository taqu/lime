#ifndef INC_LCORE_AVLTREE_H__
#define INC_LCORE_AVLTREE_H__
/**
@file AVLTree.h
@author t-sakai
@date 2008/11/13 create
*/
#include "../lcore.h"

namespace lcore
{
namespace intrusive
{
    template<class ValueType, class Predicate> class AVLTree;

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
    template<class T>
    class AVLNodeBase
    {
    private:
        friend class AVLTree<T>;

        /// コンストラクタ
        AVLNodeBase()
            :left_(NULL),
            right_(NULL),
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
    template<class ValueType, class Predicate = DefaultPredicate<ValueType> >
    class AVLTree : protected Container
    {
    public:
        typedef u32 size_type;
        typedef ValueType* pointer;
        typedef const ValueType* const_pointer;
        typedef ValueType& reference;
        typedef const ValueType& const_reference;
        typedef ValueType value_type;
        typedef AVLTree<ValueType> this_type;
        typedef AVLNodeBase<ValueType> node_type;

        AVLTree()
            :root_(NULL)
        {
        }

        ~AVLTree()
        {
            if(NULL != root_){
                eraseAll();
            }
        }

        bool isEmpty() const{ return (NULL == root_);}

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
            root_ = NULL;
        }

        const value_type* search(const value_type& key) const;

        template<class EqualFunc>
        const value_type* search(const EqualFunc& func);

        const value_type& getMin() const
        {
            LASSERT(NULL != root_);
            node_type* node = root_;
            while(NULL != node->left_){
                node = node->left_;
            }
            return *node;
        }

        const value_type& getMax() const
        {
            LASSERT(NULL != root_);
            node_type* node = root_;
            while(NULL != node->right_){
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
            return (NULL != node)? node->height_ : 0;
        }

        inline s32 calcHeight(const node_type* node)
        {
            return maximum( nodeHeight(node->left_), nodeHeight(node->right_) ) + 1;
        }

        node_type *root_;
    };

    //---------------------------------------------------------------
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::popMaxNode(node_type* node, node_type** maxNode)
    {
        LASSERT(node != NULL);
        if(NULL == node->right_){
            *maxNode = node;
            return node->left_;
        }
        node = popMaxNode(node->right_, maxNode);

        return node;
    }


    //---------------------------------------------------------------
    // 右回転
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::rotateRight(node_type* node)
    {
        LASSERT(NULL != node);

        node_type* left = node->left_;
        LASSERT(NULL != left); //右回転する場合必ず存在

        node->_left = left->rigt_;
        left->right_ = node;

        node->height_ = calcHeight(node); //高さを更新
        return left;
    }


    //---------------------------------------------------------------
    // 左回転
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::rotateLeft(node_type* node)
    {
        LASSERT(NULL != node);

        node_type* right = node->_right;
        LASSERT(NULL != right); //左回転する場合必ず存在

        node->_right = right->_left;
        right->_left = node;

        node->_height = calcHeight(node); //高さを更新
        return right;
    }


    //---------------------------------------------------------------
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::insert(node_type* node, value_type& value)
    {
        if(NULL == node){
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
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::balance(node_type* node)
    {
        LASSERT(NULL != node);

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
    template<class ValueType, class Predicate>
    typename AVLTree<ValueType, Predicate>::node_type*
        AVLTree<ValueType, Predicate>::erase(node_type* node, value_type& value)
    {
        if(NULL == node){
            // 見つからなかった
            return NULL;
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
            LIME_DELETE(oldNode);

        }else if( ret<0 ){
            node->left_ = erase(node->left_, value);

        }else{
            node->right_ = erase(node->right_, value);
        }

        if(NULL != node){
            node = balance(node); //左右部分木バランスどり
        }
        return node;
    }

    //---------------------------------------------------------------
    template<class ValueType, class Predicate>
    void AVLTree<ValueType, Predicate>::eraseNode(node_type* node)
    {
        if(NULL != node){
            eraseNode(node->left_);
            eraseNode(node->right_);
            LIME_DELETE(node);
        }
    }

    //---------------------------------------------------------------
    template<class ValueType, class Predicate>
    const typename AVLTree<ValueType, Predicate>::value_type*
        AVLTree<ValueType, Predicate>::search(const value_type& key) const
    {
        node_type* node = root_;
        while(NULL != node){
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

    template<class ValueType, class Predicate, class EqualFunc>
    const typename AVLTree<ValueType, Predicate>::value_type*
        AVLTree<ValueType, Predicate>::search(const EqualFunc& func)
    {
        node_type* node = root_;
        while(NULL != node){
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
