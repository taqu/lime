#ifndef INC_LRENDER_DYNAMICPOINTOCTREE_H__
#define INC_LRENDER_DYNAMICPOINTOCTREE_H__
/**
@file DynamicPointOctree.h
@author t-sakai
@date 2015/12/12 create
*/
#include "../lrender.h"
#include <lcore/Vector.h>
#include "AABB.h"

namespace lrender
{
    /**
    @brief ポイントクラウドの八分木
    */
    template<class T>
    class DynamicPointOctree
    {
    public:
        static const f32 Epsilon;
        static const s32 MaxDepth = 16;

        typedef T value_type;
        typedef lcore::vector_arena<value_type> value_vector_type;

        //--------------------------------------------
        //--- Node
        //--------------------------------------------
        struct Node
        {
        public:
            Node();
            ~Node();

            void swap(Node& rhs);

            Node* children_[8];
            value_vector_type values_;
        };

        class QueryBase
        {
        public:
            virtual ~QueryBase()
            {}

            virtual bool operator()(const value_type& value) = 0;

        protected:
            QueryBase()
            {}
        };

        DynamicPointOctree();
        DynamicPointOctree(const AABB& bbox, s32 maxDepth=MaxDepth);
        ~DynamicPointOctree();

        void swap(DynamicPointOctree& rhs);

        bool add(const value_type& value);

        inline void traverse(const Vector3& position, QueryBase& query);

    protected:
        DynamicPointOctree(const DynamicPointOctree&);
        DynamicPointOctree& operator=(const DynamicPointOctree&);

        bool add(const value_type& value, const AABB& bbox, Node& node, s32 depth);
        void traverse(const Vector3& position, QueryBase& query, const Node& node, const AABB& bbox);

        AABB bbox_;
        s32 maxDepth_;
        Node root_;
    };

    //--------------------------------------------
    //--- DynamicPointOctree::Node
    //--------------------------------------------
    template<class T>
    DynamicPointOctree<T>::Node::Node()
    {
        for(s32 i = 0; i < 8; ++i){
            children_[i] = NULL;
        }
    }

    template<class T>
    DynamicPointOctree<T>::Node::~Node()
    {
        for(s32 i = 0; i < 8; ++i){
            LIME_DELETE(children_[i]);
        }
    }

    template<class T>
    void DynamicPointOctree<T>::Node::swap(Node& rhs)
    {
        for(s32 i = 0; i < 8; ++i){
            lcore::swap(children_[i], rhs.children_[i]);
        }
        values_.swap(rhs.values_);
    }

    //--------------------------------------------------------------
    //--- DynamicPointOctree
    //--------------------------------------------------------------
    template<class T>
    const f32 DynamicPointOctree<T>::Epsilon = 1.0e-6f;

    template<class T>
    DynamicPointOctree<T>::DynamicPointOctree()
    {
        bbox_.setInvalid();
    }

    template<class T>
    DynamicPointOctree<T>::DynamicPointOctree(const AABB& bbox, s32 maxDepth)
        :bbox_(bbox)
        ,maxDepth_(maxDepth)
    {
    }

    template<class T>
    DynamicPointOctree<T>::~DynamicPointOctree()
    {
    }

    template<class T>
    void DynamicPointOctree<T>::swap(DynamicPointOctree& rhs)
    {
        lcore::swap(bbox_, rhs.bbox_);
        lcore::swap(maxDepth_, rhs.maxDepth_);
        root_.swap(rhs.root_);
    }

    template<class T>
    bool DynamicPointOctree<T>::add(const value_type& value)
    {
        LASSERT(bbox_.contains(value.getPosition()));
        return add(value, bbox_, root_, 0);
    }

    template<class T>
    bool DynamicPointOctree<T>::add(const value_type& value, const AABB& bbox, Node& node, s32 depth)
    {
        Vector3 extent = bbox.extent();
        bool degenerate = (extent.x_ < Epsilon && extent.y_ < Epsilon && extent.z_ < Epsilon);

        if(degenerate || maxDepth_<=depth){
            node.values_.push_back(value);
            return true;
        }

        const Vector3& position = value.getPosition();
        Vector3 pivot = bbox.center();
        AABB childBBox;
        s32 childIndex = octreeChildIndex(childBBox, position, pivot, bbox);

        if(NULL == node.children_[childIndex]){
            node.children_[childIndex] = LIME_NEW Node();
        }
        return add(value, childBBox, *node.children_[childIndex], depth+1);
    }

    template<class T>
    inline  void DynamicPointOctree<T>::traverse(const Vector3& position, QueryBase& query)
    {
        if(!bbox_.contains(position)){
            return;
        }
        traverse(position, query, root_, bbox_);
    }

    template<class T>
    void DynamicPointOctree<T>::traverse(const Vector3& position, QueryBase& query, const Node& node, const AABB& bbox)
    {
        for(s32 i=0; i<node.values_.size(); ++i){
            if(!query(node.values_[i])){
                return;
            }
        }

        Vector3 pivot = bbox.center();
        AABB childBBox;
        s32 childIndex = octreeChildIndex(childBBox, position, pivot, bbox);
        if(NULL == node.children_[childIndex]){
            return;
        }
        traverse(position, query, *node.children_[childIndex], childBBox);
    }
}
#endif //INC_LRENDER_DYNAMICPOINTOCTREE_H__
