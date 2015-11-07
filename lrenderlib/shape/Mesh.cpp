/**
@file Mesh.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "Mesh.h"
#include "../core/AABB.h"
#include "../core/Ray.h"
#include "../core/Intersection.h"

namespace lrender
{
    bool Mesh::create(
        Mesh& mesh,
        const lmath::Vector3& bmin,
        const lmath::Vector3& bmax,
        BufferVector3& points,
        BufferVector3& normals,
        BufferVector2& texcoords,
        BufferTriangle& triangles)
    {
        mesh.bmin_ = bmin;
        mesh.bmax_ = bmax;

        if(0<normals.size()){
            mesh.addComponent(Shape::Component_Normal);
        }
        if(0<texcoords.size()){
            mesh.addComponent(Shape::Component_Texcoord);
        }

        mesh.points_.swap(points);
        mesh.normals_.swap(normals);
        mesh.texcoords_.swap(texcoords);
        mesh.triangles_.swap(triangles);

        return true;
    }

    Mesh::Mesh()
    {
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::swap(Mesh& rhs)
    {
        bmin_.swap(rhs.bmin_);
        bmax_.swap(rhs.bmax_);
        points_.swap(rhs.points_);
        normals_.swap(rhs.normals_);
        texcoords_.swap(rhs.texcoords_);
        triangles_.swap(rhs.triangles_);
    }

    s32 Mesh::getPrimitiveCount() const
    {
        return triangles_.size();
    }

    void Mesh::getBBox(AABB& bbox) const
    {
        bbox.bmin_ = bmin_;
        bbox.bmax_ = bmax_;
    }

    //bool Mesh::intersect(const Ray& ray, f32& t, Intersection& intersection)
    //{
    //    return false;
    //}

    //bool Mesh::intersectP(const Ray& ray)
    //{
    //    return false;
    //}

    bool Mesh::intersect(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const
    {
        const Triangle& triangle = triangles_[primitive];
        return testRayTriangle(t, b1, b2, ray, points_[triangle.indices_[0]], points_[triangle.indices_[1]], points_[triangle.indices_[2]]);
    }

    void Mesh::getTriangles(VectorShapePrimitive& triangles) const
    {
        ShapePrimitive shapePrimitive;
        shapePrimitive.shape_ = getID();

        for(s32 i=0; i<triangles_.size(); ++i){
            shapePrimitive.primitive_ = i;
            triangles.push_back(shapePrimitive);
        }
    }

    void Mesh::getTexcoord(Vector2 uvs[3], s32 primitive) const
    {
        if(hasComponent(Shape::Component_Texcoord)){
            const Triangle& triangle = triangles_[primitive];
            uvs[0] = texcoords_[triangle.indices_[0]];
            uvs[1] = texcoords_[triangle.indices_[1]];
            uvs[2] = texcoords_[triangle.indices_[2]];

        }else{
            uvs[0].set(0.0f, 0.0f);
            uvs[1].set(1.0f, 0.0f);
            uvs[2].set(1.0f, 1.0f);
        }
    }

    void Mesh::getPrimitive(PrimitiveSample& sample, s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];
        s32 i0 = triangle.indices_[0];
        s32 i1 = triangle.indices_[1];
        s32 i2 = triangle.indices_[2];

        sample.positions_[0] = points_[i0];
        sample.positions_[1] = points_[i1];
        sample.positions_[2] = points_[i2];

        if(hasComponent(Component_Normal)){
            sample.normals_[0] = normals_[i0];
            sample.normals_[1] = normals_[i1];
            sample.normals_[2] = normals_[i2];

        }else{
            Vector3 d01;
            d01.sub(points_[i1], points_[i0]);
            Vector3 d02;
            d02.sub(points_[i2], points_[i0]);

            Vector3 n;
            n.cross(d01, d02);
            n.normalizeChecked();
            sample.normals_[0] = n;
            sample.normals_[1] = n;
            sample.normals_[2] = n;
        }

        if(hasComponent(Component_Texcoord)){
            sample.uvs_[0] = texcoords_[i0];
            sample.uvs_[1] = texcoords_[i1];
            sample.uvs_[2] = texcoords_[i2];
        }else{
            sample.uvs_[0].zero();
            sample.uvs_[1].zero();
            sample.uvs_[2].zero();
        }
    }

    Vector3 Mesh::getCentroid(s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];

        Vector3 p;
        p.add(points_[triangle.indices_[0]], points_[triangle.indices_[1]]);
        p += points_[triangle.indices_[2]];
        p *= (1.0f/3.0f);
        return p;
    }

    AABB Mesh::getBBox(s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];

        Vector3 bmin = points_[triangle.indices_[0]];
        Vector3 bmax = points_[triangle.indices_[0]];
        for(s32 i=1; i<3; ++i){
            const Vector3& p = points_[triangle.indices_[i]];
            bmin.x_ = lcore::minimum(bmin.x_, p.x_);
            bmin.y_ = lcore::minimum(bmin.y_, p.y_);
            bmin.z_ = lcore::minimum(bmin.z_, p.z_);

            bmax.x_ = lcore::maximum(bmax.x_, p.x_);
            bmax.y_ = lcore::maximum(bmax.y_, p.y_);
            bmax.z_ = lcore::maximum(bmax.z_, p.z_);
        }
        return AABB(bmin, bmax);
    }

    void Mesh::getPrimitiveSOA(Vector4 vx[3], Vector4 vy[3], Vector4 vz[3], s32 index, s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];
        for(s32 i = 0; i < 3; ++i){
            const Vector3& p = points_[triangle.indices_[i]];
            vx[i][index] = p.x_;
            vy[i][index] = p.y_;
            vz[i][index] = p.z_;
        }
    }

    void Mesh::getIntersection(Intersection& intersection, const RayDifferential& ray, f32 b1, f32 b2, s32 primitive) const
    {
        intersection.point_.muladd(ray.tmax_, ray.direction_, ray.origin_);
        intersection.shape_ = this;
        intersection.setPrimitive(primitive);

        const Triangle& triangle = triangles_[primitive];

        const Vector3& p0 = points_[triangle.indices_[0]];
        const Vector3& p1 = points_[triangle.indices_[1]];
        const Vector3& p2 = points_[triangle.indices_[2]];

        Vector2 uvs[3];
        getTexcoord(uvs, triangle);

        Vector3 e0, e1;
        e0.sub(p1, p0);
        e1.sub(p2, p0);

        f32 b0 = 1.0f - b1 - b2;
        intersection.uv_.x_ = b0 * uvs[0].x_ + b1 * uvs[1].x_ + b2 * uvs[2].x_;
        intersection.uv_.y_ = b0 * uvs[0].y_ + b1 * uvs[1].y_ + b2 * uvs[2].y_;
        intersection.geometricNormal_.cross(e0, e1);
        intersection.geometricNormal_.normalizeChecked();

        Vector3 dn02, dn12;
        if(hasComponent(Shape::Component_Normal)){
            const Vector3& n0 = normals_[triangle.indices_[0]];
            const Vector3& n1 = normals_[triangle.indices_[1]];
            const Vector3& n2 = normals_[triangle.indices_[2]];

            intersection.shadingNormal_.mul(b0, n0);
            intersection.shadingNormal_.muladd(b1, n1, intersection.shadingNormal_);
            intersection.shadingNormal_.muladd(b2, n2, intersection.shadingNormal_);
            dn02.sub(n0, n2);
            dn12.sub(n1, n2);
            intersection.shadingNormal_.normalizeChecked();

        }else{
            intersection.shadingNormal_ = intersection.geometricNormal_;
            dn02.zero();
            dn12.zero();
        }

        Vector2 duv02; duv02.sub(uvs[0], uvs[2]);
        Vector2 duv12; duv12.sub(uvs[1], uvs[2]);

        Vector3 dp02, dp12;
        dp02.sub(p0, p2);
        dp12.sub(p1, p2);
        f32 determinant = duv02[0]*duv12[1] - duv02[1]*duv12[0];
        if(lmath::isZero(determinant)){
            lrender::orthonormalBasis(intersection.dpdu_, intersection.dpdv_, intersection.shadingNormal_);
            intersection.dndu_.zero();
            intersection.dndv_.zero();

        }else{
            f32 invDet = 1.0f/determinant;
            intersection.dpdu_.mul(duv12[1], dp02);
            intersection.dpdu_.muladd(-duv02[1], dp12, intersection.dpdu_);
            intersection.dpdu_ *= invDet;

            intersection.dpdv_.mul(-duv12[0], dp02);
            intersection.dpdv_.muladd(duv02[0], dp12, intersection.dpdv_);
            intersection.dpdv_ *= invDet;

            intersection.dndu_.mul(duv12[1], dn02);
            intersection.dndu_.muladd(-duv02[1], dn12, intersection.dndu_);
            intersection.dndu_ *= invDet;

            intersection.dndv_.mul(-duv12[0], dn02);
            intersection.dndv_.muladd(duv02[0], dn12, intersection.dndv_);
            intersection.dndv_ *= invDet;
        }

        intersection.binormal0_ = normalizeChecked(intersection.dpdu_);
        intersection.binormal1_.cross(intersection.binormal0_, intersection.shadingNormal_);

        f32 l2 = intersection.binormal1_.lengthSqr();
        if(DOT_EPSILON<l2){
            intersection.binormal1_.normalize(l2);
            intersection.binormal0_.cross(intersection.binormal1_, intersection.shadingNormal_);
        }else{
            lrender::orthonormalBasis(intersection.binormal0_, intersection.binormal1_, intersection.shadingNormal_);
        }

        //if(intersection.point_.isNan()
        //    || intersection.shadingNormal_.isNan()
        //    || intersection.geometricNormal_.isNan()
        //    || intersection.binormal0_.isNan()
        //    || intersection.binormal1_.isNan()
        //    || intersection.dpdu_.isNan()
        //    || intersection.dpdv_.isNan()
        //    || intersection.dndu_.isNan()
        //    || intersection.dndv_.isNan())
        //{
        //    lcore::Log("nan!");
        //}
        intersection.computeDifferentials(ray);
    }


    f32 Mesh::getSurfaceArea(s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];
        lmath::Vector3 du, dv;
        du.sub(points_[triangle.indices_[1]], points_[triangle.indices_[0]]);
        dv.sub(points_[triangle.indices_[2]], points_[triangle.indices_[0]]);
        lmath::Vector3 c;
        c.cross(du, dv);
        return 0.5f * c.length();
    }

    void Mesh::getTexcoord(Vector2 uvs[3], const Triangle& triangle) const
    {
        if(hasComponent(Shape::Component_Texcoord)){
            uvs[0] = texcoords_[triangle.indices_[0]];
            uvs[1] = texcoords_[triangle.indices_[1]];
            uvs[2] = texcoords_[triangle.indices_[2]];

        }else{
            uvs[0].set(0.0f, 0.0f);
            uvs[1].set(1.0f, 0.0f);
            uvs[2].set(1.0f, 1.0f);
        }
    }
}
