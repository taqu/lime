/**
@file ComponentSkinningRenderer.cpp
@author t-sakai
@date 2016/10/18 create
*/
#include "ecs/ComponentSkinningRenderer.h"
#include "System.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentGeometric.h"
#include "resource/Resources.h"
#include "render/RenderQueue.h"
#include "render/RenderContext.h"
#include "render/Camera.h"
#include "render/ShadowMap.h"
#include "render/ShaderConstant.h"
#include "render/ShaderSet.h"

namespace lfw
{
    namespace
    {
        inline ComponentRendererManager* getManager()
        {
            return System::getECSManager().getComponentManager<ComponentRendererManager>();
        }
    }

    ComponentSkinningRenderer::ComponentSkinningRenderer()
        :numSkinningMatrices_(0)
        ,skinningMatrices_(NULL)
        ,prevSkinningMatrices_(NULL)
    {
        prevMatrix_.identity();
    }

    ComponentSkinningRenderer::~ComponentSkinningRenderer()
    {
        releaseMatrices();
    }

    void ComponentSkinningRenderer::onCreate()
    {
    }

    void ComponentSkinningRenderer::onStart()
    {
    }

    void ComponentSkinningRenderer::update()
    {
    }

    void ComponentSkinningRenderer::postUpdate()
    {
        animation_.update();

        const ComponentGeometric* geometric = getEntity().getGeometric();
        prevMatrix_ = geometric->getMatrix();
        if(NULL == model_){
            return;
        }
        calcNodeMatrices();
        lcore::swap(skinningMatrices_, prevSkinningMatrices_);
    }

    void ComponentSkinningRenderer::onDestroy()
    {
    }

    bool ComponentSkinningRenderer::addQueue(RenderQueue& queue)
    {
        if(NULL == model_){
            return false;
        }
        const lmath::Vector4& position = getEntity().getGeometric()->getPosition();
        const lmath::Vector3& scale = getEntity().getGeometric()->getScale();

        f32 depth = lmath::manhattanDistance3(queue.getCamera().getEyePosition(), position);
        lmath::Sphere sphere = model_->getSphere();
        sphere.translate(position);
        sphere.radius() *= lmath::maximum(scale);

        if(checkFlag(Flag_ShadowCast)){
            if(queue.getShadowMap().contains(sphere)){
                queue.add(RenderPath_Shadow, depth, this);
            }
        }

        if(queue.getWorldFrustum().contains(sphere)){
            if(checkFlag(Flag_Opaque)){
                queue.add(RenderPath_Opaque, depth, this);
            }
            if(checkFlag(Flag_Transparent)){
                queue.add(RenderPath_Transparent, depth, this);
            }
        }
        return true;
    }

    void ComponentSkinningRenderer::drawDepth(RenderContext& renderContext)
    {
        lgfx::ContextRef& context = renderContext.getContext();
        const ShadowMap& shadowMap = renderContext.getShadowMap();

        renderContext.attachDepthShader(RenderContext::DepthShader_Normal);

        PerModelConstant0 modelConstant0;
        for(s32 i=0; i<model_->getNumNodes(); ++i){
            Node& node = model_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            modelConstant0.world0_ = node.world0_;
            modelConstant0.world1_ = node.world1_;

            renderContext.setConstant(RenderContext::Shader_VS, 4, sizeof(PerModelConstant0), &modelConstant0);

            for(s32 j=0; j<node.numMeshes_; ++j){
                Mesh& mesh = node.meshes_[j];
                Geometry& geometry = *(mesh.getGeometry());
                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexedInstanced(mesh.getNumIndices(), shadowMap.getCascadeLevels(), mesh.getIndexOffset(), 0, 0);
            }
        }
    }

