#ifndef INC_LRENDER_MESHINSTANCE_H__
#define INC_LRENDER_MESHINSTANCE_H__
/**
@file MeshInstance.h
@author t-sakai
@date 2015/09/15 create
*/
#include "../lrender.h"
#include "../core/Buffer.h"
#include "Mesh.h"

namespace lrender
{
    class MeshInstance : public Shape
    {
    public:
        typedef lcore::intrusive_ptr<MeshInstance> pointer;

        MeshInstance();
        MeshInstance(
            Mesh::pointer& mesh,
            s32 offset,
            s32 numTriangles);
        ~MeshInstance();

        inline const lmath::Vector3& getBBoxMin() const;
        inline const lmath::Vector3& getBBoxMax() const;

        virtual s32 getPrimitiveCount() const;
        virtual void getBBox(AABB& bbox) const;

        virtual bool intersect(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const;

        virtual void getTriangles(VectorShapePrimitive& triangles) const;
        virtual void getTexcoord(Vector2 uvs[3], s32 primitive) const;
        virtual void getPrimitive(PrimitiveSample& sample, s32 primitive) const;

        virtual Vector3 getCentroid(s32 primitive) const;
        virtual AABB getBBox(s32 primitive) const;
        virtual void getPrimitiveSOA(Vector4 vx[3], Vector4 vy[3], Vector4 vz[3], s32 index, s32 primitive) const;

        virtual void getIntersection(Intersection& intersection, const RayDifferential& ray, f32 b1, f32 b2, s32 primitive) const;
        virtual f32 getSurfaceArea(s32 primitive) const;

        inline s32 getPrimitiveOffset() const;
        inline void setPrimitiveCount(s32 num);

        inline void setBBoxMin(const lmath::Vector3& bmin);
        inline void setBBoxMax(const lmath::Vector3& bmax);

        void swap(MeshInstance& rhs);
    private:
        MeshInstance(const MeshInstance&);
        const MeshInstance& operator=(const MeshInstance&);

        Mesh::pointer mesh_;
        s32 offset_;
        s32 numTriangles_;
        lmath::Vector3 bmin_, bmax_;
    };

    inline const lmath::Vector3& MeshInstance::getBBoxMin() const
    {
        return bmin_;
    }

    inline const lmath::Vector3& MeshInstance::getBBoxMax() const
    {
        return bmax_;
    }

    inline s32 MeshInstance::getPrimitiveOffset() const
    {
        return offset_;
    }

    inline void MeshInstance::setPrimitiveCount(s32 num)
    {
        numTriangles_ = num;
    }

    inline void MeshInstance::setBBoxMin(const lmath::Vector3& bmin)
    {
        bmin_ = bmin;
    }

    inline void MeshInstance::setBBoxMax(const lmath::Vector3& bmax)
    {
        bmax_ = bmax;
    }
}
#endif //INC_LRENDER_MESHINSTANCE_H__
