/**
@file Octree.cpp
@author t-sakai
@date 2013/05/14 create
*/
#include "Octree.h"
#include <lmath/geometry/Ray.h>
#include <lmath/geometry/RayTest.h>

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

    bool Octree::test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group)
    {
        hitInfo.collider_ = NULL;
        hitInfo.t_ = lcore::numeric_limits<f32>::maximum();

        lmath::Vector4 size;
        size.add(bmax_, bmin_);

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

        lmath::Vector4 t0, t1;
        t0.sub(bmin_, origin);
        t0 *= direction;

        t1.sub(bmax_, origin);
        t1 *= direction;

        f32 tmin = lcore::maximum(lcore::maximum(t0.x_, t0.y_), t0.z_);
        f32 tmax = lcore::minimum(lcore::minimum(t1.x_, t1.y_), t1.z_);
        if(tmin<tmax){
            targetGroup_ = group;
            innerTest(hitInfo, ray, 0, lmath::Vector3(t0), lmath::Vector3(t1));
            return (NULL != hitInfo.collider_);
        }else{
            return false;
        }
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

    void Octree::innerTest(RayHitInfo& hitInfo, const lmath::Ray& ray, s32 nodeIndex, lmath::Vector3 t0, lmath::Vector3 t1)
    {
        if(t1.x_<0.0f || t1.y_<0.0f || t1.z_<0.0f){
            return;
        }
        //lcore::Log("node:%d", nodeIndex);

        ColliderBase* collider = nodes_[nodeIndex].getNext();
        ColliderBase* end = &nodes_[nodeIndex];

        while(collider != end){
            if(collider->getGroup() == targetGroup_){
                f32 t;
                if(collider->test(ray, t) && t<hitInfo.t_){
                    hitInfo.collider_ = collider;
                    hitInfo.t_ = t;
                }
            }
            collider = collider->getNext();
        }

        s32 nextIndex = getChildStartIndex(nodeIndex);
        if(NumNodes<=nextIndex){
            return;
        }

        lmath::Vector3 tm;
        tm.add(t0, t1);
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
                v[i] = (lmath::isZeroNegative(v[i]))? -lcore::numeric_limits<f32>::maximum() : 1.0f/v[i];
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
