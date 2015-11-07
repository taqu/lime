#include "SegmentTree.h"
#include <algorithm>
#include <iostream>

namespace lmath
{
    namespace
    {
        f32 maximum(f32 v0, f32 v1)
        {
            return (v0<v1)? v1 : v0;
        }
    }
    SegmentTree::SegmentTree()
        :numUseNode_(0)
        ,root_(Invalid)
    {

    }

    SegmentTree::~SegmentTree()
    {

    }

    void SegmentTree::clear()
    {
        numUseNode_ = 0;
        root_ = Invalid;
        segments_.clear();
    }

    void SegmentTree::add(Segment& segment)
    {
        segments_.push_back(segment);
    }

    void SegmentTree::construct()
    {
        sortSegments(0, segments_.size());
        root_ = searchTree(0, segments_.size());

        if(Invalid == root_){
            return;
        }

        for(u16 i=0; i<segments_.size(); ++i){
            segmentInsertion(root_, i);
        }
    }

    bool SegmentTree::isSmaller(Segment& v0, Segment& v1)
    {
        return v0.sx_<v1.sx_;
    }

    void SegmentTree::sortSegments(s32 start, s32 end)
    {
        SegmentVector::iterator its = segments_.begin() + start;
        SegmentVector::iterator ite = segments_.begin() + end;
        std::sort(its, ite, isSmaller);
    }

    u16 SegmentTree::searchTree(u16 start, u16 end)
    {
        if(end<=start){
            return Invalid;
        }
        u16 node = popNode();
        u16 num = (end-start);
        u16 mid = (num>>1) + start;

        Node& n = nodes_[node];
        n.segments_.clear();
        n.key_ = segments_[mid].sx_;
        //n.start_ = start;
        //n.end_ = end-1;
        n.min_ = segments_[start].sx_;
        f32 maxV = segments_[end-1].ex_;
        for(u16 i=start+1; i<end; ++i){
            maxV = maximum(maxV, segments_[i].ex_);
        }
        n.max_ = maxV;

        if(1<num){
            n.left_ = searchTree(start, mid);
            n.right_ = searchTree(mid, end);
        }else{
            n.left_ = n.right_ = Invalid;
        }
        return node;
    }
    
    void SegmentTree::segmentInsertion(u16 n, u16 seg)
    {
        Node& node = nodes_[n];
        Segment& segment = segments_[seg];
        if(segment.sx_<=node.min_ && node.max_<=segment.ex_){
            node.segments_.push_back(seg);
            return;
        }

        if(Invalid != node.left_){
            Node& nleft = nodes_[node.left_];
            if(nleft.min_<=segment.sx_ && segment.sx_<=nleft.max_
               || nleft.min_<=segment.ex_ && segment.ex_<=nleft.max_)
            {
                segmentInsertion(node.left_, seg);
            }
        }

        if(Invalid != node.right_){
            Node& nright = nodes_[node.right_];
            if(nright.min_<=segment.sx_ && segment.sx_<=nright.max_
               || nright.min_<=segment.ex_ && segment.ex_<=nright.max_)
            {
                segmentInsertion(node.right_, seg);
            }
        }
    }

    u16 SegmentTree::popNode()
    {
        if(nodes_.size()<=numUseNode_){
            nodes_.resize(nodes_.size()+64);
        }
        return numUseNode_++;
    }

    void SegmentTree::debugPrint()
    {
        debugPrint(root_, 0);
    }

    void SegmentTree::debugPrint(u16 node, s32 level)
    {
        if(Invalid == node){
            return;
        }
        Node& n = nodes_[node];
        std::cout << "[" << level << "] key=" << n.key_
            << ", min:" << n.min_ << ", max:" << n.max_
            << std::endl;
        ++level;

        if(0<n.segments_.size()){
            std::cout << "segments:\n";
            for(Node::SegmentVector::iterator itr = n.segments_.begin();
                itr != n.segments_.end();
                ++itr)
            {
                Segment& seg = segments_[*itr];
                std::cout << seg.sx_ << " - " << seg.ex_ << std::endl;
            }
        }
        debugPrint(n.left_, level);
        debugPrint(n.right_, level);
    }
}
