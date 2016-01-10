/**
@file NodeAnimObject.cpp
@author t-sakai
@date 2014/12/29 create
*/
#include "NodeAnimObject.h"
#include <lgraphics/api/ShaderRef.h>
#include <lgraphics/api/TextureRef.h>

#include "RenderQueue.h"
#include "RenderContext.h"

#include "./render/AnimObject.h"
#include "./render/Node.h"
#include "./render/Material.h"
#include "./render/Mesh.h"
#include "./render/Geometry.h"
#include "./render/ShaderManager.h"
#include "ShaderConstant.h"
#include "ShadowMap.h"
#include "Scene.h"

namespace lscene
{
    NodeAnimObject::NodeAnimObject(const Char* name, u16 group, u16 type)
        :NodeTransform(name, group, type)
        ,object_(NULL)
        ,numSkinningMatrices_(0)
        ,skinningMatrices_(NULL)
    {
    }

    NodeAnimObject::~NodeAnimObject()
    {
        releaseMatrices();
        LSCENE_RELEASE(object_);
    }

    lrender::AnimObject* NodeAnimObject::getAnimObject()
    {
        return object_;
    }

    void NodeAnimObject::setAnimObject(lrender::AnimObject* obj)
    {
        lscene::move(object_, obj);
        if(object_){
            animation_.setSkeleton(object_->getSkeleton());

            releaseMatrices();
            createMatrices(object_->getSkeleton()->getNumJoints());
        }
        resetAttributes();
        resetShaders();
    }

    void NodeAnimObject::resetAttributes()
    {
        unsetFlag(NodeFlag_RenderAttributes);

        for(u32 i=0; i<object_->getNumMaterials(); ++i){
            lrender::Material& material = object_->getMaterial(i);
            if(material.isCastShadow()){
                setFlag(NodeFlag_CastShadow);
            }

            if(material.isTransparent()){
                setFlag(NodeFlag_Transparent);

            }else{
                setFlag(NodeFlag_Solid);
            }
        }
    }

    void NodeAnimObject::resetShaders()
    {
        for(u32 i=0; i<object_->getNumMeshes(); ++i){
            lrender::ShaderSetFactory::setShaderSet(object_->getMesh(i), lrender::ShaderSetFactoryFlag_Skinning);
        }
    }

    void NodeAnimObject::update()
    {
        NodeTransform::updateTransform();
        animation_.update();
    }

    void NodeAnimObject::visitRenderQueue(lscene::RenderContext& renderContext)
    {
        if(!checkFlag(NodeFlag_Render)){
            return;
        }

        createMatrix(matrix_);
        matrix_.mul(getParentMatrix(), matrix_);
        //bsphere_.setPosition(position_.x_, position_.y_, position_.z_);

        if(NULL == object_){
            return;
        }

        if(checkInWorldFrustum(object_->getSphere(), renderContext)){
            calcDepth(renderContext);

            calcNodeMatrices();
            animation_.getMatricesAligned16(skinningMatrices_);

            RenderQueue& queue = renderContext.getRenderQueue();

            if(checkFlag(NodeFlag_CastShadow)){
                queue.add(Path_Shadow, this);
            }

            if(checkFlag(NodeFlag_Solid)){
                queue.add(Path_Opaque, this);

            } else if(checkFlag(NodeFlag_Transparent)){
                queue.add(Path_Transparent, this);
            }
        }

        visitRenderQueueChildren(renderContext);
    }

    void NodeAnimObject::render(lscene::RenderContext& renderContext)
    {
        switch(renderContext.getPath())
        {
        case Path_Shadow:
            renderDepth(renderContext);
            break;
        case Path_Opaque:
            renderSolid(renderContext);
            break;
        case Path_Transparent:
            renderTransparent(renderContext);
            break;
        };
    }

    void NodeAnimObject::calcNodeMatrices()
    {
        lmath::Matrix44 local;
        u32 i;
        for(i=0; i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);
            if(lrender::InvalidNodeIndex != node.parentIndex_){
                break;
            }

            node.calcLocalMatrix(local);
            node.world_.mul(matrix_, local);
        }

        for(;i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);
            lrender::Node& parent = object_->getNode( node.parentIndex_ );

