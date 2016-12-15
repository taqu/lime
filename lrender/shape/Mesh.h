#ifndef INC_LRENDER_MESH_H__
#define INC_LRENDER_MESH_H__
/**
@file Mesh.h
@author t-sakai
@date 2013/05/09 create
*/
#include "../lrender.h"
#include "../core/Buffer.h"
#include "../shape/Shape.h"

namespace lrender
{
    class Mesh : public Shape
    {
    public:
        typedef lcore::intrusive_ptr<Mesh> pointer;

        static bool create(
            Mesh& mesh,
            const lmath::Vector3& bmin,
            const lmath::Vector3& bmax,
            BufferVector3& positions,
            BufferVector3& normals,
            BufferVector2& texcoords,
            BufferVector4& colors,
            BufferTriangle& triangles);

        Mesh();
        virtual ~Mesh();

        inline const lmath::Vector3& getBBoxMin() const;
        inline const lmath::Vector3& getBBoxMax() const;

        virtual s32 getPrimitiveCount() const;
        virtual void getBBox(AABB& bbox) const;
        //virtual bool intersect(const Ray& ray, f32& t, Intersection& intersection);
        //virtual bool intersectP(const Ray& ray);

        virtual bool intersect(f32& t, f32& u, f32& v, s32 primitive, const Ray& ray) const;
        virtual bool intersectBothSides(f32& t, f32& b1, f32& b2, s32 primitive, const Ray& ray) const;

        virtual void getVertices(VectorVertexSample& vertices) const;
        virtual void getTriangles(VectorShapePrimitive& triangles) const;
        virtual void getTexcoord(Vector2 uvs[3], s32 primitive) const;
        virtual void getPrimitive(PrimitiveSample& sample, s32 primitive) const;

        virtual const Vector4& getColor(s32 vindex) const;
        virtual void setColor(s32 vindex, const Vector4& color);

        virtual Vector3 getCentroid(s32 primitive) const;
        virtual AABB getBBox(s32 primitive) const;
        virtual void getPrimitiveSOA(Vector4 vx[3], Vector4 vy[3], Vector4 vz[3], s32 index, s32 primitive) const;

        virtual void getIntersection(Intersection& intersection, const RayDifferential& ray, f32 u, f32 v, s32 primitive) const;

        virtual f32 getSurfaceArea(s32 primitive) const;
        virtual f32 getSurfaceArea() const;

        void getTexcoord(Vector2 uvs[3], const Triangle& triangle) const;

        void swap(Mesh& rhs);

        inline const BufferVector3& getPositions() const;
        inline const BufferVector3& getNormals() const;
        inline const BufferVector2& getTexcoords() const;
        inline const BufferVector4& getColors() const;
        inline const BufferTriangle& getTriangles() const;

    private:
        Mesh(const Mesh&);
        const Mesh& operator=(const Mesh&);

        friend class MeshInstance;

        lmath::Vector3 bmin_, bmax_;

        BufferVector3 positions_;
        BufferVector3 normals_;
        BufferVector2 texcoords_;
        BufferVector4 colors_;
        BufferTriangle triangles_;
    };

    inline const lmath::Vector3& Mesh::getBBoxMin() const
    {
        return bmin_;
    }

    inline const lmath::Vector3& Mesh::getBBoxMax() const
    {
        return bmax_;
    }

    inline const BufferVector3& Mesh::getPositions() const
    {
        return positions_;
    }

    inline const BufferVector3& Mesh::getNormals() const
    {
        return normals_;
    }

    inline const BufferVector2& Mesh::getTexcoords() const
    {
        return texcoords_;
    }

    inline const BufferVector4& Mesh::getColors() const
    {
        return colors_;
    }

    inline const BufferTriangle& Mesh::getTriangles() const
    {
        return triangles_;
    }
}
#endif //INC_LRENDER_MESH_H__
