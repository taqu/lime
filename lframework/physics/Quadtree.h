#ifndef INC_LFRAMEWORK_QUADTREE_H__
#define INC_LFRAMEWORK_QUADTREE_H__
/**
@file Quadtree.h
@author t-sakai
@date 2016/12/19 create
*/
#include "lcollide.h"
#include <lcore/Array.h>
#include <lmath/Vector2.h>

namespace lfw
{
    class Quadtree
    {
    public:
        static const s32 MaxLevels = 4;
        static const u32 NumNodes = 85;
        static const s32 LevelIndexStart[MaxLevels];
        static const s32 StackSize = MaxLevels;

        typedef lcore::ArrayPOD<CollisionPair, lcore::array_static_inc_size<64>> CollisionPairArray;
        typedef lcore::ArrayPOD<ColliderBase*, lcore::array_static_inc_size<64>> ColliderBaseArray;

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

        void add(ColliderBase2D* node);
        void collideAll(CollisionPairArray& collisions);

    private:
        void recalc(ColliderBase2D* node);
        void push(ColliderBase2D* node, s32 index);
        void innerCollideAll(s32 nodeIndex, CollisionPairArray& collisions);

        s32 numSplits_;
        lmath::Vector2 bmin_;
        lmath::Vector2 bmax_;
        lmath::Vector2 invUnit_;
        u16 nodeTops_[NumNodes];

        s32 depth_;
        s32 ancestorStack_[StackSize];

        ColliderBaseArray colliderBases_;
    };
}
#endif //INC_LFRAMEWORK_QUADTREE_H__
