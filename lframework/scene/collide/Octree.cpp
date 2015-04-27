/**
@file Octree.cpp
@author t-sakai
@date 2013/05/14 create
*/
#include "Octree.h"

namespace lcollide
{
    const s32 Octree::LevelIndexStart[MaxTableLevels] =
    {
        0,
        1,
        9,
        73,
        585,
    };

    u32 Octree::separateBy2(u32 x)
    {
        x = (x | (x << 8)) & 0x0000F00FU;
        x = (x | (x << 4)) & 0x000C30C3U;
        x = (x | (x << 2)) & 0x00249249U;
        return x;
    }

    u32 Octree::mortonCode3(u32 x, u32 y, u32 z)
    {
        return Octree::separateBy2(x) | (Octree::separateBy2(y) << 1) | (Octree::separateBy2(z) << 2);
    }

    void Octree::calcContains3(s32& level, s32& childIndex, u32 minCode, u32 maxCode)
    {
        u32 x = minCode ^ maxCode;
        level = 0;
        childIndex = 0;

        s32 hi = 1;
        for(s32 i=0; i<Octree::MaxLevels; ++i){
            u32 n = x & 0x07U;
            if(0 != n){
                hi = i+1;
            }
            x >>= 3;
        }
        level = Octree::MaxLevels - hi;
        childIndex = maxCode >> (hi*3);
    }

    s32 Octree::calcIndex3(s32 level, s32 childIndex)
    {
        LASSERT(0<=level && level<MaxLevels);
        return Octree::LevelIndexStart[level] + childIndex;
    }

    s32 Octree::getParentIndex(s32 index)
    {
        return (index-1)>>3;
    }

    s32 Octree::getChildStartIndex(s32 index)
    {
        return (index<<3) + 1;
    }


    Octree::Octree()
    {
        bmin_.zero();
        bmax_.zero();
        invUnit_.one();
        numSplits_ = 1;
        for(s32 i=0; i<MaxLevels; ++i){
            numSplits_ <<= 1;
        }

        for(s32 i=0; i<NumNodes; ++i){
            nodes_[i].reset();
        }
    }

    Octree::~Octree()
    {
    }

    void Octree::clear()
    {
        for(s32 i=0; i<NumNodes; ++i){
            nodes_[i].unlink();
        }
    }

    void Octree::reset()
    {
        NodeTop nodes;

        for(s32 i=0; i<NumNodes; ++i){
            for(ColliderBase* n=nodes_[i].getNext(); n != &nodes_[i];){
                ColliderBase* node = n;
                n=n->getNext();
                node->unlink();
                nodes.link(node);
            }
        }

        for(ColliderBase* n=nodes.getNext(); n != &nodes;){
            ColliderBase* node = n;
            n=n->getNext();
            node->unlink();
            add(reinterpret_cast<ColliderBase3*>(node));
        }
    }

    void Octree::setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        bmin_ = bmin;
        bmax_ = bmax;

        invUnit_ = bmax_;
        invUnit_ -= bmin_;

        invUnit_ *= 1.0f/numSplits_;
        for(s32 i=0; i<3; ++i){
            invUnit_[i] = 1.0f/invUnit_[i];
        }
    }

    u32 Octree::calcMortonCode3(const lmath::Vector4& position)
    {
        lmath::Vector4 d;
        d.sub(position, bmin_);
        d *= invUnit_;

        u32 v[3];
        for(s32 i=0; i<3; ++i){
            v[i] = static_cast<u32>(d[i]);
            v[i] = (numSplits_<=v[i])? numSplits_-1 : v[i];
        }
        return Octree::mortonCode3(v[0], v[1], v[2]);
    }

    void Octree::add(ColliderBase3* node)
    {
        LASSERT(NULL != node);
        lmath::Vector4 bmin, bmax;
        node->getBoundingBox(bmin, bmax);

        bmin.maximum(bmin, bmin_);
        u32 minCode = calcMortonCode3(bmin);
        u32 maxCode = calcMortonCode3(bmax);

        s32 level;
        s32 childIndex;
        calcContains3(level, childIndex, minCode, maxCode);
        s32 index = calcIndex3(level, childIndex);
        LASSERT(0<=index && index<NumNodes);
        push(node, index);
    }

    void Octree::collideAll(CollisionPairVector& collisions)
    {
        depth_ = 0;
        innerCollideAll(0, collisions);
    }

    void Octree::push(ColliderBase3* node, s32 index)
    {
        node->link(&nodes_[index]);
    }

    void Octree::innerCollideAll(s32 nodeIndex, CollisionPairVector& collisions)
    {
        ColliderBase* node0 = nodes_[nodeIndex].getNext();
        ColliderBase* end = &nodes_[nodeIndex];

        while(node0 != end){
            ColliderBase* node1 = node0->getNext();
            while(node1 != end){
                collisions.push_back(CollisionPair(node0, node1));
                node1 = node1->getNext();
            }

            for(s32 i=0; i<depth_; ++i){
                ColliderBase* end1 = &nodes_[ancestorStack_[i]];

                node1 = end1->getNext();
                while(node1 != end1){
                    collisions.push_back(CollisionPair(node0, node1));
                    node1 = node1->getNext();
                }
            }

            node0 = node0->getNext();
        }


        if((MaxLevels-1)<=depth_){
            return;
        }

        ancestorStack_[depth_] = nodeIndex;
        ++depth_;
        s32 nextIndex = getChildStartIndex(nodeIndex);
        for(s32 i=0; i<8; ++i){
            innerCollideAll(nextIndex+i, collisions);
        }
        --depth_;
    }
}
