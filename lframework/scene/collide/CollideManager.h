#ifndef INC_LCOLLIDE_COLLIDEMANAGER_H__
#define INC_LCOLLIDE_COLLIDEMANAGER_H__
/**
@file CollideManager.h
@author t-sakai
@date 2015/02/17 create
*/
#include "lcollide.h"
#include "Octree.h"

namespace lcollide
{
    class Collider;

    //-------------------------------------------------------
    //---
    //--- CollideManager
    //---
    //-------------------------------------------------------
    class CollideManager : public lscene::CollideManagerAllocator
    {
    public:
        struct Statistics
        {
            s32 numCandidates_;
            s32 numCollides_;
        };

        typedef bool(*PrimitiveCollideFunc)(Collider* node0, Collider* node1, CollisionInfo& info);

        CollideManager();
        ~CollideManager();

        void setCollisionGroup(bool collidable, u16 group0, u16 group1);
        bool isGroupCollidable(u16 group0, u16 group1) const;

        void clear();
        void setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax);
        void add(Collider* collider);
        void collideAll();

        inline bool test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group);
    private:
        CollideManager(const CollideManager&);
        CollideManager& operator=(const CollideManager&);

        static PrimitiveCollideFunc collideFuncs_[CollisionType_Num][CollisionType_Num];

        Statistics statistics_;
        u16 collisionGroupFlags_[MaxCollisionGroup];

        lcollide::Octree::CollisionPairVector collisions_;
        lcollide::Octree octree_;
    };

    inline bool CollideManager::test(RayHitInfo& hitInfo, const lmath::Ray& ray, s16 group)
    {
        return octree_.test(hitInfo, ray, group);
    }
}
#endif //INC_LCOLLIDE_COLLIDEMANAGER_H__