            node.calcLocalMatrix(local);
            node.world_.mul(parent.world_, local);
        }
    }

    void NodeAnimObject::renderDepth(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();
        const lscene::ShadowMap& shadowMap = renderContext.getShadowMap();

        renderContext.attachDepthShader(RenderContext::DepthShader_Skinning);

        setSkinningMatrices(renderContext);

        NodeConstant constant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }

            constant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(NodeConstant), &constant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lrender::Mesh& mesh = node.meshes_[j];
                lrender::Geometry& geometry = *(mesh.getGeometry());
                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexedInstanced(mesh.getNumIndices(), shadowMap.getCascadeLevels(), mesh.getIndexOffset(), 0, 0);
            }
        }
    }

    void NodeAnimObject::renderSolid(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();

        setSkinningMatrices(renderContext);

        NodeConstant nodeConstant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            nodeConstant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(NodeConstant), &nodeConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lrender::Mesh& mesh = node.meshes_[j];
                lrender::Geometry& geometry = *(mesh.getGeometry());
                lrender::Material& material = *(mesh.getMaterial());

                if(material.isTransparent()){
                    continue;
                }
                lrender::ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_->attach(context);
                shaderSet->ps_->attach(context);

                MaterialConstant* materialConstant = reinterpret_cast<MaterialConstant*>(&material.diffuse_);
                renderContext.setConstant(lscene::RenderContext::Shader_PS, 0, sizeof(MaterialConstant), materialConstant);

                setTexture(context, material);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    void NodeAnimObject::renderTransparent(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();

        setSkinningMatrices(renderContext);

        NodeConstant nodeConstant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            nodeConstant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(NodeConstant), &nodeConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lrender::Mesh& mesh = node.meshes_[j];
                lrender::Geometry& geometry = *(mesh.getGeometry());
                lrender::Material& material = *(mesh.getMaterial());

                if(material.checkFlag(lrender::Material::Flag_Transparent)){
                    renderContext.setBlendState(lgraphics::ContextRef::BlendState_Alpha);

                }else if(material.checkFlag(lrender::Material::Flag_TransparentAdd)){
                    renderContext.setBlendState(lgraphics::ContextRef::BlendState_AlphaAdditive);
                }else{
                    continue;
                }
                lrender::ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_->attach(context);
                shaderSet->ps_->attach(context);

                MaterialConstant* materialConstant = reinterpret_cast<MaterialConstant*>(&material.diffuse_);
                renderContext.setConstant(lscene::RenderContext::Shader_PS, 0, sizeof(MaterialConstant), materialConstant);

                setTexture(context, material);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    inline void NodeAnimObject::setTexture(lgraphics::ContextRef& context, lrender::Material& material)
    {
        if(material.hasTexture(lrender::Tex_Albedo)){
            lrender::Texture2D& tex = object_->getTexture( material.textureIDs_[lrender::Tex_Albedo] );
            context.setPSResources(lrender::Tex_Albedo, 1, (tex.srv_));
            context.setPSSamplerStates(lrender::Tex_Albedo, 1, (tex.sampler_));
        }
        if(material.hasTexture(lrender::Tex_Normal)){
            lrender::Texture2D& tex = object_->getTexture( material.textureIDs_[lrender::Tex_Normal] );
            context.setPSResources(lrender::Tex_Normal, 1, (tex.srv_));
            context.setPSSamplerStates(lrender::Tex_Normal, 1, (tex.sampler_));
        }
    }

    inline void NodeAnimObject::setSkinningMatrices(lscene::RenderContext& renderContext)
    {
        renderContext.setConstantMatricesAligned16(lscene::RenderContext::Shader_VS, 1, numSkinningMatrices_, skinningMatrices_);
    }

    void NodeAnimObject::createMatrices(s32 num)
    {
        numSkinningMatrices_ = num;
        skinningMatrices_ = (lmath::Matrix34*)LSCENE_ALIGNED_MALLOC(sizeof(lmath::Matrix34)*numSkinningMatrices_, MemoryAlign);
    }

    void NodeAnimObject::releaseMatrices()
    {
        LSCENE_ALIGNED_FREE(skinningMatrices_, MemoryAlign);
    }
}
