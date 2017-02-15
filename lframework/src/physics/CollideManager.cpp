/**
@file CollideManager.cpp
@author t-sakai
@date 2016/12/19 create
*/
#include "physics/CollideManager.h"
#include "physics/Collider.h"
#include <lmath/geometry/PrimitiveTest.h>
#include <lmath/geometry/RayTest.h>

namespace lfw
{
    bool activeCollisionFunc(ColliderBase3D*, ColliderBase3D*, CollisionInfo&)
    {
        return true;
    }

    bool deactiveCollisionFunc(ColliderBase3D*, ColliderBase3D*, CollisionInfo&)
    {
        return false;
    }

    bool sphereVSSphere(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderSphere* s1 = reinterpret_cast<ColliderSphere*>(node1);

        f32 distance;
        if(!lmath::testSphereSphere(distance, s0->bsphere_, s1->bsphere_)){
            return false;
        }

        f32 radius = s0->bsphere_.r_ + s1->bsphere_.r_;

        f32 depth = 0.5f * (radius - distance);

        info.type_ = CollisionInfoType_NormalDepth;
        info.info_ = lmath::Vector4::construct(s1->bsphere_ - s0->bsphere_);
        info.info_.w_ = 0.0f;
        info.info_ = lmath::Vector4::construct(normalizeChecked(info.info_));
        info.info_.w_ = depth;

        return true;
    }

