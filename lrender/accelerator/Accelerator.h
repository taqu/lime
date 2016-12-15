#ifndef INC_LRENDER_ACCELERATOR_H__
#define INC_LRENDER_ACCELERATOR_H__
/**
@file Accelerator.h
@author t-sakai
@date 2015/09/14 create
*/
#include <lcore/Array.h>
#include <lcore/Sort.h>
#include "../lrender.h"

namespace lrender
{
    struct ShapePrimitive
    {
        //static const u32 NumMaxShapes = 0x01U<<8;
        //static const u32 NumMaxPrimitives = 0x01U<<24;

        //inline u32 getShapeID() const{ return (shapePrimitive_>>24) & 0xFFU;}
        //inline void setShapeID(u32 id){ shapePrimitive_ = (shapePrimitive_ & 0xFFFFFFU) | (id<<24);}
        //inline u32 getPrimitiveID() const{ return shapePrimitive_ & 0xFFFFFFU;}
        //inline void setPrimitiveID(u32 primitive){ shapePrimitive_ = (shapePrimitive_&0xFF000000U) | (primitive & 0xFFFFFFU);}

        s32 shape_;
        s32 pack_;
        s32 primitive_;
        s32 index_;
    };

    typedef lcore::Array<ShapePrimitive> VectorShapePrimitive;

    struct SortFuncCentroid
    {
        SortFuncCentroid(const f32* centroids)
            :centroids_(centroids)
        {}

        bool operator()(const ShapePrimitive& i0, const ShapePrimitive& i1) const
        {
            return centroids_[i0.index_] < centroids_[i1.index_];
        }

        const f32* centroids_;
    };

    inline f32 getHalfArea(const Vector3& v)
    {
        return v.x_*v.y_ + v.y_*v.z_ + v.z_*v.x_;
    }
}
#endif //INC_LRENDER_ACCELERATOR_H__
