#ifndef INC_LRENDER_POINTOCTREE_H__
#define INC_LRENDER_POINTOCTREE_H__
/**
@file PointOctree.h
@author t-sakai
@date 2015/11/11 create
*/
#include "../lrender.h"
#include "AABB.h"
#include "Buffer.h"

namespace lrender
{
    /**
    @brief ポイントクラウドの八分木
    */
    template<class T>
    class PointOctree
    {
    public:
        static const f32 Epsilon;
        static const s32 MaxNumNodePoints = 8;

        typedef T value_type;
        typedef lrender::Buffer<value_type> value_buffer_type;

        //--------------------------------------------
        //--- Node
        //--------------------------------------------
        struct Node
        {
        public:
            inline void clear();

            inline s32 getOffset() const;
            inline void setOffset(s32 offset);

            inline s32 getNum() const;
            inline void setNum(s32 num);

            inline bool isLeaf() const;
            inline void setLeaf(bool leaf);

            u32 num_;
            s32 offset_;
            s32 children_[8];
        };

        class QueryBase
        {
        public:
            virtual ~QueryBase()
            {}

            virtual void procLeafNode(const Node& node, const value_type* values) = 0;
            virtual bool procInternalNode(const Node& node) = 0;

        protected:
            QueryBase()
            {}
        };

        PointOctree();
        PointOctree(value_buffer_type& values);
        ~PointOctree();

        inline const value_buffer_type& getValues() const;

        void swap(PointOctree& rhs);

        inline void traverse(QueryBase& query);

    protected:
        PointOctree(const PointOctree&);
        PointOctree& operator=(const PointOctree&);

        typedef lcore::vector_arena<Node, lcore::DefaultAllocator, lcore::vector_arena_dynamic_inc_size > NodeVector;

        void clear();
        void build();
        void split(s32 nodeIndex);
        void split(
            s32& numLeft,
            s32& numRight,
            s32& offsetRight,
            s32 offset,
            s32 num,
            value_type* values,
            s32 axis,
            const Vector3& pivot);

        void traverse(const Node& node, QueryBase& query);

        AABB bbox_;
        NodeVector nodes_;
        value_buffer_type values_;
    };

    //--------------------------------------------
    //--- PointOctree::Node
    //--------------------------------------------
    template<class T>
    inline void PointOctree<T>::Node::clear()
    {
        for(s32 i = 0; i < 8; ++i){
            children_[i] = 0;
        }
    }

    template<class T>
    inline s32 PointOctree<T>::Node::getOffset() const
    {
        return offset_;
    }

    template<class T>
    inline void PointOctree<T>::Node::setOffset(s32 offset)
    {
        offset_ = offset;
    }

    template<class T>
    inline s32 PointOctree<T>::Node::getNum() const
    {
        return static_cast<s32>(num_ & 0x7FFFFFFFU);
    }

    template<class T>
    inline void PointOctree<T>::Node::setNum(s32 num)
    {
        num_ = (num_ & 0x80000000U) | (num & 0x7FFFFFFFU);
    }

    template<class T>
    inline bool PointOctree<T>::Node::isLeaf() const
    {
        return 0 != (num_ & 0x80000000U);
    }

    template<class T>
    inline void PointOctree<T>::Node::setLeaf(bool leaf)
    {
        if(leaf){
            num_ |= 0x80000000U;
        } else{
            num_ &= 0x7FFFFFFFU;
        }
    }

    //--------------------------------------------------------------
    //--- PointOctree
    //--------------------------------------------------------------
    template<class T>
    const f32 PointOctree<T>::Epsilon = 1.0e-6f;

    template<class T>
    PointOctree<T>::PointOctree()
    {
        clear();
    }

    template<class T>
    PointOctree<T>::PointOctree(value_buffer_type& values)
    {
        clear();
        values_.swap(values);
        build();
    }

    template<class T>
    PointOctree<T>::~PointOctree()
    {
    }

    template<class T>
    inline const typename PointOctree<T>::value_buffer_type& PointOctree<T>::getValues() const
    {
        return values_;
    }

    template<class T>
    void PointOctree<T>::clear()
    {
        nodes_.clear();
        Node root;
        root.num_ = 0;
        root.offset_ = 0;
        nodes_.push_back(root);
    }

    template<class T>
    void PointOctree<T>::swap(PointOctree& rhs)
    {
        lcore::swap(bbox_, rhs.bbox_);
        values_.swap(rhs.values_);
        nodes_.swap(rhs.nodes_);
    }

