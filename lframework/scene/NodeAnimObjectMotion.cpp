/**
@file NodeAnimObjectMotion.cpp
@author t-sakai
@date 2015/01/26 create
*/
#include "NodeAnimObjectMotion.h"
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
    NodeAnimObjectMotion::NodeAnimObjectMotion(const Char* name, u16 group, u16 type)
        :NodeAnimObject(name, group, type)
        ,prevSkinningMatrices_(NULL)
    {
        prevMatrix_.identity();
    }

    NodeAnimObjectMotion::~NodeAnimObjectMotion()
    {
        releaseMatrices();
    }

    void NodeAnimObjectMotion::update()
    {
        pushMatrix();
        NodeAnimObject::update();
    }

    void NodeAnimObjectMotion::pushMatrix()
    {
        prevMatrix_ = matrix_;
        lcore::swap(skinningMatrices_, prevSkinningMatrices_);
    }

    void NodeAnimObjectMotion::copyMatrices()
    {
        lscene::copyAlign16Size16Times(prevSkinningMatrices_, skinningMatrices_, sizeof(lmath::Matrix34)*numSkinningMatrices_);
    }

    void NodeAnimObjectMotion::visitRenderQueue(lscene::RenderContext& renderContext)
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

            if(checkFlag(NodeFlag_Update)){
                queue.add(Path_MotionVelocity, this);
            }
        }

        visitRenderQueueChildren(renderContext);
    }

    void NodeAnimObjectMotion::render(lscene::RenderContext& renderContext)
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

    void NodeAnimObjectMotion::renderMotionVelocity(lscene::RenderContext& renderContext)
    {
        setSkinningMatricesTwoFrames(renderContext);

        lgraphics::ContextRef& context = renderContext.getContext();

        lrender::ShaderManager& shaderManager = lrender::ShaderManager::getInstance();

        lgraphics::VertexShaderRef* motionVelocityVS = shaderManager.getVS(lscene::lrender::ShaderVS_NormalSkinningMotion);
        motionVelocityVS->attach(context);

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

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    inline void NodeAnimObjectMotion::setSkinningMatricesTwoFrames(lscene::RenderContext& renderContext)
    {
        renderContext.setConstantMatricesAligned16(lscene::RenderContext::Shader_VS, 1, numSkinningMatrices_, prevSkinningMatrices_);
        renderContext.setConstantMatricesAligned16(lscene::RenderContext::Shader_VS, 2, numSkinningMatrices_, skinningMatrices_);
    }

    void NodeAnimObjectMotion::createMatrices(s32 num)
    {
        numSkinningMatrices_ = num;
        skinningMatrices_ = (lmath::Matrix34*)LSCENE_ALIGNED_MALLOC(sizeof(lmath::Matrix34)*numSkinningMatrices_*2, MemoryAlign);
        prevSkinningMatrices_ = skinningMatrices_ + numSkinningMatrices_;
    }

    void NodeAnimObjectMotion::releaseMatrices()
    {
        if(prevSkinningMatrices_<skinningMatrices_){
            LSCENE_ALIGNED_FREE(prevSkinningMatrices_, MemoryAlign);
            skinningMatrices_ = NULL;
        } else{
            LSCENE_ALIGNED_FREE(skinningMatrices_, MemoryAlign);
            prevSkinningMatrices_ = NULL;
        }
    }
}
