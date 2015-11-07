#ifndef INC_LRENDER_QBVH_H__
#define INC_LRENDER_QBVH_H__
/**
@file QBVH.h
@author t-sakai
@date 2013/05/09 create
*/
#include "../lrender.h"
#include <lcore/Vector.h>

namespace lmath
{
    class Ray;
    class Sphere;
}

namespace lrender
{
    class Face;
    struct HitRecord;

    class QBVH
    {
    public:
        static const f32 BoundingExpantion;// = F32_EPSILON*2.0f;

        static const u32 MinLeafFaces = 16;
        static const u32 MaxLeafFaces = 0xFFU;
        static const u32 MaxFaces = (((u32)-1) >> 9) - 1;

        static const u32 TestStackSize = 64;

        struct Node
        {
            static const u32 EmptyMask = (u32)-1;
            static const u32 LeafMask = ~(EmptyMask>>1);

            static bool isLeaf(u32 child)
            {
                return (LeafMask&child) != 0;
            }

            static bool isEmpty(u32 child)
            {
                return EmptyMask == child;
            }

            static u32 getLeaf(u32 faceIndex, u32 numFaces)
            {
                return LeafMask|((faceIndex&0xFFFFFFU)<<8)|(numFaces&0xFFU);
            }

            static u32 getFaceIndex(u32 child)
            {
                return ((~LeafMask)&child) >> 8;
            }

            static u32 getFaceNum(u32 child)
            {
                return (~LeafMask)&child&0xFFU;
            }

            lmath::lm128 bbox_[2][3];
            //f32 bbox_[2][3][4];
            u32 children_[4];
            s32 axis0_;
            s32 axis1_;
            s32 axis2_;
            s32 reserved_;
        };

        bool test(HitRecord& hitRecord, const lmath::Ray& ray) const;
        bool test(HitRecord& hitRecord, const lmath::Vector3& bmin, const lmath::Vector3& bmax);

        bool test(HitRecord& hitRecord, const lmath::Sphere& sphere);

        void construct(u32 numFaces, const Face** faces, const lmath::Vector3& bmin, const lmath::Vector3& bmax);

        void print();
    private:
        bool innerTest(HitRecord& hitRecord, const lmath::Ray& ray, f32 tmin, f32 tmax, s32 raySign[3]) const;
        bool innerTest(HitRecord& hitRecord, lmath::lm128 bbox[2][3]);

        u32 recursiveConstruct(u32 begin, u32 numFaces, const lmath::Vector3& bmin, const lmath::Vector3& bmax);

        lmath::Vector3 bmin_;
        lmath::Vector3 bmax_;
        u32 numFaces_;
        const Face** faces_;

        lcore::vector_arena<Node, Align16Allocator> nodes_;
        //s32 raySign_[3];

        //u32 numTestFaces_;
    };
}
#endif //INC_LRENDER_QBVH_H__
