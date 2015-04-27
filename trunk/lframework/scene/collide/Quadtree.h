#ifndef INC_LCOLLIDE_QUADTREE_H__
#define INC_LCOLLIDE_QUADTREE_H__
/**
@file Quadtree.h
@author t-sakai
@date 2013/05/14 create
*/
#include "lcollide.h"
#include <lcore/Vector.h>

namespace lcollide
{
    class Quadtree
    {
    public:
        static const s32 MaxLevels = 4;
        static const u32 NumNodes = 85;
        static const s32 LevelIndexStart[MaxLevels];
        static const s32 StackSize = MaxLevels;

        static u32 separateBy1(u32 x);
        static u32 mortonCode2(u32 x, u32 y);

        static void calcContains2(s32& level, s32& childIndex, u32 minCode, u32 maxCode);
        static s32 calcIndex2(s32 level, s32 childIndex);
        static s32 getParentIndex(s32 index);
        static s32 getChildStartIndex(s32 index);

        Quadtree();
        ~Quadtree();

        void clear();
        void reset();

        void setRange(const lmath::Vector2& bmin, const lmath::Vector2& bmax);
        u32 calcMortonCode2(const lmath::Vector2& position);

        void add(ColliderBase2* node);
        void collideAll(lcore::vector_arena<CollisionPair>& collisions);

    private:
        class NodeTop : public ColliderBase
        {
        };

        void push(ColliderBase2* node, s32 index);
        void innerCollideAll(s32 nodeIndex, lcore::vector_arena<CollisionPair>& collisions);

        s32 numSplits_;
        lmath::Vector2 bmin_;
        lmath::Vector2 bmax_;
        lmath::Vector2 invUnit_;
        NodeTop nodes_[NumNodes];

        s32 depth_;
        s32 ancestorStack_[StackSize];
    };
}
#endif //INC_LCOLLIDE_QUADTREE_H__
