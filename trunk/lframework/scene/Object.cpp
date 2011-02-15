/**
@file Object.cpp
@author t-sakai
@date 2011/02/16
*/
#include "Object.h"

#include "Geometry.h"
#include "Material.h"
#include "ShaderManager.h"
#include "Scene.h"
#include "ShaderBase.h"
#include "../render/Batch.h"

#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>

#include "../System.h"
#include "../render/RenderingSystem.h"

namespace lscene
{
    Object::Object()
        :numGeometries_(0)
        ,geometries_(NULL)
        ,numMaterials_(0)
        ,materials_(NULL)
        ,batches_(NULL)
    {
    }


    Object::Object(
        u32 numGeometries,
        u32 numMaterials)
        :numGeometries_(numGeometries)
        ,numMaterials_(numMaterials)
    {
        u32 geomSize = sizeof(Geometry);
        u32 matSize = sizeof(Material);
        u32 batchSize = sizeof(lrender::Batch);

        u32 bufferSize = numGeometries * geomSize + numMaterials * matSize + numGeometries * batchSize;
        resourceBuffer_.resize(bufferSize);

        //---------------------------------------------------
        for(u32 i=0; i<numGeometries_; ++i){
            resourceBuffer_.allocate<Geometry>(i*geomSize);
        }
        geometries_ = resourceBuffer_.get<Geometry>(0);


        //---------------------------------------------------
        u32 matTopOffset = numGeometries_ * geomSize;

        for(u32 i=0; i<numMaterials_; ++i){
            resourceBuffer_.allocate<Material>(matTopOffset + i*matSize);
        }
        materials_ = resourceBuffer_.get<Material>(matTopOffset);

        //---------------------------------------------------
        u32 batchTopOffset = matTopOffset + numMaterials * matSize;

        batches_ = resourceBuffer_.get<lrender::Batch>(batchTopOffset);
        for(u32 i=0; i<numGeometries_; ++i){
            resourceBuffer_.allocate<lrender::Batch>(batchTopOffset + i*batchSize);
        }
    }

    Object::~Object()
    {
        removeDraw();

        for(u32 i=0; i<numGeometries_; ++i){
            lrender::Batch *batch = &batches_[i];
            batch->~Batch();

            Geometry *geom = &geometries_[i];
            geom->~Geometry();
        }
        geometries_ = NULL;

        for(u32 i=0; i<numMaterials_; ++i){
            Material *mat = &materials_[i];
            mat->~Material();
        }
        materials_ = NULL;
    }

    //-----------------------------------------------------------
    void Object::initializeShader()
    {
        lcore::Log("AnimObj init shader");

        ShaderManager *manager = ShaderManager::getInstance();

        ShaderVSBase* shaderVS = NULL;
        ShaderPSBase* shaderPS = NULL;
        for(u32 i=0; i<numGeometries_; ++i){
            Material& material = materials_[ geometries_[i].getMaterialIndex() ];

            manager->create(geometries_[i], material, shaderVS, shaderPS);
            geometries_[i].setShaderVS(shaderVS);
            geometries_[i].setShaderPS(shaderPS);
        }
    }


    Geometry& Object::getGeometry(u32 index)
    {
        LASSERT(0<= index && index<numGeometries_);
        return geometries_[index];
    }

    Material& Object::getMaterial(s32 index)
    {
        LASSERT(0<= static_cast<u32>(index) && static_cast<u32>(index)<numMaterials_);
        return materials_[index];
    }

    void Object::swap(Object& rhs)
    {
        lcore::swap(numGeometries_, rhs.numGeometries_);
        lcore::swap(geometries_, rhs.geometries_);
        lcore::swap(numMaterials_, rhs.numMaterials_);
        lcore::swap(materials_, rhs.materials_);
        lcore::swap(batches_, rhs.batches_);

        resourceBuffer_.swap(rhs.resourceBuffer_);

    }

    void Object::addDraw()
    {
        lrender::RenderingSystem& renderSystem = lframework::System::getRenderSys();

        for(u32 i=0; i<numGeometries_; ++i){
            batches_[i].setGeometry( &geometries_[i] );
            batches_[i].setMaterial( &materials_[ geometries_[i].getMaterialIndex() ] );

            if(batches_[i].getDrawable() == NULL){
                batches_[i].setDrawable(this);
                renderSystem.add(lrender::Pass_Main, &batches_[i]);
            }
        }
    }

    void Object::removeDraw()
    {
        lrender::RenderingSystem& renderSystem = lframework::System::getRenderSys();

        for(u32 i=0; i<numGeometries_; ++i){
            if(batches_[i].getDrawable() != NULL){
                renderSystem.remove(lrender::Pass_Main, &batches_[i]);
                batches_[i].setDrawable(NULL);
            }
        }
    }
}
