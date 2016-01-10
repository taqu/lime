#ifndef INC_LRENDER_MIDBVH2D_H__
#define INC_LRENDER_MIDBVH2D_H__
/**
@file MidBVH2D.h
@author t-sakai
@date 2015/10/07 create
*/
#include "Accelerator.h"
#include "../shape/Shape.h"

namespace lrender
{
    class MidBVH2D
    {
    public:
        static const f32 Epsilon;
        static const s32 MinLeafPrimitives = 15;

        struct Node
        {
            static const u32 LeafMask = ~(((u32)-1)>>1);
            static const u32 EmptyMask = LeafMask;

            static const s32 MaxNumLeafPrimitiveShift = 4;
            static const s32 LeafPrimitiveMask = (0x01U<<MaxNumLeafPrimitiveShift)-1;
            static const u32 MaxNumPrimitives = (~(LeafMask|EmptyMask))>>MaxNumLeafPrimitiveShift;

            void clear()
            {
                bbox_.setInvalid();
                child_ = EmptyMask;
            }

            bool isLeaf() const
            {
                return (LeafMask&child_) != 0;
            }

            bool isEmpty() const
            {
                return (EmptyMask == child_);
            }

            void setLeaf(u32 primitiveIndex, u32 numPrimitives)
            {
                LASSERT(0<numPrimitives);
                child_ =  LeafMask | ((primitiveIndex&MaxNumPrimitives) << MaxNumLeafPrimitiveShift) | (numPrimitives&LeafPrimitiveMask);
            }

            u32 getPrimitiveIndex() const
            {
                return (child_ >> MaxNumLeafPrimitiveShift) & MaxNumPrimitives;
            }

            u32 getNumPrimitives() const
            {
                return (child_&0x0FU);
            }

            AABB2D bbox_;
            u32 child_;
        };


        struct HitRecord
        {
            f32 b0_;
            f32 b1_;
            f32 b2_;
            s32 primitive_;
            const Shape* shape_;
        };

        MidBVH2D();
        ~MidBVH2D();

        void clearShapes();
        void addShape(const Shape::pointer& shape);
        void build();

        bool intersect(HitRecord& hitRecord, const Vector2& point) const;
        s32 getDepth() const{ return depth_;}

        void swap(MidBVH2D& rhs);
    private:
        MidBVH2D(const MidBVH2D&);
        MidBVH2D& operator=(const MidBVH2D&);

        inline void getBBox(AABB2D& bbox, s32 start, s32 end);
        inline Vector2 getCentroid(const ShapePrimitive& primitive) const;
        inline AABB2D getBBox(const ShapePrimitive& primitive) const;

        static inline void sort(s32 numPrimitives, ShapePrimitive* primitives, const f32* centroids)
        {
            lcore::introsort(numPrimitives, primitives, SortFuncCentroid(centroids));
        }

        static inline void insertionsort(s32 numPrimitives, ShapePrimitive* primitives, const f32* centroids)
        {
            lcore::insertionsort(numPrimitives, primitives, SortFuncCentroid(centroids));
        }

        s32 triangulate();
        void recursiveConstruct(s32 start, s32 numPrimitives, s32 nodeIndex, s32 depth);

        s32 depth_;

        typedef lcore::vector_arena<f32> F32Vector;
        typedef lcore::vector_arena<AABB2D> AABBVector;

        lcore::vector_arena<const Shape*> shapes_;
        lcore::vector_arena<Node, lcore::vector_arena_static_inc_size<16>, Align16Allocator> nodes_;

        VectorShapePrimitive primitives_;
        F32Vector primitiveCentroids_;
        AABBVector primitiveBBoxes_;
        s32* stack_;
    };

    inline void MidBVH2D::getBBox(AABB2D& bbox, s32 start, s32 end)
    {
        bbox.setInvalid();
        Vector2 uvs[3];
        for(s32 i=start; i<end; ++i){
            const ShapePrimitive& primitive = primitives_[i];
            shapes_[primitive.shape_]->getTexcoord(uvs, primitive.primitive_);
            bbox.extend(uvs[0]);
            bbox.extend(uvs[1]);
            bbox.extend(uvs[2]);
        }
    }

    inline Vector2 MidBVH2D::getCentroid(const ShapePrimitive& primitive) const
    {
        LASSERT(0<=primitive.shape_ && primitive.shape_<shapes_.size());
        Vector2 uvs[3];
        shapes_[primitive.shape_]->getTexcoord(uvs, primitive.primitive_);
        f32 x = (uvs[0].x_+uvs[1].x_+uvs[2].x_)*(1.0f/3.0f);
        f32 y = (uvs[0].y_+uvs[1].y_+uvs[2].y_)*(1.0f/3.0f);
        return Vector2(x, y);
    }

    inline AABB2D MidBVH2D::getBBox(const ShapePrimitive& primitive) const
    {
        LASSERT(0<=primitive.shape_ && primitive.shape_<shapes_.size());
        Vector2 uvs[3];
        shapes_[primitive.shape_]->getTexcoord(uvs, primitive.primitive_);
        AABB2D aabb;
        aabb.bmin_ = uvs[0];
        aabb.bmax_ = uvs[0];

        aabb.extend(uvs[1]);
        aabb.extend(uvs[2]);
        return aabb;
    }
}
#endif //INC_LRENDER_MIDBVH2D_H__
