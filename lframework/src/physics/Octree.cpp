/**
@file Octree.cpp
@author t-sakai
@date 2016/12/19 create
*/
#include "physics/Octree.h"
#include <lmath/geometry/Ray.h>
#include <lmath/geometry/RayTest.h>

namespace lfw
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
        :depth_(0)
        ,negativeBits_(0)
    {
        bmin_.zero();
        bmax_.zero();
        invUnit_.one();
        numSplits_ = 1;
        for(s32 i=0; i<MaxLevels; ++i){
            numSplits_ <<= 1;
        }

        clear();
    }

    Octree::~Octree()
    {
    }

    inline void Octree::clearTops()
    {
        static const s32 N = (sizeof(u16)*NumNodes)>>4;
        static const s32 Rest = (sizeof(u16)*NumNodes) - (N<<4);
        const u32 invalid = 0xFFFFFFFFU;
        const lmath::lm128 n = _mm_set1_ps(*reinterpret_cast<const f32*>(&invalid));
        lmath::lm128* dst = reinterpret_cast<lmath::lm128*>(nodeTops_);
        for(s32 i=0; i<N; ++i,++dst){
            _mm_storeu_ps(reinterpret_cast<f32*>(dst), n);
        }
        u8* pb = reinterpret_cast<u8*>(dst);
        for(s32 i=0; i<Rest; ++i){
            pb[i] = 0xFFU;
        }
    }

    void Octree::clear()
    {
        clearTops();
        colliderBases_.clear();
    }

    void Octree::reset()
    {
        clearTops();
        for(s32 i=0; i<colliderBases_.size(); ++i){
            recalc(reinterpret_cast<ColliderBase3D*>(colliderBases_[i]));
        }
    }

    void Octree::setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        bmin_ = lmath::Vector4::construct(bmin);
        bmax_ = lmath::Vector4::construct(bmax);

        invUnit_ = bmax_;
        invUnit_ -= bmin_;

        invUnit_ *= 1.0f/numSplits_;
        for(s32 i=0; i<3; ++i){
            invUnit_[i] = 1.0f/invUnit_[i];
        }
    }

    u32 Octree::calcMortonCode3(const lmath::Vector4& position)
    {
        lmath::Vector4 d = lmath::Vector4::construct(position - bmin_);
        d *= invUnit_;

        u32 v[3];
        for(s32 i=0; i<3; ++i){
            v[i] = static_cast<u32>(d[i]);
            v[i] = (numSplits_<=v[i])? numSplits_-1 : v[i];
        }
        return Octree::mortonCode3(v[0], v[1], v[2]);
    }

    void Octree::add(ColliderBase3D* node)
    {
        LASSERT(NULL != node);
        LASSERT(colliderBases_.size()<MaxColliders);

        lmath::Vector4 bmin, bmax;
        node->getBoundingBox(bmin, bmax);

        bmin = lmath::Vector4::construct(maximum(bmin, bmin_));
        u32 minCode = calcMortonCode3(bmin);
        u32 maxCode = calcMortonCode3(bmax);

        s32 level;
        s32 childIndex;
        calcContains3(level, childIndex, minCode, maxCode);
        s32 index = calcIndex3(level, childIndex);
        LASSERT(0<=index && index<NumNodes);
        node->setID(static_cast<u16>(colliderBases_.size()));
        node->setNext(ColliderBase::Invalid);
        colliderBases_.push_back(node);
        push(node, index);
    }

    void Octree::recalc(ColliderBase3D* node)
    {
        lmath::Vector4 bmin, bmax;
        node->getBoundingBox(bmin, bmax);

        bmin = lmath::Vector4::construct(maximum(bmin, bmin_));
        u32 minCode = calcMortonCode3(bmin);
        u32 maxCode = calcMortonCode3(bmax);

        s32 level;
        s32 childIndex;
        calcContains3(level, childIndex, minCode, maxCode);
        s32 index = calcIndex3(level, childIndex);
        LASSERT(0<=index && index<NumNodes);
        node->setNext(ColliderBase::Invalid);
        push(node, index);
    }

    void Octree::collideAll(CollisionPairArray& collisions)
    {
        depth_ = 0;
        innerCollideAll(0, collisions);
    }

    bool Octree::test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group)
    {
        hitInfo.collider_ = NULL;
        hitInfo.t_ = lcore::numeric_limits<f32>::maximum();

        lmath::Vector4 size = lmath::Vector4::construct(bmax_ + bmin_);

        negativeBits_ = 0;
        lmath::Vector4 origin;
        lmath::Vector4 direction;
        origin.w_ = direction.w_ = 0.0f;

        for(s32 i=0; i<3; ++i){
            if(ray.direction_[i]<0.0f){
                origin[i] = size[i] - ray.origin_[i];
                direction[i] = -ray.direction_[i];
                negativeBits_ |= (1<<(2-i));
            }else{
                origin[i] = ray.origin_[i];
                direction[i] = ray.direction_[i];
            }
        }

        invert(direction);

        lmath::Vector4 t0 = lmath::Vector4::construct(bmin_ - origin);
        lmath::Vector4 t1 = lmath::Vector4::construct(bmax_ - origin);
        t0 *= direction;
        t1 *= direction;

        f32 tmin = lcore::maximum(lcore::maximum(t0.x_, t0.y_), t0.z_);
        f32 tmax = lcore::minimum(lcore::minimum(t1.x_, t1.y_), t1.z_);
        if(tmin<tmax){
            targetGroup_ = group;
            innerTest(hitInfo, ray, 0, lmath::Vector3::construct(t0), lmath::Vector3::construct(t1));
            return (NULL != hitInfo.collider_);
        }else{
            return false;
        }
    }


    void Octree::push(ColliderBase3D* node, s32 index)
    {
        node->setNext(nodeTops_[index]);
        nodeTops_[index] = node->getID();
    }

    void Octree::innerCollideAll(s32 nodeIndex, CollisionPairArray& collisions)
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
        for(s32 i=0; i<8; ++i){
            innerCollideAll(nextIndex+i, collisions);
        }
        --depth_;
    }

    void Octree::innerTest(RayHitInfo& hitInfo, const lmath::Ray& ray, s32 nodeIndex, lmath::Vector3 t0, lmath::Vector3 t1)
    {
        if(t1.x_<0.0f || t1.y_<0.0f || t1.z_<0.0f){
            return;
        }
        //lcore::Log("node:%d", nodeIndex);

        u16 nid0 = nodeTops_[nodeIndex];
        while(ColliderBase::Invalid != nid0){
            ColliderBase* collider = colliderBases_[nid0];
            if(collider->getGroup() == targetGroup_){
                f32 t;
                if(collider->test(ray, t) && t<hitInfo.t_){
                    hitInfo.collider_ = collider;
                    hitInfo.t_ = t;
                }
            }
            nid0 = colliderBases_[nid0]->getNext();
        }

        s32 nextIndex = getChildStartIndex(nodeIndex);
        if(NumNodes<=nextIndex){
            return;
        }

        lmath::Vector3 tm = t0+t1;
        tm *= 0.5f;

        s32 node = firstNode(t0, tm);
        lmath::Vector3 tt0, tt1;
        do{
            switch(node)
            {
            case 0:
                innerTest(hitInfo, ray, nextIndex+negativeBits_, t0, tm);
                node = nextNode(tm, 4, 2, 1);
                break;

            case 1:
                tt0.set(t0.x_, t0.y_, tm.z_);
                tt1.set(tm.x_, tm.y_, t1.z_);
                innerTest(hitInfo, ray, nextIndex+(1^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 5, 3, 8);
                break;

            case 2:
                tt0.set(t0.x_, tm.y_, t0.z_);
                tt1.set(tm.x_, t1.y_, tm.z_);
                innerTest(hitInfo, ray, nextIndex+(2^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 6, 8, 3);
                break;

            case 3:
                tt0.set(t0.x_, tm.y_, tm.z_);
                tt1.set(tm.x_, t1.y_, t1.z_);
                innerTest(hitInfo, ray, nextIndex+(3^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 7, 8, 8);
                break;

            case 4:
                tt0.set(tm.x_, t0.y_, t0.z_);
                tt1.set(t1.x_, tm.y_, tm.z_);
                innerTest(hitInfo, ray, nextIndex+(4^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 8, 6, 5);
                break;

            case 5:
                tt0.set(tm.x_, t0.y_, tm.z_);
                tt1.set(t1.x_, tm.y_, t1.z_);
                innerTest(hitInfo, ray, nextIndex+(5^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 8, 7, 8);
                break;

            case 6:
                tt0.set(tm.x_, tm.y_, t0.z_);
                tt1.set(t1.x_, t1.y_, tm.z_);
                innerTest(hitInfo, ray, nextIndex+(6^negativeBits_), tt0, tt1);
                node = nextNode(tt1, 8, 8, 7);
                break;

            case 7:
                tt0.set(tm.x_, tm.y_, tm.z_);
                tt1.set(t1.x_, t1.y_, t1.z_);
                innerTest(hitInfo, ray, nextIndex+(7^negativeBits_), tt0, tt1);
                node = 8;
                break;
            }
        }while(node<8);
    }

    inline void Octree::invert(lmath::Vector4& v)
    {
        for(s32 i=0; i<3; ++i){
            if(lmath::isPositive(v[i])){
                v[i] = (lmath::isZeroPositive(v[i]))? lcore::numeric_limits<f32>::maximum() : 1.0f/v[i];
            }else{
                v[i] = (lmath::isZeroNegative(v[i]))? lcore::numeric_limits<f32>::lowest() : 1.0f/v[i];
            }
        }
    }

    s32 Octree::firstNode(const lmath::Vector3& t0, const lmath::Vector3& tm)
    {
        s32 node = 0;

        if(t0.y_<t0.x_){
            if(t0.z_<t0.x_){
                //Plane YZ
                if(tm.y_<t0.x_){
                    node |= 2;
                }
                if(tm.z_<t0.x_){
                    node |= 1;
                }
                return node;
            }

        }else{
            if(t0.z_ < t0.y_){
                //Plane XZ
                if(tm.x_<t0.y_){
                    node |= 4;
                }
                if(tm.z_<t0.y_){
                    node |= 1;
                }
                return node;
            }
        }

        //Plane XY
        if(tm.x_<t0.z_){
            node |= 4;
        }
        if(tm.y_<t0.z_){
            node |= 2;
        }
        return node;
    }

    s32 Octree::nextNode(const lmath::Vector3& t, s32 x, s32 y, s32 z)
    {
        if(t.x_<t.y_){
            if(t.x_<t.z_){
                return x;
            }
        }else{
            if(t.y_<t.z_){
                return y;
            }
        }
        return z;
    }
}
