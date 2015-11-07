#ifndef INC_LRENDER_BINQBVH_H__
#define INC_LRENDER_BINQBVH_H__
/**
@file BinQBVH.h
@author t-sakai
@date 2015/09/09 create
*/
#include "Accelerator.h"
#include "../shape/Shape.h"

namespace lrender
{
#define LRENDER_BINQBVH_SSEPACK (1)

    class Intersection;

    class BinQBVH
    {
    public:
        static const f32 Epsilon;
        static const s32 MinLeafPrimitives = 15;
        static const s32 NumBins = 32;
        static const s32 MaxBinningDepth = 16;

        struct Node
        {
            static const u32 LeafMask = ~(((u32)-1)>>1);
            static const u32 EmptyMask = LeafMask;

            static const s32 MaxNumLeafPrimitiveShift = 4;
            static const s32 LeafPrimitiveMask = (0x01U<<MaxNumLeafPrimitiveShift)-1;
            static const u32 MaxNumPrimitives = (~(LeafMask|EmptyMask))>>MaxNumLeafPrimitiveShift;


            inline static bool isLeaf(u32 index)
            {
                return (0 != (index&LeafMask));
            }

            inline static bool isEmpty(u32 index)
            {
                return (0 == (index&LeafPrimitiveMask));
            }

            inline static u32 getPrimitiveIndex(u32 index)
            {
                return (index >> MaxNumLeafPrimitiveShift) & MaxNumPrimitives;
            }

            inline static u32 getNumPrimitives(u32 index)
            {
                return (index&LeafPrimitiveMask);
            }

            bool isJoint(s32 index) const
            {
                return (LeafMask&children_[index]) == 0;
            }

            bool isLeaf(s32 index) const
            {
                return (LeafMask&children_[index]) != 0;
            }

            bool isEmpty(s32 index) const
            {
                return (EmptyMask == children_[index]);
            }

            void setLeaf(s32 index, u32 primitiveIndex, u32 numPrimitives)
            {
                children_[index] =  LeafMask | ((primitiveIndex&MaxNumPrimitives) << MaxNumLeafPrimitiveShift) | (numPrimitives&LeafPrimitiveMask);
            }

            void setBBox(const AABB bbox[4]);

            u32 getPrimitiveIndex(s32 index) const
            {
                return (children_[index] >> MaxNumLeafPrimitiveShift) & MaxNumPrimitives;
            }

            u32 getNumPrimitives(s32 index) const
            {
                return (children_[index]&LeafPrimitiveMask);
            }

            void clear();

            __m128 bbox_[2][3];
            u32 children_[4];
            s32 axis0_;
            s32 axis1_;
            s32 axis2_;
            s32 fill_;
        };

        BinQBVH();
        ~BinQBVH();

        void clearShapes();
        void addShape(const Shape::pointer& shape);

        void build();
        bool intersect(Intersection& intersection, const Ray& ray) const;
        bool intersect(const Ray& ray) const;
        s32 getDepth() const{ return depth_;}

        void swap(BinQBVH& rhs);
    private:
        BinQBVH(const BinQBVH&);
        BinQBVH& operator=(const BinQBVH&);

        static const s32 PackAlign = 16;
        struct Primitive4Pack
        {
            __m128 x_[3];
            __m128 y_[3];
            __m128 z_[3];
            //Vector4 x_[3];
            //Vector4 y_[3];
            //Vector4 z_[3];
        };

        inline void getBBox(AABB& bbox, s32 start, s32 end);
        inline Vector3 getCentroid(const ShapePrimitive& primitive) const;
        inline AABB getBBox(const ShapePrimitive& primitive) const;

        static inline void sort(s32 numPrimitives, ShapePrimitive* primitives, const f32* centroids)
        {
            lcore::introsort(numPrimitives, primitives, SortFuncCentroid(centroids));
        }

        static inline void insertionsort(s32 numPrimitives, ShapePrimitive* primitives, const f32* centroids)
        {
            lcore::insertionsort(numPrimitives, primitives, SortFuncCentroid(centroids));
        }

        s32 triangulate();

        void recursiveConstruct(s32 start, s32 numPrimitives, s32 nodeIndex, const AABB& bbox, s32 depth);
        void split(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, f32 invArea, s32 start, s32 numPrimitives, const AABB& bbox);
        void splitMid(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, s32 start, s32 numPrimitives, const AABB& bbox);
        void splitBinned(s32& axis, s32& num_l, s32& num_r, AABB& bbox_l, AABB& bbox_r, f32 area, s32 start, s32 numPrimitives, const AABB& bbox);

#if LRENDER_BINQBVH_SSEPACK
        void pushPrimitives(Node& node, s32 index, s32 start, s32 num);
        //void checkPrimitives(s32 start, s32 num) const;
        void testPrimitives(
            f32& u, f32& v,
            __m128& tmaxSSE,
            Ray& ray,
            const __m128 origin[3],
            const __m128 direction[3],
            ShapePrimitive& shapePrimitive,
            const Shape*& shape,
            s32 start, s32 num) const;
#endif

        f32 SAH_KI_;
        f32 SAH_KT_;

        s32 depth_;

        typedef lcore::vector_arena<f32> F32Vector;
        typedef lcore::vector_arena<AABB> AABBVector;

        lcore::vector_arena<const Shape*> shapes_;
        lcore::vector_arena<Node, Align16Allocator> nodes_;

        VectorShapePrimitive primitives_;
        F32Vector primitiveCentroids_;
        AABBVector primitiveBBoxes_;

#if LRENDER_BINQBVH_SSEPACK
        s32 numPackes_;
        s32 numPackesCapacity_;
        Primitive4Pack* packes_;
#endif

        s32* stack_;
    };

    inline void BinQBVH::getBBox(AABB& bbox, s32 start, s32 end)
    {
        bbox.setInvalid();
        for(s32 i=start; i<end; ++i){
            const ShapePrimitive& primitive = primitives_[i];
            bbox.extend(shapes_[primitive.shape_]->getBBox(primitive.primitive_));
        }
    }

    inline Vector3 BinQBVH::getCentroid(const ShapePrimitive& primitive) const
    {
        LASSERT(0<=primitive.shape_ && primitive.shape_<shapes_.size());
        return shapes_[primitive.shape_]->getCentroid(primitive.primitive_);
    }

    inline AABB BinQBVH::getBBox(const ShapePrimitive& primitive) const
    {
        LASSERT(0<=primitive.shape_ && primitive.shape_<shapes_.size());
        return shapes_[primitive.shape_]->getBBox(primitive.primitive_);
    }
}
#endif //INC_LRENDER_BINQBVH_H__
