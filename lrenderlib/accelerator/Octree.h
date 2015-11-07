#ifndef INC_LCOLLIDE_OCTREE_H__
#define INC_LCOLLIDE_OCTREE_H__
/**
@file Octree.h
@author t-sakai
@date 2013/05/14 create
*/
#include "lcollide.h"
#include <lcore/Vector.h>

namespace lcollide
{
    class Octree
    {
    public:
        typedef CollideFuncBase CollideFuncType;

        static const s32 MaxLevels = 2;
        static const u32 NumNodes = 585;
        static const s32 LevelIndexStart[MaxLevels];
        static const s32 StackSize = MaxLevels;

        static u32 separateBy2(u32 x);
        static u32 mortonCode3(u32 x, u32 y, u32 z);

        static void calcContains3(s32& level, s32& childIndex, u32 minCode, u32 maxCode);
        static s32 calcIndex3(s32 level, s32 childIndex);
        static s32 getParentIndex(s32 index);
        static s32 getChildStartIndex(s32 index);

        Octree();
        ~Octree();

        void setCollideFunc(CollideFuncType* collideFunc);

        void reset();

        void setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax);
        u32 calcMortonCode3(const lmath::Vector3& position);

        void add(const lmath::Vector3& bmin, const lmath::Vector3& bmax);
        void collideAll(lcore::vector_arena<CollisionPair>& collisions);

        u32 getNumTestPairs() const{ return numTestPairs_;}
    private:
        class NodeTop : public NodeBase
        {
        };

        void innerCollideAll(s32 nodeIndex, lcore::vector_arena<CollisionPair>& collisions);

        CollideFuncType* collideFunc_;

        s32 numSplits_;
        lmath::Vector3 bmin_;
        lmath::Vector3 bmax_;
        lmath::Vector3 invUnit_;
        NodeTop nodes_[NumNodes];

        s32 depth_;
        s32 ancestorStack_[StackSize];
        u32 numTestPairs_;
    };
}
#endif //INC_LCOLLIDE_OCTREE_H__
