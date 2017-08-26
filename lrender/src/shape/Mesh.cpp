/**
@file Mesh.cpp
@author t-sakai
@date 2013/05/09 create
*/
#include "shape/Mesh.h"
#include <lmath/geometry/RayTest.h>
#include "core/AABB.h"
#include "core/Ray.h"
#include "core/Intersection.h"

namespace lrender
{
    bool Mesh::create(
        Mesh& mesh,
        const lmath::Vector3& bmin,
        const lmath::Vector3& bmax,
        BufferVector3& positions,
        BufferVector3& normals,
        BufferVector2& texcoords,
        BufferVector4& colors,
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
        if(0<colors.size()){
            mesh.addComponent(Shape::Component_Color);
        }

        mesh.positions_.swap(positions);
        mesh.normals_.swap(normals);
        mesh.texcoords_.swap(texcoords);
        mesh.colors_.swap(colors);
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
        positions_.swap(rhs.positions_);
        normals_.swap(rhs.normals_);
        texcoords_.swap(rhs.texcoords_);
        colors_.swap(rhs.colors_);
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
        return testRayTriangleFront(t, b1, b2, ray,
            positions_[triangle.indices_[0]],
            positions_[triangle.indices_[1]],
            positions_[triangle.indices_[2]]);
    }

    bool Mesh::intersectBothSides(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const
    {
        const Triangle& triangle = triangles_[primitive];
        return lmath::Result_Fail != lmath::testRayTriangleBoth(
            t, b1, b2, ray,
            positions_[triangle.indices_[0]],
            positions_[triangle.indices_[1]],
            positions_[triangle.indices_[2]]);
    }

    void Mesh::getVertices(VectorVertexSample& vertices) const
    {
        lrender::create(vertices, positions_.size());

        PrimitiveSample primitiveSample;
        Vector4 colors[3];
        for(s32 i=0; i<getPrimitiveCount(); ++i){
            getPrimitive(primitiveSample, i);

            const Triangle& triangle = triangles_[i];
            s32 i0 = triangle.indices_[0];
            s32 i1 = triangle.indices_[1];
            s32 i2 = triangle.indices_[2];

            if(hasComponent(Component_Color)){
                colors[0] = colors_[i0];
                colors[1] = colors_[i1];
                colors[2] = colors_[i2];
            } else{
                colors[0] = Vector4::one();
                colors[1] = Vector4::one();
                colors[2] = Vector4::one();
            }

            for(s32 j=0; j<3; ++j){
                s32 vindex = triangle.indices_[j];
                vertices[vindex].position_ = primitiveSample.positions_[j];
                vertices[vindex].normal_ = primitiveSample.normals_[j];
                vertices[vindex].uv_ = primitiveSample.uvs_[j];
                vertices[vindex].color_ = colors[j];
            }
        }
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

        sample.positions_[0] = positions_[i0];
        sample.positions_[1] = positions_[i1];
        sample.positions_[2] = positions_[i2];

        if(hasComponent(Component_Normal)){
            sample.normals_[0] = normals_[i0];
            sample.normals_[1] = normals_[i1];
            sample.normals_[2] = normals_[i2];

        }else{
            Vector3 d01 = sub(positions_[i1], positions_[i0]);
            Vector3 d02 = sub(positions_[i2], positions_[i0]);

            Vector3 n = normalizeChecked(cross(d01, d02));
            sample.normals_[0] = n;
            sample.normals_[1] = n;
            sample.normals_[2] = n;
        }

        if(hasComponent(Component_Texcoord)){
            sample.uvs_[0] = texcoords_[i0];
            sample.uvs_[1] = texcoords_[i1];
            sample.uvs_[2] = texcoords_[i2];
        }else{
            sample.uvs_[0] = Vector2::zero();
            sample.uvs_[1] = Vector2::zero();
            sample.uvs_[2] = Vector2::zero();
        }
    }

    const Vector4& Mesh::getColor(s32 vindex) const
    {
        LASSERT(0<=vindex && vindex<colors_.size());
        return colors_[vindex];
    }

    void Mesh::setColor(s32 vindex, const Vector4& color)
    {
        LASSERT(0<=vindex && vindex<colors_.size());
        colors_[vindex] = color;
    }

    Vector3 Mesh::getCentroid(s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];

        Vector3 p = positions_[triangle.indices_[0]] + positions_[triangle.indices_[1]] + positions_[triangle.indices_[2]];
        p *= (1.0f/3.0f);
        return p;
    }

