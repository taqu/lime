/**
@file Renderer.cpp
@author t-sakai
@date 2016/11/10 create
*/
#include "render/Renderer.h"
#include <lgraphics/Graphics.h>
#include "render/RendererInitParam.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentRenderer.h"
#include "resource/Resources.h"
#include "resource/ShaderManager.h"

namespace lfw
{
    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {
        for(s32 i=0; i<renderQueuePerCamera_.size(); ++i){
            renderQueuePerCamera_[i].destroy();
        }
    }

    void Renderer::initialize(const RendererInitParam& initParam)
    {
        frameSync_.initialize(initParam.numSyncFrames_);
        constantBufferTableSet_.initialize(&frameSync_, initParam.numSyncFrames_);
        transientVertexBuffer_.initialize(&frameSync_);
        transientIndexBuffer_.initialize(&frameSync_);

        renderContext_.initialize(&lgfx::getDevice());
        renderContext_.setConstantBuffer(&constantBufferTableSet_);
        ShaderManager& shaderManager = Resources::getInstance().getShaderManager();
        renderContext_.setDepthShaderVS(RenderContext::DepthShader_Normal, shaderManager.getVS(ShaderVS_LightDepth));
        renderContext_.setDepthShaderVS(RenderContext::DepthShader_Skinning, shaderManager.getVS(ShaderVS_LightDepthSkinning));
        renderContext_.setDepshShaderGS(shaderManager.getGS(ShaderGS_LighDepth));
        renderContext_.setShadowMap(&shadowMap_);

        s32 cascadeLevels = 2;
        shadowMap_.initialize(cascadeLevels, initParam.shadowMapSize_, initParam.shadowMapZNear_, initParam.shadowMapZFar_);
        shadowMap_.setSceneAABB(lmath::Vector4(-100.0f, -100.0f, -20.0f, 0.0f), lmath::Vector4(100.0f, 100.0f, 80.0f, 0.0f));
        {
            lgfx::DSVDesc dsvDesc;
            lgfx::SRVDesc srvDesc;

            dsvDesc.format_ = lgfx::Data_D32_Float;
            dsvDesc.dimension_ = lgfx::ViewDSVDimension_Texture2DArray;
            dsvDesc.tex2DArray_.mipSlice_ = 0;
            dsvDesc.tex2DArray_.firstArraySlice_ = 0;
            dsvDesc.tex2DArray_.arraySize_ = cascadeLevels;

            srvDesc.format_ = lgfx::Data_R32_Float;
            srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2DArray;
            srvDesc.tex2DArray_.mostDetailedMip_ = 0;
            srvDesc.tex2DArray_.mipLevels_ = 1;
            srvDesc.tex2DArray_.firstArraySlice_ = 0;
            srvDesc.tex2DArray_.arraySize_ = cascadeLevels;

            shadowMapDepthStencil_.texture_ = lgfx::Texture::create2D(
                initParam.shadowMapSize_,
                initParam.shadowMapSize_,
                1, cascadeLevels,
                lgfx::Data_R32_TypeLess,
                lgfx::Usage_Default,
                lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);
            shadowMapDepthStencil_.depthStencilView_ = shadowMapDepthStencil_.texture_.createDSView(dsvDesc);
            shadowMapDepthStencil_.shaderResourceView_ = shadowMapDepthStencil_.texture_.createSRView(srvDesc);
        }
    }

    void Renderer::terminate()
    {
        for(s32 i=0; i<renderQueuePerCamera_.size(); ++i){
            renderQueuePerCamera_[i].destroy();
        }
        renderContext_.terminate();
        transientIndexBuffer_.terminate();
        transientVertexBuffer_.terminate();
        constantBufferTableSet_.terminate();
        frameSync_.terminate();
    }

    void Renderer::update()
    {
        lgfx::GraphicsDeviceRef& device = lgfx::getDevice();
        frameSync_.begin(device);
        constantBufferTableSet_.begin();
        transientVertexBuffer_.begin();
        transientIndexBuffer_.begin();

        traverseComponents();
        draw();

        frameSync_.end(device);
        device.present();
    }

