/**
@file Octree.cpp
@author t-sakai
@date 2013/05/14 create
*/
#include "Octree.h"
#include <stdio.h>

namespace lcollide
{
    const s32 Octree::LevelIndexStart[MaxLevels] =
    {
        0,
        1,
        //9,
        //73,
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
        s32 index = 0;

        s32 hi = 1;
        for(s32 i=0; i<Octree::MaxLevels; ++i){
            u32 n = x&0x07U;
            if(0 != n){
                hi = i + 1;
            }
            x >>= 3;
        }

        level = Octree::MaxLevels - hi;
        //childIndex = (0 == level)? 0 : (maxCode >> (hi*3)) & 0x07U;
        childIndex = (maxCode >> (hi*3)) & 0x07U;
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
        :numTestPairs_(0)
        ,collideFunc_(NULL)
    {
        bmin_.zero();
        bmax_.zero();
        invUnit_.one();
        numSplits_ = 1;
        for(s32 i=0; i<MaxLevels; ++i){
            numSplits_ <<= 1;
        }

        reset();
    }

    Octree::~Octree()
    {
    }

    void Octree::setCollideFunc(CollideFuncType* collideFunc)
    {
        collideFunc_ = collideFunc;
    }

    void Octree::reset()
    {
        for(s32 i=0; i<NumNodes; ++i){
            nodes_[i].reset();
        }
    }

    void Octree::setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        bmin_ = bmin;
        bmax_ = bmax;

        invUnit_ = bmax_;
        invUnit_ -= bmin_;

        invUnit_ /= numSplits_;
        for(s32 i=0; i<3; ++i){
            invUnit_[i] = 1.0f/invUnit_[i];
        }
    }

    u32 Octree::calcMortonCode3(const lmath::Vector3& position)
    {
        u32 v[3];
        for(s32 i=0; i<3; ++i){
            v[i] = static_cast<u32>((position[i] - bmin_[i])*invUnit_[i]);
        }
        return Octree::mortonCode3(v[0], v[1], v[2]);
    }

    void Octree::add(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        u32 minCode = calcMortonCode3(bmin);
        u32 maxCode = calcMortonCode3(bmax);

        s32 level;
        s32 childIndex;
        calcContains3(level, childIndex, minCode, maxCode);
        s32 index = calcIndex3(level, childIndex);
        printf("min:%d, max:%d, level:%d, index:%d\n", minCode, maxCode, level, childIndex);
    }

    void Octree::collideAll(lcore::vector_arena<CollisionPair>& collisions)
    {
        LASSERT(NULL != collideFunc_);
        depth_ = 0;
        numTestPairs_ = 0;
        innerCollideAll(0, collisions);
    }

    void Octree::innerCollideAll(s32 nodeIndex, lcore::vector_arena<CollisionPair>& collisions)
    {
        NodeBase* node0 = nodes_[nodeIndex].getNext();
        NodeBase* end = &nodes_[nodeIndex];

        while(node0 != end){
            NodeBase* node1 = node0->getNext();
            lmath::Vector4 normalDepth;
            while(node1 != end){
                ++numTestPairs_;
                //if((*collideFunc_)(node0, node1, normalDepth)){
                //    collisions.push_back(CollisionPair(node0, node1, normalDepth));
                //}
                node1 = node1->getNext();
            }

            for(s32 i=0; i<depth_; ++i){
                NodeBase* end1 = &nodes_[ ancestorStack_[i] ];

                node1 = end1->getNext();
                while(node1 != end1){
                    ++numTestPairs_;
                    //if((*collideFunc_)(node0, node1, normalDepth)){
                    //    collisions.push_back(CollisionPair(node0, node1, normalDepth));
                    //}
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

        for(s32 i=0; i<8; ++i){
            s32 nextIndex = getChildStartIndex(nodeIndex) + i;
            innerCollideAll(nextIndex, collisions);
        }
        --depth_;
    }
}