    AABB Mesh::getBBox(s32 primitive) const
    {
        const Triangle& triangle = triangles_[primitive];

        Vector3 bmin = positions_[triangle.indices_[0]];
        Vector3 bmax = positions_[triangle.indices_[0]];
        for(s32 i=1; i<3; ++i){
            const Vector3& p = positions_[triangle.indices_[i]];
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
            const Vector3& p = positions_[triangle.indices_[i]];
            vx[i][index] = p.x_;
            vy[i][index] = p.y_;
            vz[i][index] = p.z_;
        }
    }

    void Mesh::getIntersection(Intersection& intersection, const RayDifferential& ray, f32 b1, f32 b2, s32 primitive) const
    {
        intersection.point_ = muladd(ray.tmax_, ray.direction_, ray.origin_);
        intersection.shape_ = this;
        intersection.setPrimitive(primitive);

        const Triangle& triangle = triangles_[primitive];

        const Vector3& p0 = positions_[triangle.indices_[0]];
        const Vector3& p1 = positions_[triangle.indices_[1]];
        const Vector3& p2 = positions_[triangle.indices_[2]];

        Vector2 uvs[3];
        getTexcoord(uvs, triangle);

        f32 b0 = 1.0f - b1 - b2;
        intersection.b0_ = b0;
        intersection.b1_ = b1;
        intersection.b2_ = b2;
        intersection.uv_.x_ = b0 * uvs[0].x_ + b1 * uvs[1].x_ + b2 * uvs[2].x_;
        intersection.uv_.y_ = b0 * uvs[0].y_ + b1 * uvs[1].y_ + b2 * uvs[2].y_;

        Vector3 e0 = normalize(p1 - p0);
        Vector3 e1 = normalize(p2 - p0);
        intersection.geometricNormal_ = normalizeChecked(cross(e0, e1));

        Vector3 dn02, dn12;
        if(hasComponent(Shape::Component_Normal)){
            const Vector3& n0 = normals_[triangle.indices_[0]];
            const Vector3& n1 = normals_[triangle.indices_[1]];
            const Vector3& n2 = normals_[triangle.indices_[2]];

            intersection.shadingNormal_ = normalizeChecked(b0*n0 + b1*n1 + b2*n2);
            dn02 = sub(n0, n2);
            dn12 = sub(n1, n2);

        }else{
            intersection.shadingNormal_ = intersection.geometricNormal_;
            dn02 = Vector3::zero();
            dn12 = Vector3::zero();
        }

        Vector2 duv02 = uvs[0] - uvs[2];
        Vector2 duv12 = uvs[1] - uvs[2];

        Vector3 dp02 = p0 - p2;
        Vector3 dp12 = p1 - p2;
        f32 determinant = duv02[0]*duv12[1] - duv02[1]*duv12[0];
        if(lmath::isZero(determinant)){
            lrender::orthonormalBasis(intersection.dpdu_, intersection.dpdv_, intersection.shadingNormal_);
            intersection.dndu_ = Vector3::zero();
            intersection.dndv_ = Vector3::zero();

        }else{
            f32 invDet = 1.0f/determinant;
            intersection.dpdu_ = (duv12[1]*dp02) + (-duv02[1]*dp12);
            intersection.dpdu_ *= invDet;

            intersection.dpdv_ = (-duv12[0]*dp02) + (duv02[0]*dp12);
            intersection.dpdv_ *= invDet;

            intersection.dndu_ = (duv12[1]*dn02) + (-duv02[1]*dn12);
            intersection.dndu_ *= invDet;

            intersection.dndv_ = (-duv12[0]*dn02) + (duv02[0]*dn12);
            intersection.dndv_ *= invDet;
        }

        intersection.binormal0_ = normalizeChecked(intersection.dpdu_);
        intersection.binormal1_ = cross(intersection.binormal0_, intersection.shadingNormal_);

        f32 l2 = intersection.binormal1_.lengthSqr();
        if(DOT_EPSILON<l2){
            intersection.binormal1_ = normalize(intersection.binormal1_, l2);
            intersection.binormal0_ = cross(intersection.binormal1_, intersection.shadingNormal_);
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
        lmath::Vector3 du = sub(positions_[triangle.indices_[1]], positions_[triangle.indices_[0]]);
        lmath::Vector3 dv = sub(positions_[triangle.indices_[2]], positions_[triangle.indices_[0]]);
        lmath::Vector3 c;
        c = cross(du, dv);
        return 0.5f * c.length();
    }

    f32 Mesh::getSurfaceArea() const
    {
        f32 sum = 0.0f;
        for(s32 i=0; i<triangles_.size(); ++i){
            sum += Mesh::getSurfaceArea(i);
        }
        return sum;
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
