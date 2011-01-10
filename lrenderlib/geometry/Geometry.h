#ifndef INC_LRENDER_GEOMETRY_H__
#define INC_LRENDER_GEOMETRY_H__
/**
@file Geometry.h
@author t-sakai
@date 2010/01/12 create

*/
#include "../lrendercore.h"
#include <lcore/smart_ptr/intrusive_ptr.h>
#include <lgraphics/lgraphicscore.h>
#include "Shape.h"

namespace lgraphics
{
    class VertexDeclaration;
    class VertexBufferUP;
    class IndexBufferUP;
}

namespace lrender
{
     class GeometryBuffer;

    //----------------------------------------------------
    //---
    //--- Geometry
    //---
    //----------------------------------------------------
    class Geometry : public Shape
    {
    public:
        typedef smart_ptr::intrusive_ptr<GeometryBuffer> GeomBufferPtr;

        Geometry();
        Geometry(GeomBufferPtr& geomBuffer, u32 count, s32 materialIndex);

        virtual ~Geometry();

        lgraphics::PrimitiveType getType() const;
        u32 getPrimitiveCount() const{ return count_;}

        s32 getMaterialIndex() const{ return materialIndex_;}

        lgraphics::VertexBufferUP& getVertexBuffer();
        const lgraphics::VertexBufferUP& getVertexBuffer() const;

        lgraphics::IndexBufferUP& getIndexBuffer();
        const lgraphics::IndexBufferUP& getIndexBuffer() const;

        lgraphics::VertexDeclaration& getDecl();

        void swap(Geometry& rhs);

        void calcBBox(Vector3& bmin, Vector3& bmax) const;
        void calcMedian(Vector3& median) const;

        // Shapeのメソッド実装
        //---------------------------------------------------------------------------
        virtual u32 calcNumElements() const;
        virtual bool getElements(Element* elements, u32 bufferCount);

        virtual void calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const;
        virtual void calcMedian(u32 index, Vector3& median) const;

        virtual bool hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const;
        virtual void calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const;
    private:
        Geometry(const Geometry&);
        Geometry& operator=(const Geometry&);

        GeomBufferPtr geomBuffer_;
        u32 count_;
        s32 materialIndex_;
    };
}


#endif //INC_LRENDER_GEOMETRY_H__
