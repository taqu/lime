#ifndef INC_LFRAMEWORK_COLLIDEMANAGER_H_
#define INC_LFRAMEWORK_COLLIDEMANAGER_H_
/**
@file CollideManager.h
@author t-sakai
@date 2016/12/19 create
*/
#include "lcollide.h"
#include "Octree.h"

namespace lfw
{
    class ColliderBase3D;

    //-------------------------------------------------------
    //---
    //--- CollideManager
    //---
    //-------------------------------------------------------
    class CollideManager
    {
    public:
        struct Statistics
        {
            s32 numCandidates_;
            s32 numCollides_;
        };

        typedef bool(*PrimitiveCollideFunc)(ColliderBase3D* node0, ColliderBase3D* node1, CollisionInfo& info);

        CollideManager();
        ~CollideManager();

        void setCollisionGroup(bool collidable, u16 group0, u16 group1);
        bool isGroupCollidable(u16 group0, u16 group1) const;

        void clear();
        void setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax);
        void add(ColliderBase3D* collider);
        void collideAll();

        inline bool test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group);
    private:
        CollideManager(const CollideManager&);
        CollideManager& operator=(const CollideManager&);

        static PrimitiveCollideFunc collideFuncs_[CollisionType_Num][CollisionType_Num];

        Statistics statistics_;
        u16 collisionGroupFlags_[MaxCollisionGroup];

        Octree::CollisionPairArray collisions_;
        Octree octree_;
    };

    inline bool CollideManager::test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group)
    {
        return octree_.test(hitInfo, ray, group);
    }
}
#endif //INC_LFRAMEWORK_COLLIDEMANAGER_H_
