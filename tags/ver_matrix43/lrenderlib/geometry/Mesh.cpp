/**
@file Mesh.cpp
@author t-sakai
@date 2009/12/21
*/
#include "Mesh.h"

#include <lgraphics/scene/Material.h>

#include "Face.h"
#include "Geometry.h"
#include "ShadingGeometry.h"
#include "HitRecord.h"

using namespace lgraphics;

namespace lrender
{
    Mesh::Mesh(
        u32 numGeometries,
        u32 numMaterials)
        :numGeometries_(numGeometries)
        ,numMaterials_(numMaterials)
    {
        geometries_ = LIME_NEW Geometry[numGeometries_];
        materials_ = LIME_NEW lgraphics::Material[numMaterials_];
    }

    Mesh::~Mesh()
    {
        LIME_DELETE_ARRAY(materials_);
        LIME_DELETE_ARRAY(geometries_);
    }

    Geometry& Mesh::getGeometry(u32 index)
    {
        LASSERT(0<=index && index<numGeometries_);
        LASSERT(geometries_ != NULL);
        return geometries_[index];
    }

    lgraphics::Material& Mesh::getMaterial(u32 index)
    {
        LASSERT(0<=index && index<numMaterials_);
        LASSERT(materials_ != NULL);
        return materials_[index];
    }


    // Shapeのメソッド実装
    //---------------------------------------------------------------------------
    u32 Mesh::calcNumElements() const
    {
        LASSERT(geometries_ != NULL);
        u32 numElements = 0;
        for(u32 i=0; i<numGeometries_; ++i){
            numElements += geometries_[i].calcNumElements();
        }
        return numElements;
    }

    bool Mesh::getElements(Element* elements, u32 bufferCount)
    {
        u32 numElements = 0;
        for(u32 i=0; i<numGeometries_; ++i){
            geometries_[i].getElements(elements+numElements, bufferCount-numElements);
            numElements += geometries_[i].calcNumElements();
        }
        return true;
    }

    void Mesh::calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const
    {
        if(index<numGeometries_){
            geometries_[index].calcBBox(bmin, bmax);
        }
    }

    void Mesh::calcMedian(u32 index, Vector3& median) const
    {
        if(index<numGeometries_){
            geometries_[index].calcMedian(median);
        }
    }

    bool Mesh::hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const
    {
        LASSERT(false);
        HitRecord tmpHitRecord;

        f32 t = std::numeric_limits<f32>::max();
        bool bHit = false;

        for(u32 i=0; i<numGeometries_; ++i){
            if(geometries_[i].hit(tmpHitRecord, i, ray)){
                if(tmpHitRecord.t_ < t){
                    bHit = true;
                    t = tmpHitRecord.t_;
                    hitRecord = tmpHitRecord;
                }
            }
        }
        return bHit;
    }

    void Mesh::calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const
    {
        LASSERT(false);
    }
}