    bool sphereVSRay(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
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

            lmath::lm128 p = lmath::Vector4::construct(ray.origin_);
            lmath::lm128 d = lmath::Vector4::construct(ray.direction_);
            p = p + t*d;

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = lmath::Vector4::construct(p);
            return true;
        } else{
            return false;
        }
    }

    bool rayVSSphere(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
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

            lmath::lm128 p = lmath::Vector4::construct(ray.origin_);
            lmath::lm128 d = lmath::Vector4::construct(ray.direction_);
             p = p + t*d;

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = lmath::Vector4::construct(p);
            return true;
        } else{
            return false;
        }
    }

    bool sphereVSAABB(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderAABB* s1 = reinterpret_cast<ColliderAABB*>(node1);

        const lmath::Sphere& sphere = s0->bsphere_;
        const lmath::Vector4& bmin = s1->bmin_;
        const lmath::Vector4& bmax = s1->bmax_;

        lmath::Vector4 closePoint;
        if(lmath::testSphereAABB(closePoint, sphere, bmin, bmax)){

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = closePoint;
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSSphere(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderSphere* s1 = reinterpret_cast<ColliderSphere*>(node1);

        const lmath::Vector4& bmin = s0->bmin_;
        const lmath::Vector4& bmax = s0->bmax_;
        const lmath::Sphere& sphere = s1->bsphere_;

        lmath::Vector4 closePoint;
        if(lmath::testSphereAABB(closePoint, sphere, bmin, bmax)){
            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = closePoint;
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSAABB(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderAABB* s1 = reinterpret_cast<ColliderAABB*>(node1);

        const lmath::Vector4& bmin0 = s0->bmin_;
        const lmath::Vector4& bmax0 = s0->bmax_;
        const lmath::Vector4& bmin1 = s1->bmin_;
        const lmath::Vector4& bmax1 = s1->bmax_;

        if(lmath::testAABBAABB(bmin0, bmax0, bmin1, bmax1)){

            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_.zero();
            return true;
        } else{
            return false;
        }
    }

    bool AABBVSRay(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderAABB* s0 = reinterpret_cast<ColliderAABB*>(node0);
        ColliderRay* s1 = reinterpret_cast<ColliderRay*>(node1);

        const lmath::Vector4& bmin = s0->bmin_;
        const lmath::Vector4& bmax = s0->bmax_;
        const lmath::Ray& ray = s1->ray_;

        f32 tmin, tmax;
        if(lmath::testRayAABB(tmin, tmax, ray, bmin, bmax)){
            info.type_ = CollisionInfoType_ClosestPoint;

            lmath::lm128 p = lmath::Vector4::construct(ray.origin_);
            lmath::lm128 d = lmath::Vector4::construct(ray.direction_);
            p = p + tmin*d;
            info.info_ = lmath::Vector4::construct(p);
            return true;
        } else{
            return false;
        }
    }

    bool rayVSAABB(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        return AABBVSRay(node1, node0, info);
    }


    bool sphereVSCapsule(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderSphere* s0 = reinterpret_cast<ColliderSphere*>(node0);
        ColliderCapsule* s1 = reinterpret_cast<ColliderCapsule*>(node1);

        lmath::Vector3 center = s0->bsphere_.position();
        lmath::Vector3 p;
        lmath::closestPointPointSegment(p, center, s1->p0_, s1->p1_);
        f32 d = distanceSqr(p, center);

        f32 radius = s0->bsphere_.radius() + s1->radius_;
        if(d<=(radius*radius)){
            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = lmath::Vector4::construct((p+center)*0.5f);
            return true;
        }else{
            return false;
        }
    }

    bool rayVSCapsule(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderRay* s0 = reinterpret_cast<ColliderRay*>(node0);
        ColliderCapsule* s1 = reinterpret_cast<ColliderCapsule*>(node1);

        f32 s,t;
        lmath::Vector3 c0,c1;
        lmath::Vector3 q0 = s0->ray_.origin_ + s0->ray_.t_*s0->ray_.direction_;
        f32 distanceSqr = lmath::closestPointSegmentSegmentSqr(
            s,t,
            c0, c1,
            s0->ray_.origin_, q0,
            s1->p0_, s1->p1_);
        f32 radius = s1->radius_;
        if(distanceSqr<=(radius*radius)){
            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = lmath::Vector4::construct(c0);
            return true;
        }else{
            return false;
        }
    }

    bool capsuleVSSphere(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        return sphereVSCapsule(node1, node0, info);
    }

    bool capsuleVSRay(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        return rayVSCapsule(node1, node0, info);
    }

    bool capsuleVSCapsule(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info)
    {
        ColliderCapsule* s0 = reinterpret_cast<ColliderCapsule*>(node0);
        ColliderCapsule* s1 = reinterpret_cast<ColliderCapsule*>(node1);
        f32 s,t;
        lmath::Vector3 c0,c1;
        f32 distanceSqr = lmath::closestPointSegmentSegmentSqr(
            s,t,
            c0, c1,
            s0->p0_, s0->p1_,
            s1->p0_, s1->p1_);
        f32 radius = s0->radius_ + s1->radius_;
        if(distanceSqr <= (radius*radius)){
            info.type_ = CollisionInfoType_ClosestPoint;
            info.info_ = lmath::Vector4::construct((c0+c1)*0.5f);
            return true;
        }else{
            return false;
        }
    }

    CollideManager::PrimitiveCollideFunc CollideManager::collideFuncs_[CollisionType_Num][CollisionType_Num] =
    {
        sphereVSSphere, sphereVSRay, sphereVSAABB, sphereVSCapsule,
        rayVSSphere, deactiveCollisionFunc, rayVSAABB, rayVSCapsule,
        AABBVSSphere, AABBVSRay, AABBVSAABB, deactiveCollisionFunc,
        capsuleVSSphere, capsuleVSRay, deactiveCollisionFunc, capsuleVSCapsule,
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

    void CollideManager::add(ColliderBase3D* collider)
    {
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

            ColliderBase3D* node0 = reinterpret_cast<ColliderBase3D*>(collisions_[i].node0_);
            ColliderBase3D* node1 = reinterpret_cast<ColliderBase3D*>(collisions_[i].node1_);

            if(isGroupCollidable(node0->getGroup(), node1->getGroup())){

                PrimitiveCollideFunc func = collideFuncs_[node0->getType()][node1->getType()];
                if(func(node0, node1, info)){
                    LASSERT(NULL != node0->getData());
                    LASSERT(NULL != node1->getData());
                    ++statistics_.numCollides_;
                    node0->invoke(node1, info);
                    if(info.type_ == CollisionInfoType_NormalDepth){
                        info.info_ = -info.info_;
                        info.info_.w_ = -info.info_.w_;
                    }
                    node1->invoke(node0, info);
                }
            }
        }

        octree_.clear();
    }
}
