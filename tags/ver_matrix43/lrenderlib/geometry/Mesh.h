#ifndef INC_LRENDER_MESH_H__
#define INC_LRENDER_MESH_H__
/**
@file Mesh.h
@author t-sakai
@date 2009/12/21 create
*/
#include "../lrendercore.h"
#include "Shape.h"

namespace lgraphics
{
    class Material;
}

namespace lrender
{
    class Geometry;
    struct HitRecord;
    class ShadingGeometry;
    
    //----------------------------------------------------------
    //---
    //--- Mesh
    //---
    //----------------------------------------------------------
    class Mesh : public Shape
    {
    public:
        Mesh(
            u32 numGeometries,
            u32 numMaterials);

        virtual ~Mesh();

        u32 getNumGeometries() const{ return numGeometries_;}
        Geometry& getGeometry(u32 index);

        u32 getNumMaterials() const{ return numMaterials_;}
        lgraphics::Material& getMaterial(u32 index);

        // Shapeのメソッド実装
        //---------------------------------------------------------------------------
        virtual u32 calcNumElements() const;
        virtual bool getElements(Element* elements, u32 bufferCount);

        virtual void calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const;
        virtual void calcMedian(u32 index, Vector3& median) const;

        virtual bool hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const;
        virtual void calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const;
    private:
        u32 numGeometries_;
        Geometry *geometries_;

        u32 numMaterials_;
        lgraphics::Material *materials_;
    };
}

#endif //INC_LRENDER_MESH_H__
