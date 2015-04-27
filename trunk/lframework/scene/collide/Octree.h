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
        static const s32 MaxLevels = 4;
        static const u32 NumNodes = 585;
        static const s32 MaxTableLevels = 5;
        //static const u32 NumNodes = 4681;
        static const s32 LevelIndexStart[MaxTableLevels];
        static const s32 StackSize = MaxLevels;

        typedef lcore::vector_arena<CollisionPair, lscene::SceneAllocator, lcore::vector_arena_static_inc_size<64> > CollisionPairVector;

        static u32 separateBy2(u32 x);
        static u32 mortonCode3(u32 x, u32 y, u32 z);

        static void calcContains3(s32& level, s32& childIndex, u32 minCode, u32 maxCode);
        static s32 calcIndex3(s32 level, s32 childIndex);
        static s32 getParentIndex(s32 index);
        static s32 getChildStartIndex(s32 index);

        Octree();
        ~Octree();

        void clear();
        void reset();

        void setRange(const lmath::Vector3& bmin, const lmath::Vector3& bmax);
        u32 calcMortonCode3(const lmath::Vector4& position);

        void add(ColliderBase3* node);
        void collideAll(CollisionPairVector& collisions);
    private:
        class NodeTop : public ColliderBase
        {
        };

        void push(ColliderBase3* node, s32 index);
        void innerCollideAll(s32 nodeIndex, CollisionPairVector& collisions);

        u32 numSplits_;
        lmath::Vector4 bmin_;
        lmath::Vector4 bmax_;
        lmath::Vector4 invUnit_;
        NodeTop nodes_[NumNodes];

        s32 depth_;
        s32 ancestorStack_[StackSize];
    };
}
#endif //INC_LCOLLIDE_OCTREE_H__
