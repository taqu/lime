/**
@file MeshInstance.cpp
@author t-sakai
@date 2015/09/15 create
*/
#include "shape/MeshInstance.h"
#include "core/AABB.h"
#include "core/Ray.h"
#include "core/Intersection.h"

namespace lrender
{
    MeshInstance::MeshInstance()
        :mesh_(NULL)
        ,offset_(0)
        ,numTriangles_(0)
    {
    }

    MeshInstance::MeshInstance(
        Mesh::pointer& mesh,
        s32 offset,
        s32 numTriangles)
        :mesh_(mesh)
        ,offset_(offset)
        ,numTriangles_(numTriangles)
    {
    }

    MeshInstance::~MeshInstance()
    {
    }

    void MeshInstance::swap(MeshInstance& rhs)
    {
        lcore::swap(components_, rhs.components_);
        lcore::swap(mesh_, rhs.mesh_);
        bmin_.swap(rhs.bmin_);
        bmax_.swap(rhs.bmax_);
        lcore::swap(offset_, rhs.offset_);
        lcore::swap(numTriangles_, rhs.numTriangles_);
    }

    s32 MeshInstance::getPrimitiveCount() const
    {
        return numTriangles_;
    }

    void MeshInstance::getBBox(AABB& bbox) const
    {
        bbox.bmin_ = bmin_;
        bbox.bmax_ = bmax_;
    }

    bool MeshInstance::intersect(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const
    {
        LASSERT(NULL != mesh_);

        primitive += offset_;

        const Triangle& triangle = mesh_->triangles_[primitive];
        return testRayTriangleFront(t, b1, b2, ray,
            mesh_->positions_[triangle.indices_[0]],
            mesh_->positions_[triangle.indices_[1]],
            mesh_->positions_[triangle.indices_[2]]);
    }

    bool MeshInstance::intersectBothSides(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const
    {
        LASSERT(NULL != mesh_);

        primitive += offset_;

        const Triangle& triangle = mesh_->triangles_[primitive];
        return lmath::Result_Fail != testRayTriangleBoth(
            t, b1, b2, ray,
            mesh_->positions_[triangle.indices_[0]],
            mesh_->positions_[triangle.indices_[1]],
            mesh_->positions_[triangle.indices_[2]]);
    }

    void MeshInstance::getVertices(VectorVertexSample& vertices) const
    {
        mesh_->getVertices(vertices);
    }

    void MeshInstance::getTriangles(VectorShapePrimitive& triangles) const
    {
        LASSERT(NULL != mesh_);

        ShapePrimitive shapePrimitive;
        shapePrimitive.shape_ = getID();

        for(s32 i=0; i<numTriangles_; ++i){
            shapePrimitive.primitive_ = i;
            triangles.push_back(shapePrimitive);
        }
    }

    void MeshInstance::getTexcoord(Vector2 uvs[3], s32 primitive) const
    {
        LASSERT(NULL != mesh_);
        primitive += offset_;
        mesh_->getTexcoord(uvs, primitive);
    }

    void MeshInstance::getPrimitive(PrimitiveSample& sample, s32 primitive) const
    {
        LASSERT(NULL != mesh_);
        primitive += offset_;
        mesh_->getPrimitive(sample, primitive);
    }

    const Vector4& MeshInstance::getColor(s32 vindex) const
    {
        LASSERT(0<=vindex && vindex<mesh_->colors_.size());
        return mesh_->colors_[vindex];
    }

    void MeshInstance::setColor(s32 vindex, const Vector4& color)
    {
        LASSERT(0<=vindex && vindex<mesh_->colors_.size());
        mesh_->colors_[vindex] = color;
    }

    Vector3 MeshInstance::getCentroid(s32 primitive) const
    {
        LASSERT(NULL != mesh_);

        primitive += offset_;

        const Triangle& triangle = mesh_->triangles_[primitive];

        Vector3 p = add(mesh_->positions_[triangle.indices_[0]], mesh_->positions_[triangle.indices_[1]]);
        p += mesh_->positions_[triangle.indices_[2]];
        p *= (1.0f/3.0f);
        return p;
    }

    AABB MeshInstance::getBBox(s32 primitive) const
    {
        LASSERT(NULL != mesh_);

        primitive += offset_;

        const Triangle& triangle = mesh_->triangles_[primitive];

        Vector3 bmin = mesh_->positions_[triangle.indices_[0]];
        Vector3 bmax = mesh_->positions_[triangle.indices_[0]];
        for(s32 i=1; i<3; ++i){
            const Vector3& p = mesh_->positions_[triangle.indices_[i]];
            bmin.x_ = lcore::minimum(bmin.x_, p.x_);
            bmin.y_ = lcore::minimum(bmin.y_, p.y_);
            bmin.z_ = lcore::minimum(bmin.z_, p.z_);

            bmax.x_ = lcore::maximum(bmax.x_, p.x_);
            bmax.y_ = lcore::maximum(bmax.y_, p.y_);
            bmax.z_ = lcore::maximum(bmax.z_, p.z_);
        }
        return AABB(bmin, bmax);
    }

    void MeshInstance::getPrimitiveSOA(Vector4 vx[3], Vector4 vy[3], Vector4 vz[3], s32 index, s32 primitive) const
    {
        LASSERT(NULL != mesh_);

        primitive += offset_;

        const Triangle& triangle = mesh_->triangles_[primitive];
        for(s32 i = 0; i < 3; ++i){
            const Vector3& p = mesh_->positions_[triangle.indices_[i]];
            vx[i][index] = p.x_;
            vy[i][index] = p.y_;
            vz[i][index] = p.z_;
        }
    }

    void MeshInstance::getIntersection(Intersection& intersection, const RayDifferential& ray, f32 b1, f32 b2, s32 primitive) const
    {
        mesh_->getIntersection(intersection, ray, b1, b2, primitive+offset_);
        intersection.shape_ = this;
    }

    f32 MeshInstance::getSurfaceArea(s32 primitive) const
    {
        return mesh_->getSurfaceArea(primitive);
    }

    f32 MeshInstance::getSurfaceArea() const
    {
        s32 end = offset_ + numTriangles_;
        f32 sum = 0.0f;
        for(s32 i=offset_; i<end; ++i){
            sum += mesh_->getSurfaceArea(i);
        }
        return sum;
    }
}
