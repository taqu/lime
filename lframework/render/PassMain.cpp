/**
@file PassMain.cpp
@author t-sakai
@date 2010/11/22 create

*/
#include "PassMain.h"

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/RenderStateRef.h>
#include "../scene/Geometry.h"
#include "../scene/Material.h"
#include "../scene/ShaderBase.h"

#include "Batch.h"
#include "Drawable.h"

namespace lrender
{
    PassMain::PassMain()
        :drawablesChanged_(false)
    {
    }

    PassMain::~PassMain()
    {
    }

    void PassMain::add(Drawable* drawable)
    {
        LASSERT(drawable != NULL);
        //セットされているDrawableは重複は無いと仮定
#if defined(_DEBUG)
        for(Drawable* itr = drawableList_.begin();
            itr != drawableList_.end();
            itr = drawableList_.next(itr))
        {
            if(itr == drawable){
                LASSERT(false);
                return;
            }
        }
#endif
        drawableList_.push_back(drawable);
        drawablesChanged_ = true;
    }

    void PassMain::remove(Drawable* drawable)
    {
        LASSERT(drawable != NULL);

        // とりあえず線形探索
        for(Drawable* itr = drawableList_.begin();
            itr != drawableList_.end();
            itr = drawableList_.next(itr))
        {
            if(itr == drawable){
                drawableList_.erase(drawable);
                break;
            }
        }

        drawablesChanged_ = true;
    }

    inline void PassMain::drawBatch(Batch& batch, lgraphics::GeometryBuffer*& geomBuffer, lscene::Geometry*& geometry)
    {
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

        lscene::ShaderVSBase* vs = geometry->getShaderVS();

#if defined(LIME_GLES2)
        vs->attach();
#endif
        vs->setParameters(batch, scene_);

        lscene::ShaderPSBase* ps = geometry->getShaderPS();
        ps->setParameters(batch, scene_);

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

    void PassMain::draw()
    {
        if(drawablesChanged_){
            sortBatches();
        }

        lgraphics::GeometryBuffer* geomBuffer = NULL;
        lscene::Geometry* geometry = NULL;
        lscene::Material* material = NULL;

        //アルファブレンド無効
        lgraphics::RenderState::setAlphaBlendEnable(false);

        for(BatchVector::iterator itr = batches_.begin();
            itr != batches_.end();
            ++itr)
        {
            Batch& batch = **itr;
            geometry = batch.getGeometry();


            lgraphics::GeometryBuffer* currentGeom = geometry->getGeometryBuffer().get();

            if(currentGeom != geomBuffer){
                geomBuffer = currentGeom;
                geomBuffer->attach();
            }

            lscene::Material* currentMat = batch.getMaterial();
            if(currentMat != material){
                material = currentMat;
                material->applyRenderState();
            }

            drawBatch(batch, geomBuffer, geometry);
        }

        //アルファブレンド有効
        lgraphics::RenderState::setAlphaBlendEnable(true);
        geomBuffer = NULL;
        material = NULL;

        for(BatchVector::iterator itr = alphaBatches_.begin();
            itr != alphaBatches_.end();
            ++itr)
        {
            Batch& batch = **itr;
            geometry = batch.getGeometry();


            lgraphics::GeometryBuffer* currentGeom = geometry->getGeometryBuffer().get();

            if(currentGeom != geomBuffer){
                geomBuffer = currentGeom;
                geomBuffer->attach();
            }

            lscene::Material* currentMat = batch.getMaterial();
            if(currentMat != material){
                material = currentMat;
                material->applyRenderState();
            }

            drawBatch(batch, geomBuffer, geometry);
        }

        if(geomBuffer){
            geomBuffer->detach();
        }
#if defined(LIME_GLES2)
        glDepthMask( GL_TRUE ); //元に戻さないとglClearも効かない
#endif
    }


    void PassMain::sortBatches()
    {
        batches_.clear();
        alphaBatches_.clear();

        // とりあえずアルファブレンドとそれ以外を分けるだけ
        for(Drawable* itr = drawableList_.begin();
            itr != drawableList_.end();
            itr = drawableList_.next(itr))
        {
            Drawable& drawable = *itr;

            for(u32 j=0; j<drawable.getNumGeometries(); ++j){
                Batch& batch = drawable.getBatch(j);
                const lscene::Material* material = batch.getMaterial();

#if 0
                batches_.push_back(&batch);
#else
                if(material->getRenderState().getAlphaBlendEnable()){
                    alphaBatches_.push_back(&batch);
                }else{
                    batches_.push_back(&batch);
                }
#endif
            }
        }// for(Drawable

        drawablesChanged_ = false;
    }


}