    void ComponentSkinningRenderer::drawGBuffer(RenderContext& renderContext)
    {
        lgfx::ContextRef& context = renderContext.getContext();

        renderContext.attachDepthShader(RenderContext::DepthShader_Normal);

        PerModelConstant0 modelConstant;
        for(s32 i=0; i<model_->getNumNodes(); ++i){
            Node& node = model_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            modelConstant.world0_ = node.world0_;
            modelConstant.world1_ = node.world1_;

            renderContext.setConstant(RenderContext::Shader_VS, 4, sizeof(PerModelConstant0), &modelConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                Mesh& mesh = node.meshes_[j];
                Geometry& geometry = *(mesh.getGeometry());
                Material* material = mesh.getMaterial();
                if(material->isTransparent()){
                    continue;
                }

                ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_.attach(context);
                shaderSet->ds_.attach(context);
                shaderSet->hs_.attach(context);
                shaderSet->gs_.attach(context);
                shaderSet->ps_.attach(context);

                MaterialConstant* materialConstant = reinterpret_cast<MaterialConstant*>(&material->diffuse_);
                renderContext.setConstant(RenderContext::Shader_PS, 4, sizeof(MaterialConstant), materialConstant);

                lgfx::ShaderResourceView srvs(context);
                for(s32 k=0; k<TextureType_Used; ++k){
                    if(material->hasTexture(k)){
                        Texture2D& tex = model_->getTexture(material->textureIDs_[k]);
                        srvs.add(tex.srv_);
                    } else{
                        srvs.add(NULL);
                    }
                }
                srvs.setPS(0);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    void ComponentSkinningRenderer::drawOpaque(RenderContext& renderContext)
    {
        lgfx::ContextRef& context = renderContext.getContext();

        renderContext.attachDepthShader(RenderContext::DepthShader_Normal);

        PerModelConstant0 modelConstant;
        for(s32 i=0; i<model_->getNumNodes(); ++i){
            Node& node = model_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            modelConstant.world0_ = node.world0_;
            modelConstant.world1_ = node.world1_;

            renderContext.setConstant(RenderContext::Shader_VS, 4, sizeof(PerModelConstant0), &modelConstant);

            for(s32 j=0; j<node.numMeshes_; ++j){
                Mesh& mesh = node.meshes_[j];
                Geometry& geometry = *(mesh.getGeometry());
                Material* material = mesh.getMaterial();
                if(material->isTransparent()){
                    continue;
                }

                ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_.attach(context);
                shaderSet->ds_.attach(context);
                shaderSet->hs_.attach(context);
                shaderSet->gs_.attach(context);
                shaderSet->ps_.attach(context);

                MaterialConstant* materialConstant = reinterpret_cast<MaterialConstant*>(&material->diffuse_);
                renderContext.setConstant(RenderContext::Shader_PS, 4, sizeof(MaterialConstant), materialConstant);

                lgfx::ShaderResourceView srvs(context);
                for(s32 k=0; k<TextureType_Used; ++k){
                    if(material->hasTexture(k)){
                        Texture2D& tex = model_->getTexture(material->textureIDs_[k]);
                        srvs.add(tex.srv_);
                    } else{
                        srvs.add(NULL);
                    }
                }
                srvs.setPS(0);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    void ComponentSkinningRenderer::drawTransparent(RenderContext& renderContext)
    {
        lgfx::ContextRef& context = renderContext.getContext();

        renderContext.attachDepthShader(RenderContext::DepthShader_Normal);

        PerModelConstant0 modelConstant0;
        for(s32 i=0; i<model_->getNumNodes(); ++i){
            Node& node = model_->getNode(i);
            if(node.numMeshes_<=0){
                continue;
            }
            modelConstant0.world0_ = node.world0_;
            modelConstant0.world1_ = node.world1_;

            renderContext.setConstant(RenderContext::Shader_VS, 4, sizeof(PerModelConstant0), &modelConstant0);

            for(s32 j=0; j<node.numMeshes_; ++j){
                Mesh& mesh = node.meshes_[j];
                Geometry& geometry = *(mesh.getGeometry());
                Material* material = mesh.getMaterial();
                if(!material->isTransparent()){
                    continue;
                }
                context.setBlendState(material->blendState_.get());

                ShaderSet* shaderSet = mesh.getShaderSet();
                shaderSet->vs_.attach(context);
                shaderSet->ds_.attach(context);
                shaderSet->hs_.attach(context);
                shaderSet->gs_.attach(context);
                shaderSet->ps_.attach(context);

                MaterialConstant* materialConstant = reinterpret_cast<MaterialConstant*>(&material->diffuse_);
                renderContext.setConstant(RenderContext::Shader_PS, 4, sizeof(MaterialConstant), materialConstant);

                lgfx::ShaderResourceView srvs(context);
                for(s32 k=0; k<TextureType_Used; ++k){
                    if(material->hasTexture(k)){
                        Texture2D& tex = model_->getTexture(material->textureIDs_[k]);
                        srvs.add(tex.srv_);
                    } else{
                        srvs.add(NULL);
                    }
                }
                srvs.setPS(0);

                context.setPrimitiveTopology(mesh.getType());
                geometry.attach(context);
                context.drawIndexed(mesh.getNumIndices(), mesh.getIndexOffset(), 0);
            }
        }
    }

    void ComponentSkinningRenderer::getAABB(lmath::lm128& bmin, lmath::lm128& bmax)
    {
        if(NULL != model_){
            const ComponentGeometric* geometric = getEntity().getGeometric();
            const lmath::Sphere& sphere = model_->getSphere();
            const lmath::Vector3& scale = geometric->getScale();
            const lmath::Vector4& translate = geometric->getPosition();

            lmath::lm128 ts = _mm_set_ps(0.0f, scale.z_, scale.y_, scale.x_);
            ts = _mm_mul_ps(ts, _mm_set1_ps(sphere.radius()));
            lmath::lm128 tp = _mm_add_ps(_mm_loadu_ps(&sphere.x_), _mm_loadu_ps(&translate.x_));
            bmin = _mm_sub_ps(tp,ts);
            bmax = _mm_add_ps(tp, ts);
        }else{
            ComponentRenderer::getAABB(bmin, bmax);
        }
    }

    void ComponentSkinningRenderer::setMesh(Model::pointer& model, Skeleton::pointer& skeleton)
    {
        if(skeleton_ != skeleton){
            skeleton_ = skeleton;
            animation_.setSkeleton(skeleton_.get());
            releaseMatrices();
            if(NULL != skeleton_){
                createMatrices(skeleton_->getNumJoints());
            }
        }

        if(model_ != model){
            model_ = model;
            if(NULL != model_){
                resetShaderSet();
                resetFlags();
                calcPrevNodeMatrices();
            } else{
                resetFlag(Flag_ShadowCast);
                resetFlag(Flag_Opaque);
                resetFlag(Flag_Transparent);
            }
        }
    }

    void ComponentSkinningRenderer::pushMatrix()
    {
        const ComponentGeometric* geometric = getEntity().getGeometric();
        prevMatrix_ = geometric->getMatrix();
        if(NULL == model_){
            return;
        }
        calcPrevNodeMatrices();
    }

    void ComponentSkinningRenderer::createMatrices(s32 num)
    {
        numSkinningMatrices_ = num;
        skinningMatrices_ = (lmath::Matrix34*)LALIGNED_MALLOC(sizeof(lmath::Matrix34)*numSkinningMatrices_*2, lcore::SSE_ALIGN);
        prevSkinningMatrices_ = skinningMatrices_ + numSkinningMatrices_;
    }

    void ComponentSkinningRenderer::releaseMatrices()
    {
        if(prevSkinningMatrices_<skinningMatrices_){
            LALIGNED_FREE(prevSkinningMatrices_, lcore::SSE_ALIGN);
            skinningMatrices_ = NULL;
        } else{
            LALIGNED_FREE(skinningMatrices_, lcore::SSE_ALIGN);
            prevSkinningMatrices_ = NULL;
        }
    }

    void ComponentSkinningRenderer::calcNodeMatrices()
    {
        const ComponentGeometric* geometric = getEntity().getGeometric();
        const lmath::Matrix44& matrix = geometric->getMatrix();

        lmath::Matrix44 local;
        s32 i;
        for(i=0; i<model_->getNumNodes(); ++i){
            lfw::Node& node = model_->getNode(i);
            if(lfw::InvalidNode != node.parentIndex_){
                break;
            }

            node.calcLocalMatrix(local);
            node.world0_ = node.world1_;
            node.world1_.mul(matrix, local);
        }

        for(;i<model_->getNumNodes(); ++i){
            lfw::Node& node = model_->getNode(i);
            lfw::Node& parent = model_->getNode(node.parentIndex_ );

            node.calcLocalMatrix(local);
            node.world0_ = node.world1_;
            node.world1_.mul(parent.world1_, local);
        }
    }

    void ComponentSkinningRenderer::calcPrevNodeMatrices()
    {
        lmath::Matrix44 local;
        s32 i;
        for(i=0; i<model_->getNumNodes(); ++i){
            lfw::Node& node = model_->getNode(i);
            if(lfw::InvalidNode != node.parentIndex_){
                break;
            }

            node.calcLocalMatrix(local);
            node.world0_.mul(prevMatrix_, local);
        }

        for(;i<model_->getNumNodes(); ++i){
            lfw::Node& node = model_->getNode(i);
            lfw::Node& parent = model_->getNode(node.parentIndex_);

            node.calcLocalMatrix(local);
            node.world0_.mul(parent.world0_, local);
        }
    }

    void ComponentSkinningRenderer::resetShaderSet()
    {
        ShaderManager& shaderManager = System::getResources().getShaderManager();
        for(s32 i=0; i<model_->getNumMeshes(); ++i){
            shaderManager.setShaderSet(model_->getMesh(i), ShaderSetFlag_None);
        }
    }

    void ComponentSkinningRenderer::resetFlags()
    {
        for(s32 i=0; i<model_->getNumMaterials(); ++i){
            if(model_->getMaterial(i).isCastShadow()){
                setFlag(Flag_ShadowCast);
            }
            if(model_->getMaterial(i).isTransparent()){
                setFlag(Flag_Transparent);
            } else{
                setFlag(Flag_Opaque);
            }
        }
    }
}
