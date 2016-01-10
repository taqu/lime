/**
@file NodeObjectMotion.cpp
@author t-sakai
@date 2015/01/26 create
*/
#include "NodeObjectMotion.h"
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
    NodeObjectMotion::NodeObjectMotion(const Char* name, u16 group, u16 type)
        :NodeObject(name, group, type)
    {
        prevMatrix_.identity();
    }

    NodeObjectMotion::~NodeObjectMotion()
    {
    }

    void NodeObjectMotion::update()
    {
        pushMatrix();
        NodeTransform::updateTransform();
    }

    void NodeObjectMotion::pushMatrix()
    {
        prevMatrix_ = matrix_;
    }

    void NodeObjectMotion::visitRenderQueue(lscene::RenderContext& renderContext)
    {
        if(!checkFlag(NodeFlag_Render)){
            return;
        }

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

            queue.add(Path_MotionVelocity, this);
        }
        visitRenderQueueChildren(renderContext);
    }

    void NodeObjectMotion::render(lscene::RenderContext& renderContext)
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
        case Path_MotionVelocity:
            renderMotionVelocity(renderContext);
            break;
        };
    }

    void NodeObjectMotion::renderMotionVelocity(lscene::RenderContext& renderContext)
    {
        lgraphics::ContextRef& context = renderContext.getContext();

        lrender::ShaderManager& shaderManager = lrender::ShaderManager::getInstance();

        lgraphics::VertexShaderRef* motionVelocityVS = shaderManager.getVS(lscene::lrender::ShaderVS_NormalMotion);
        lgraphics::VertexShaderRef* motionVelocityNoUVVS = shaderManager.getVS(lscene::lrender::ShaderVS_NormalNoUVMotion);

        NodeMotionVelocityConstant nodeConstant;

        lmath::Matrix44 local;
        for(u32 i=0; i<object_->getNumNodes(); ++i){
            lrender::Node& node = object_->getNode(i);

            node.calcLocalMatrix(local);
            nodeConstant.w1_ = node.world_;

            if(lrender::InvalidNodeIndex != node.parentIndex_){
                lrender::Node& parent = object_->getNode(node.parentIndex_);
                node.world_.mul(parent.world_, local);

            } else{
                node.world_.mul(prevMatrix_, local);
            }

            if(node.numMeshes_<=0){
                continue;
            }

            nodeConstant.w0_ = node.world_;
            renderContext.setConstant(lscene::RenderContext::Shader_VS, 0, sizeof(NodeMotionVelocityConstant), &nodeConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                lrender::Mesh& mesh = node.meshes_[j];
                lrender::Geometry& geometry = *(mesh.getGeometry());

                lgraphics::VertexShaderRef* vs = (mesh.hasUV())? motionVelocityVS : motionVelocityNoUVVS;
                vs->attach(context);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }
}
