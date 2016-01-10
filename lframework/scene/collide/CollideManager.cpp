/**
@file CollideManager.cpp
@author t-sakai
@date 2015/02/17 create
*/
#include "CollideManager.h"
#include "Collider.h"
#include <lmath/geometry/PrimitiveTest.h>
#include <lmath/geometry/RayTest.h>
#include "../NodeBase.h"

namespace lcollide
{
    bool activeCollisionFunc(Collider*, Collider*, CollisionInfo&)
    {
        return true;
    }

    bool deactiveCollisionFunc(Collider*, Collider*, CollisionInfo&)
    {
        return false;
    }

    bool sphereVSSphere(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderSphere* s1 = reinterpret_cast<ColliderSphere*>(node1);

        f32 distance;
        if(!lmath::testSphereVsSphere(distance, s0->bsphere_, s1->bsphere_)){
            return false;
        }

        f32 radius = s0->bsphere_.s_.w_ + s1->bsphere_.s_.w_;

        f32 depth = 0.5f * (radius - distance);

        info.type_ = CollisionInfoType_NormalDepth;
        info.info_.sub(s1->bsphere_.s_, s0->bsphere_.s_);
        info.info_.w_ = 0.0f;
        info.info_.normalizeChecked();
        info.info_.w_ = depth;

        return true;
    }

    bool sphereVSRay(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderRay* s1 = reinterpret_cast<ColliderRay*>(node1);

        const lmath::Sphere& sphere = s0->bsphere_;
        const lmath::Ray& ray = s1->ray_;

        f32 t;
        if(lmath::testRaySphere(t, ray, sphere)){
            f32 T = lcore::absolute(ray.t_);
            if(T < t){
                return false;
            }

            lmath::Vector4 p(ray.origin_);
            lmath::Vector4 d(ray.direction_);
            p.muladd(t, d, p);

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_.muladd(t, d, p);
            return true;
        } else{
            return false;
        }
    }

    bool rayVSSphere(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderRay* s0 = reinterpret_cast<ColliderRay*>(node0);
        ColliderSphere* s1 = reinterpret_cast<ColliderSphere*>(node1);

        const lmath::Ray& ray = s0->ray_;
        const lmath::Sphere& sphere = s1->bsphere_;

        f32 t;
        if(lmath::testRaySphere(t, ray, sphere)){
            f32 T = lcore::absolute(ray.t_);
            if(T < t){
                return false;
            }

            lmath::Vector4 p(ray.origin_);
            lmath::Vector4 d(ray.direction_);

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_.muladd(t, d, p);
            return true;
        } else{
            return false;
        }
    }

    bool sphereVSAABB(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderAABB* s1 = reinterpret_cast<ColliderAABB*>(node1);

        const lmath::Sphere& sphere = s0->bsphere_;
        const lmath::Vector4& bmin = s1->bmin_;
        const lmath::Vector4& bmax = s1->bmax_;

        lmath::Vector4 closePoint;
        if(lmath::testSphereVsAABB(closePoint, sphere, bmin, bmax)){

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = closePoint;
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSSphere(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderSphere* s1 = reinterpret_cast<ColliderSphere*>(node1);

        const lmath::Vector4& bmin = s0->bmin_;
        const lmath::Vector4& bmax = s0->bmax_;
        const lmath::Sphere& sphere = s1->bsphere_;

        lmath::Vector4 closePoint;
        if(lmath::testSphereVsAABB(closePoint, sphere, bmin, bmax)){
            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = closePoint;
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSAABB(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderAABB* s1 = reinterpret_cast<ColliderAABB*>(node1);

        const lmath::Vector4& bmin0 = s0->bmin_;
        const lmath::Vector4& bmax0 = s0->bmax_;
        const lmath::Vector4& bmin1 = s1->bmin_;
        const lmath::Vector4& bmax1 = s1->bmax_;

        if(lmath::testAABBVsAABB(bmin0, bmax0, bmin1, bmax1)){

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_.zero();
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSRay(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderRay* s1 = reinterpret_cast<ColliderRay*>(node1);

        const lmath::Vector4& bmin = s0->bmin_;
        const lmath::Vector4& bmax = s0->bmax_;
        const lmath::Ray& ray = s1->ray_;

        f32 tmin, tmax;
        if(lmath::testRayAABB(tmin, tmax, ray, bmin, bmax)){
            info.type_ = CollisionInfoType_ClosestPoint;

            lmath::Vector4 p(ray.origin_);
            lmath::Vector4 d(ray.direction_);
            info.info_.muladd(tmin, p, d);
            return true;
        } else{
            return false;
        }
    }

    bool rayVSAABB(Collider* node0, Collider* node1, CollisionInfo& info)
    {
        return AABBVSRay(node1, node0, info);
    }

    CollideManager::PrimitiveCollideFunc CollideManager::collideFuncs_[CollisionType_Num][CollisionType_Num] =
    {
        sphereVSSphere, sphereVSRay, sphereVSAABB,
        rayVSSphere, deactiveCollisionFunc, rayVSAABB,
        AABBVSSphere, AABBVSRay, AABBVSAABB,
    };

    CollideManager::CollideManager()
    {
        statistics_.numCandidates_ = 0;
        statistics_.numCollides_ = 0;

        for(s32 i=0; i<MaxCollisionGroup; ++i){
            collisionGroupFlags_[i] = 0;
        }
    }

    CollideManager::~CollideManager()
    {
        collisions_.clear();
        octree_.clear();
    }

    void CollideManager::setCollisionGroup(bool collidable, u16 group0, u16 group1)
    {
        LASSERT(group0<MaxCollisionGroup);
        LASSERT(group1<MaxCollisionGroup);

        if(collidable){
            collisionGroupFlags_[group0] |= (1U<<group1);
            collisionGroupFlags_[group1] |= (1U<<group0);
        } else{
            collisionGroupFlags_[group0] &= ~(1U<<group1);
            collisionGroupFlags_[group1] &= ~(1U<<group0);
        }
    }

    bool CollideManager::isGroupCollidable(u16 group0, u16 group1) const
    {
        return (0 != (collisionGroupFlags_[group0] & (1U<<group1)));
    }

    void CollideManager::clear()
    {
        octree_.clear();
    }

    void CollideManager::setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax)
    {
        octree_.setRange(bmin, bmax);
        octree_.reset();
    }

    void CollideManager::add(Collider* collider)
    {
        collider->unlink();
        octree_.add(collider);
    }

    void CollideManager::collideAll()
    {
        collisions_.clear();
        octree_.collideAll(collisions_);

        statistics_.numCandidates_ = collisions_.size();
        statistics_.numCollides_ = 0;

        CollisionInfo info;
        for(s32 i=0; i<collisions_.size(); ++i){

            Collider* node0 = reinterpret_cast<Collider*>(collisions_[i].node0_);
            Collider* node1 = reinterpret_cast<Collider*>(collisions_[i].node1_);

            if(isGroupCollidable(node0->getGroup(), node1->getGroup())){

                PrimitiveCollideFunc func = collideFuncs_[node0->getType()][node1->getType()];
                if(func(node0, node1, info)){
                    LASSERT(NULL != node0->getData());
                    LASSERT(NULL != node1->getData());
                    ++statistics_.numCollides_;
                    node0->getCollidable()->onCollide(node1->getCollidable(), info);
                    if(info.type_ == CollisionInfoType_NormalDepth){
                        info.info_ = -info.info_;
                        info.info_.w_ = -info.info_.w_;
                    }
                    node1->getCollidable()->onCollide(node0->getCollidable(), info);
                }
            }
        }

        octree_.clear();
    }
}