    template<class T>
    void PointOctree<T>::build()
    {
        if(128 < values_.size()){
            nodes_.reserve(values_.size() >> 1);
            nodes_.setIncSize(values_.size() >> 2);
        } else{
            nodes_.setIncSize(128);
        }

        bbox_.setInvalid();
        for(s32 i=0; i<values_.size(); ++i){
            bbox_.extend(values_[i].getPosition());
        }
        Node& root = nodes_[0];
        root.offset_ = 0;
        root.setNum(values_.size());
        split(0);
    }

    template<class T>
    void PointOctree<T>::split(s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        node.clear();

        s32 num = node.getNum();
        s32 offset = node.offset_;

        s32 end = offset + num;
        AABB bbox;
        bbox.setInvalid();
        for(s32 i=offset; i<end; ++i){
            bbox.extend(values_[i].getPosition());
        }

        Vector3 extent = bbox.extent();
        bool degenerate = (extent.x_ < Epsilon && extent.y_ < Epsilon && extent.z_ < Epsilon);
        bbox.expand(Epsilon);

        if(num <= MaxNumNodePoints || degenerate){
            node.setLeaf(true);
            return;
        }
        node.setLeaf(false);

        s32 numPoints[8];
        s32 offsets[8];
        Vector3 pivot = bbox.center();
        value_type* values = &values_[0];

        offsets[0] = offset;
        split(numPoints[0], numPoints[4], offsets[4], offsets[0], num, values, 0, pivot);
        split(numPoints[0], numPoints[2], offsets[2], offsets[0], numPoints[0], values, 1, pivot);
        split(numPoints[0], numPoints[1], offsets[1], offsets[0], numPoints[0], values, 2, pivot);
        split(numPoints[2], numPoints[3], offsets[3], offsets[2], numPoints[2], values, 2, pivot);

        split(numPoints[4], numPoints[6], offsets[6], offsets[4], numPoints[4], values, 1, pivot);
        split(numPoints[4], numPoints[5], offsets[5], offsets[4], numPoints[4], values, 2, pivot);
        split(numPoints[6], numPoints[7], offsets[7], offsets[6], numPoints[6], values, 2, pivot);

        for(s32 i = 0; i < 8; ++i){
            if(numPoints[i] <= 0){
                continue;
            }
            Node child;
            child.setOffset(offsets[i]);
            child.setNum(numPoints[i]);

            nodes_[nodeIndex].children_[i] = nodes_.size();
            nodes_.push_back(child);
        }
        for(s32 i=0; i<8; ++i){
            if(numPoints[i]<=0){
                continue;
            }
            split(nodes_[nodeIndex].children_[i]);
        }
    }

    template<class T>
    void PointOctree<T>::split(
        s32& numLeft,
        s32& numRight,
        s32& offsetRight,
        s32 offset,
        s32 num,
        value_type* values,
        s32 axis,
        const Vector3& pivot)
    {
        values += offset;
        s32 i0 = 0;
        s32 i1 = num - 1;

        for(;;){
            while(i0 < num && values[i0].getPosition()[axis] < pivot[axis]){
                ++i0;
            }

            while(0 <= i1 && pivot[axis] < values[i1].getPosition()[axis]){
                --i1;
            }

            if(i1 <= i0){
                break;
            }
            lcore::swap(values[i0], values[i1]);
            ++i0;
            --i1;
        }

        numLeft = lcore::minimum(i0, num);
        numRight = num - numLeft;
        offsetRight = offset + numLeft;

#if 1
        for(s32 i = 0; i < numLeft; ++i){
            if(pivot[axis] < values[i].getPosition()[axis]){
                lcore::Log("error");
            }
        }
        for(s32 i = numLeft; i < num; ++i){
            if(values[i].getPosition()[axis] < pivot[axis]){
                lcore::Log("error");
            }
        }
#endif
    }

    template<class T>
    inline  void PointOctree<T>::traverse(QueryBase& query)
    {
        traverse(nodes_[0], query);
    }

    template<class T>
    void PointOctree<T>::traverse(const Node& node, QueryBase& query)
    {
        if(node.isLeaf()){
            query.procLeafNode(node, &values_[0]);

        } else{
            if(query.procInternalNode(node)){
                for(s32 i = 0; i < 8; ++i){
                    if(node.children_[i]){
                        const Node& child = nodes_[node.children_[i]];
                        traverse(child, query);
                    }
                }
            } //if(query.procInternalNode(node))
        } //if(node.isLeaf())
    }
}
#endif //INC_LRENDER_POINTOCTREE_H__
