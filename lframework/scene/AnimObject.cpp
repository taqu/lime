/**
@file AnimObject.cpp
@author t-sakai
@date 2010/04/02 create
*/
#include "AnimObject.h"

#include "Geometry.h"
#include "Material.h"
#include "ShaderManager.h"
#include "Scene.h"
#include "ShaderBase.h"
#include "../render/Batch.h"

#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>

#include "../anim/SkeletonPose.h"
#include "../System.h"
#include "../render/RenderingSystem.h"

namespace lscene
{
    AnimObject::AnimObject()
        :skeletonPose_(NULL)
        ,numGeometries_(0)
        ,geometries_(NULL)
        ,numMaterials_(0)
        ,materials_(NULL)
        ,batches_(NULL)
    {
    }


    AnimObject::AnimObject(
        u32 numGeometries,
        u32 numMaterials)
        :skeletonPose_(NULL)
        ,numGeometries_(numGeometries)
        ,numMaterials_(numMaterials)
    {
        u32 geomSize = sizeof(Geometry);
        u32 matSize = sizeof(Material);
        u32 batchSize = sizeof(lrender::Batch);

        u32 bufferSize = numGeometries * geomSize + numMaterials * matSize + numGeometries * batchSize;
        resourceBuffer_.reset(bufferSize);

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

    AnimObject::~AnimObject()
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
    void AnimObject::initializeShader()
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

#if 0
    void AnimObject::draw(const Scene& scene)
    {
#if defined(LIME_DX9)
        GraphicsDeviceRef& device = Graphics::getDevice();

        PrimitiveType type = geomBuffer_->getType();

        ShaderVSBase* prevVS = NULL;
        // ソフトウェアスキニング
        if(softSkinning_){
            softSkinning_->update(reinterpret_cast<lmath::Matrix43*>(bones_), geomBuffer_->getVertexBuffer());
        }

        // 頂点バッファ・インテックスバッファ・頂点宣言は、
        // AnimObjectのGeometry全てでひとつを共有している
        geomBuffer_->attach();

        for(u32 i=0; i<getNumGeometries(); ++i){
            Geometry &geom = getGeometry(i);
            Material &material = getMaterial( geom.getMaterialIndex() );

            ShaderVSBase* vs = geom.getShader();

            vs->setParameters(matrix_, material, scene);

            if(prevVS != vs){
                if(softSkinning_ == NULL){
                    vs->setSkinningParameters(geom, bones_, numBones_);
                }
                vs->attach();
                prevVS = vs;
            }

            ShaderPSBase* ps = material.getShader();
            ps->setParameters(matrix_, material, scene);
            ps->attach();

            material.applyRenderState();

            u32 vnum = geomBuffer_->getVertexBuffer().getVertexNum();
            if(geomBuffer_->hasIndex()){
                device.drawIndexed(type, vnum, geom.getPrimitiveCount(), geom.getFaceOffset());
            }else{
                device.draw(type, geom.getPrimitiveCount());
            }
		}

        geomBuffer_->detach();

#elif defined(LIME_GLES1)
		// 頂点バッファ・インテックスバッファ・頂点宣言は、
        // AnimObjectのGeometry全てでひとつを共有している
        //geomBuffer_->attach();

        GraphicsDeviceRef& device = Graphics::getDevice();

        PrimitiveType type = geomBuffer_->getType();

        // ソフトウェアスキニング
        if(softSkinning_){
            softSkinning_->update(reinterpret_cast<lmath::Matrix43*>(bones_), geomBuffer_->getVertexBuffer());
        }
        ShaderVSBase* prevVS = NULL;
        geomBuffer_->attach();
        for(u32 i=0; i<getNumGeometries(); ++i){

            Geometry &geom = getGeometry(i);
            Material &material = getMaterial( geom.getMaterialIndex() );

            //テクスチャ
            //TODO:マルチテクスチャ
            if(material.getTextureNum() > 0){
                const SamplerState& state = material.getSamplerState(0);
                state.apply(0);
                const TextureRef& texture = material.getTexture(0);
                texture.attach(0);
            }

            // glTextureCoordPointerはglClientActivateTexture後で
            // glVertexPointer等は、頂点・インデックスバッファのバインド後でないといけない
            geomBuffer_->attachDeclaration(); //無駄が多いので再考の必要あり

            material.applyRenderState();

            ShaderVSBase* vs = geom.getShader();

            if(prevVS != vs){
                vs->setParameters(matrix_, material, scene);

                vs->attach();
                prevVS = vs;
            }

            ShaderPSBase* ps = material.getShader();
            ps->setParameters(matrix_, material, scene);
            ps->attach();

            

            // TODO: TriangleList以外の場合はどうするか
            u32 num = geom.getPrimitiveCount() * 3;
            if(geomBuffer_->hasIndex()){
                u32 offset = geom.getFaceOffset() * sizeof(u16);
                device.drawIndexed(type, num, offset);
            }else{
                u32 offset = geom.getFaceOffset();
                device.draw(type, num, offset);
            }

            geomBuffer_->detachDeclaration(); //無駄が多いので再考の必要あり
		}
        geomBuffer_->detach();
#endif

    }
#endif

    Geometry& AnimObject::getGeometry(u32 index)
    {
        LASSERT(0<= index && index<numGeometries_);
        return geometries_[index];
    }

    Material& AnimObject::getMaterial(s32 index)
    {
        LASSERT(0<= static_cast<u32>(index) && static_cast<u32>(index)<numMaterials_);
        return materials_[index];
    }

    void AnimObject::swap(AnimObject& rhs)
    {
        skeleton_.swap(rhs.skeleton_);
        lcore::swap(skeletonPose_, rhs.skeletonPose_);
        lcore::swap(numGeometries_, rhs.numGeometries_);
        lcore::swap(geometries_, rhs.geometries_);
        lcore::swap(numMaterials_, rhs.numMaterials_);
        lcore::swap(materials_, rhs.materials_);
        lcore::swap(batches_, rhs.batches_);

        resourceBuffer_.swap(rhs.resourceBuffer_);

    }

    void AnimObject::addDraw()
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

    void AnimObject::removeDraw()
    {
        lrender::RenderingSystem& renderSystem = lframework::System::getRenderSys();

        for(u32 i=0; i<numGeometries_; ++i){
            if(batches_[i].getDrawable() != NULL){
                renderSystem.remove(lrender::Pass_Main, &batches_[i]);
                batches_[i].setDrawable(NULL);
            }
        }
    }


    u32 AnimObject::getNumJointPoses() const
    {
        return (skeletonPose_ != NULL)? skeletonPose_->getNumJoints() : 0;
    }

    const lmath::Matrix43* AnimObject::getGlobalJointPoses() const
    {
        LASSERT(skeletonPose_ != NULL);
        return skeletonPose_->getMatrices();
    }
}
