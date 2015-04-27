/**
@file NodeObject.cpp
@author t-sakai
@date 2014/11/16 create
*/
#include "NodeObject.h"
#include <lgraphics/api/ShaderRef.h>
#include <lgraphics/api/TextureRef.h>

#include "RenderQueue.h"
#include "RenderContext.h"

#include "./render/Object.h"
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
    NodeObject::NodeObject(const Char* name)
        :NodeTransform(name)
        ,object_(NULL)
    {
    }

    NodeObject::~NodeObject()
    {
        LSCENE_RELEASE(object_);
    }

    s32 NodeObject::getType() const
    {
        return NodeType_Object;
    }

    lrender::Object* NodeObject::getObject()
    {
        return object_;
    }

    void NodeObject::setObject(lrender::Object* obj)
    {
        lscene::move(object_, obj);
        resetAttributes();
        resetShaders();
    }

    void NodeObject::resetAttributes()
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

    void NodeObject::resetShaders()
    {
        for(u32 i=0; i<object_->getNumMeshes(); ++i){
            lrender::ShaderSetFactory::setShaderSet(object_->getMesh(i), lrender::ShaderSetFactoryFlag_None);
        }
    }

    void NodeObject::visitRenderQueue(lscene::RenderContext& renderContext)
    {
        if(!checkFlag(NodeFlag_Render)){
            return;
        }

        createMatrix(matrix_);
        matrix_.mul(parent_->getMatrix(), matrix_);

        if(NULL == object_){
            return;
        }

        if(checkInWorldFrustum(object_->getSphere(), renderContext)){
            calcDepth(renderContext);
            calcNodeMatrices();

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

    void NodeObject::render(lscene::RenderContext& renderContext)
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

    void NodeObject::calcNodeMatrices()
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

    void NodeObject::renderDepth(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();
        const lscene::ShadowMap& shadowMap = renderContext.getShadowMap();

        renderContext.attachDepthShader(lscene::RenderContext::DepthShader_Normal);

        lscene::NodeConstant constant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lscene::lrender::Node& node = object_->getNode(i);

            if(node.numMeshes_<=0){
                continue;
            }
            constant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(lscene::NodeConstant), &constant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lscene::lrender::Mesh& mesh = node.meshes_[j];
                lscene::lrender::Geometry& geometry = *(mesh.getGeometry());
                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexedInstanced(mesh.getNumIndices(), shadowMap.getCascadeLevels(), mesh.getIndexOffset(), 0, 0);
            }
        }
    }

    void NodeObject::renderSolid(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();

        lscene::NodeConstant nodeConstant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lscene::lrender::Node& node = object_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            nodeConstant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(lscene::NodeConstant), &nodeConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lscene::lrender::Mesh& mesh = node.meshes_[j];
                lscene::lrender::Geometry& geometry = *(mesh.getGeometry());
                lscene::lrender::Material& material = *(mesh.getMaterial());

                if(material.isTransparent()){
                    continue;
                }
                lscene::lrender::ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_->attach(context);
                shaderSet->ps_->attach(context);

                lscene::MaterialConstant* materialConstant = reinterpret_cast<lscene::MaterialConstant*>(&material.diffuse_);
                renderContext.setConstant(lscene::RenderContext::Shader_PS, 0, sizeof(lscene::MaterialConstant), materialConstant);

                setTexture(context, material);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    void NodeObject::renderTransparent(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();

        lscene::NodeConstant nodeConstant;

        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lscene::lrender::Node& node = object_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }

            nodeConstant.w_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(lscene::NodeConstant), &nodeConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lscene::lrender::Mesh& mesh = node.meshes_[j];
                lscene::lrender::Geometry& geometry = *(mesh.getGeometry());
                lscene::lrender::Material& material = *(mesh.getMaterial());

                if(material.checkFlag(lscene::lrender::Material::Flag_Transparent)){
                    renderContext.setBlendState(lgraphics::ContextRef::BlendState_Alpha);

                } else if(material.checkFlag(lscene::lrender::Material::Flag_TransparentAdd)){
                    renderContext.setBlendState(lgraphics::ContextRef::BlendState_AlphaAdditive);
                }else{
                    continue;
                }
                lscene::lrender::ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_->attach(context);
                shaderSet->ps_->attach(context);

                lscene::MaterialConstant* materialConstant = reinterpret_cast<lscene::MaterialConstant*>(&material.diffuse_);
                renderContext.setConstant(lscene::RenderContext::Shader_PS, 0, sizeof(lscene::MaterialConstant), materialConstant);

                setTexture(context, material);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    inline void NodeObject::setTexture(lgraphics::ContextRef& context, lrender::Material& material)
    {
        if(material.hasTexture(lrender::Tex_Albedo)){
            lgraphics::Texture2DRef& tex = object_->getTexture( material.textureIDs_[lrender::Tex_Albedo] );
            tex.attachPS(context, lrender::Tex_Albedo, lrender::Tex_Albedo);
        }
        if(material.hasTexture(lrender::Tex_Normal)){
            lgraphics::Texture2DRef& tex = object_->getTexture( material.textureIDs_[lrender::Tex_Normal] );
            tex.attachPS(context, lrender::Tex_Normal, lrender::Tex_Normal);
        }
    }
}
