/**
@file Quadtree.cpp
@author t-sakai
@date 2016/12/19 create
*/
#include "physics/Quadtree.h"

namespace lfw
{
    const s32 Quadtree::LevelIndexStart[MaxLevels] =
    {
        0,
        1,
        5,
        21,
    };

    u32 Quadtree::separateBy1(u32 x)
    {
        x = (x | (x << 8)) & 0x00FF00FFU;
        x = (x | (x << 4)) & 0x0F0F0F0FU;
        x = (x | (x << 2)) & 0x33333333U;
        x = (x | (x << 1)) & 0x55555555U;
        return x;
    }

    u32 Quadtree::mortonCode2(u32 x, u32 y)
    {
        return Quadtree::separateBy1(x) | (Quadtree::separateBy1(y) << 1);
    }

    void Quadtree::calcContains2(s32& level, s32& childIndex, u32 minCode, u32 maxCode)
    {
        u32 x = minCode ^ maxCode;
        level = 0;
        childIndex = 0;
        s32 index = 0;
        for(s32 i=0; i<Quadtree::MaxLevels; ++i){
            u32 n = x & 0x03U;
            if(n != 0){
                level = Quadtree::MaxLevels - i - 1;
                index = i;
            }
            x >>= 2;
        }

        childIndex = (maxCode >> (level*2))&0x03U;
    }

    s32 Quadtree::calcIndex2(s32 level, s32 childIndex)
    {
        LASSERT(0<=level && level<MaxLevels);
        return Quadtree::LevelIndexStart[level] + childIndex;
    }

    s32 Quadtree::getParentIndex(s32 index)
    {
        return (index-1)>>2;
    }

    s32 Quadtree::getChildStartIndex(s32 index)
    {
        return (index<<2) + 1;
    }


    Quadtree::Quadtree()
    {
        bmin_ = lmath::Vector2::zero();
        bmax_ = lmath::Vector2::zero();
        invUnit_ = lmath::Vector2::one();
        numSplits_ = 1;
        for(s32 i=0; i<MaxLevels; ++i){
            numSplits_ <<= 1;
        }
        clear();
    }

    Quadtree::~Quadtree()
    {
    }

    void Quadtree::clear()
    {
        for(s32 i=0; i<NumNodes; ++i){
            nodeTops_[i] = ColliderBase::Invalid;
        }
        colliderBases_.clear();
    }

    void Quadtree::reset()
    {
        for(s32 i=0; i<NumNodes; ++i){
            nodeTops_[i] = ColliderBase::Invalid;
        }
        for(s32 i=0; i<colliderBases_.size(); ++i){
            recalc(reinterpret_cast<ColliderBase2D*>(colliderBases_[i]));
        }
    }

    void Quadtree::setRange(const lmath::Vector2& bmin, const lmath::Vector2& bmax)
    {
        bmin_ = bmin;
        bmax_ = bmax;

        invUnit_ = bmax_;
        invUnit_ -= bmin_;

        for(s32 i=0; i<2; ++i){
            invUnit_[i] = static_cast<f32>(numSplits_)/invUnit_[i];
        }
    }

    u32 Quadtree::calcMortonCode2(const lmath::Vector2& position)
    {
        u32 v[2];
        for(s32 i=0; i<2; ++i){
            v[i] = static_cast<u32>((position[i] - bmin_[i])*invUnit_[i]);
        }
        return Quadtree::mortonCode2(v[0], v[1]);
    }

    void Quadtree::add(ColliderBase2D* node)
    {
        LASSERT(NULL != node);
        LASSERT(colliderBases_.size()<MaxColliders);

        lmath::Vector2 bmin, bmax;
        node->getBoundingBox(bmin, bmax);

        u32 minCode = calcMortonCode2(bmin);
        u32 maxCode = calcMortonCode2(bmax);

        s32 level;
        s32 childIndex;
        calcContains2(level, childIndex, minCode, maxCode);
        s32 index = calcIndex2(level, childIndex);
        node->setID(static_cast<u16>(colliderBases_.size()));
        node->setNext(ColliderBase::Invalid);
        colliderBases_.push_back(node);
        push(node, index);
    }

    void Quadtree::recalc(ColliderBase2D* node)
    {
        lmath::Vector2 bmin, bmax;
        node->getBoundingBox(bmin, bmax);

        u32 minCode = calcMortonCode2(bmin);
        u32 maxCode = calcMortonCode2(bmax);

        s32 level;
        s32 childIndex;
        calcContains2(level, childIndex, minCode, maxCode);
        s32 index = calcIndex2(level, childIndex);
        node->setID(static_cast<u16>(colliderBases_.size()));
        node->setNext(ColliderBase::Invalid);
        push(node, index);
    }

    void Quadtree::collideAll(CollisionPairArray& collisions)
    {
        depth_ = 0;
        innerCollideAll(0, collisions);
    }

    void Quadtree::push(ColliderBase2D* node, s32 index)
    {
        node->setNext(nodeTops_[index]);
        nodeTops_[index] = node->getID();
    }

    void Quadtree::innerCollideAll(s32 nodeIndex, CollisionPairArray& collisions)
    {
        u16 nid0 = nodeTops_[nodeIndex];
        while(ColliderBase::Invalid != nid0){
            u16 nid1 = colliderBases_[nid0]->getNext();
            while(ColliderBase::Invalid != nid1){
                collisions.push_back(CollisionPair::construct(colliderBases_[nid0], colliderBases_[nid1]));
                nid1 = colliderBases_[nid1]->getNext();
            }
            for(s32 i=0; i<depth_; ++i){
                nid1 = nodeTops_[ancestorStack_[i]];
                nid1 = colliderBases_[nid1]->getNext();
                while(ColliderBase::Invalid != nid1){
                    collisions.push_back(CollisionPair::construct(colliderBases_[nid0], colliderBases_[nid1]));
                    nid1 = colliderBases_[nid1]->getNext();
                }
            }
            nid0 = colliderBases_[nid0]->getNext();
        }

        if((MaxLevels-1)<=depth_){
            return;
        }

        ancestorStack_[depth_] = nodeIndex;
        ++depth_;
        s32 nextIndex = getChildStartIndex(nodeIndex);
        for(s32 i=0; i<4; ++i){
            innerCollideAll(nextIndex+i, collisions);
        }
        --depth_;
    }
}
