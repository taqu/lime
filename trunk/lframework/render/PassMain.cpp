/**
@file PassMain.cpp
@author t-sakai
@date 2010/11/22 create

*/
#include "PassMain.h"

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/GeometryBuffer.h>
#include "../scene/Geometry.h"
#include "../scene/Material.h"
#include "../scene/ShaderBase.h"

#include "Batch.h"
#include "Drawable.h"

namespace lrender
{
    PassMain::PassMain()
    {
    }

    PassMain::~PassMain()
    {
    }

    void PassMain::add(Batch* batch)
    {
        batches_.push_back(batch);
    }

    void PassMain::remove(Batch* batch)
    {
        // とりあえず線形探索
        for(u32 i=0; i<batches_.size(); ++i){
            if(batch == batches_[i]){
                lcore::swap(batches_[i], batches_[batches_.size()-1]);
                batches_.pop_back();
                break;
            }
        }
    }

    void PassMain::draw()
    {
        if(batches_.size()<=0){
            return;
        }

        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

        lgraphics::GeometryBuffer* geomBuffer = batches_[0]->getGeometry()->getGeometryBuffer().get();
        lscene::Geometry* geometry = NULL;
        lscene::Material* material = NULL;

        //lcore::Log("pass main %d batches", batches_.size());

        geomBuffer->attach();
        for(u32 i=0; i<batches_.size(); ++i){
            geometry = batches_[i]->getGeometry();


            lgraphics::GeometryBuffer* currentGeom = geometry->getGeometryBuffer().get();
            geomBuffer = currentGeom;
            geomBuffer->attach();
            if(currentGeom != geomBuffer){
                geomBuffer->detach();
                geomBuffer = currentGeom;
                geomBuffer->attach();
            }

            lscene::Material* currentMat = batches_[i]->getMaterial();
            if(currentMat != material){
                material = currentMat;
                material->applyRenderState();
            }

            lscene::ShaderVSBase* vs = geometry->getShaderVS();

#if defined(LIME_GLES2)
            vs->attach();
#endif
            vs->setParameters(*(batches_[i]), scene_);

            lscene::ShaderPSBase* ps = geometry->getShaderPS();
            ps->setParameters(*(batches_[i]), scene_);

            lgraphics::PrimitiveType type = geomBuffer->getType();

#if defined(LIME_GLES2)
            u32 offset = geometry->getIndexOffset();
            
            if(geomBuffer->hasIndex()){
                u32 num = geometry->getNumIndices();
                device.drawIndexed(type, num, offset);
            }else{
                u32 num = geometry->getPrimitiveCount() * 3;
                device.draw(type, num, offset);
            }
#else
            vs->attach();
            ps->attach();
            u32 vnum = geomBuffer->getVertexBuffer().getVertexNum();
            if(geomBuffer->hasIndex()){
                device.drawIndexed(type, vnum, geometry->getPrimitiveCount(), geometry->getIndexOffset());
            }else{
                device.draw(type, geometry->getPrimitiveCount(), geometry->getIndexOffset());
            }
#endif
        }
        geomBuffer->detach();

    }
}