    void Renderer::traverseComponents()
    {
        ComponentRendererManager* rendererManager = ECSManager::getInstance().getComponentManager<ComponentRendererManager>();
        ComponentRendererManager::ComponentRendererArray& components = rendererManager->getComponents();

        ComponentSceneElementManager* sceneManager = ECSManager::getInstance().getComponentManager<ComponentSceneElementManager>();

        const ComponentSceneElementManager::CameraArray& cameras = sceneManager->getCameras();
        if(renderQueuePerCamera_.size() != cameras.size()){
            if(renderQueuePerCamera_.size() <= cameras.size()){
                s32 size = renderQueuePerCamera_.size();
                renderQueuePerCamera_.resize(cameras.size());
                for(s32 i=size; i<renderQueuePerCamera_.size(); ++i){
                    renderQueuePerCamera_[i].initialize();
                }
            } else{
                for(s32 i=cameras.size(); i<renderQueuePerCamera_.size(); ++i){
                    renderQueuePerCamera_[i].destroy();
                }
                renderQueuePerCamera_.resize(cameras.size());
            }
        }

        for(s32 i=0; i<cameras.size(); ++i){
            renderQueuePerCamera_[i].clear();
            renderQueuePerCamera_[i].setCameraPosition(cameras[i].camera_.getEyePosition());
            for(s32 j=0; j<components.size(); ++j){
                if(0 != (cameras[i].camera_.getLayerMask() & components[j]->getLayer())){
                    components[j]->addQueue(renderQueuePerCamera_[i]);
                }
            }
            renderQueuePerCamera_[i].sort();
        }
    }

    void Renderer::draw()
    {
        //ComponentRendererManager* rendererManager = ECSManager::getInstance().getComponentManager<ComponentRendererManager>();
        //ComponentRendererManager::ComponentRendererArray& components = rendererManager->getComponents();

        ComponentSceneElementManager* sceneManager = ECSManager::getInstance().getComponentManager<ComponentSceneElementManager>();

        lgfx::ContextRef& context = lgfx::getDevice();
        ComponentSceneElementManager::CameraArray& cameras = sceneManager->getCameras();
        ComponentSceneElementManager::LightArray& lights = sceneManager->getLights();

        if(0<lights.size()){
            for(s32 i=0; i<cameras.size(); ++i){
                Camera& camera = cameras[i].camera_;
                //Depth path
                switch(camera.getShadowType())
                {
                case ShadowType_ShadowMap:
                {
                    context.setRenderTargets(0, NULL, shadowMapDepthStencil_.depthStencilView_.get());
                    context.setViewport(0.0f, 0.0f, shadowMap_.getResolution(), shadowMap_.getResolution());
                    context.clearDepthStencilView(shadowMapDepthStencil_.depthStencilView_.get(), lgfx::ClearFlag_Depth, 0.0f, 0);
                    context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
                    shadowMap_.update(camera, lights[0].light_);
                    renderContext_.setPerShadowMapConstants();

                    RenderQueue& queue = renderQueuePerCamera_[i];
                    renderContext_.setRenderPath(RenderPath_Shadow);
                    for(s32 j=0; j<queue[RenderPath_Shadow].size_; ++j){
                        queue[RenderPath_Shadow].entries_[j].component_->drawDepth(renderContext_);
                    }
                    i=cameras.size();
                }
                break;
                }
            }
        }

        renderContext_.setPerFrameConstants();

        for(s32 i=0; i<cameras.size(); ++i){
            Camera& camera = cameras[i].camera_;
            renderContext_.setCamera(&camera);
            renderContext_.setPerCameraConstants(camera);

            RenderQueue& queue = renderQueuePerCamera_[i];
            camera.begin(context);

            //Opaque path
            renderContext_.setRenderPath(RenderPath_Opaque);
            for(s32 j=0; j<queue[RenderPath_Opaque].size_; ++j){
                queue[RenderPath_Opaque].entries_[j].component_->drawOpaque(renderContext_);
            }
            //Transparent path
            renderContext_.setRenderPath(RenderPath_Transparent);
            for(s32 j=0; j<queue[RenderPath_Transparent].size_; ++j){
                queue[RenderPath_Transparent].entries_[j].component_->drawTransparent(renderContext_);
            }
        }
        context.restoreDefaultRenderTargets();
    }
}
