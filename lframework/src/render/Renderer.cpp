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
#include "ecs/ComponentRenderer2DManager.h"
#include "ecs/ComponentRenderer2D.h"
#include "ecs/ComponentCamera.h"
#include "resource/Resources.h"
#include "resource/ShaderManager.h"
#include "render/RenderTask.h"
#include "gui/gui.h"

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

    void Renderer::initialize(s32 backBufferWidth, s32 backBufferHeight, const RendererInitParam& initParam)
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

        renderContext2D_.initialize(
            &lgfx::getDevice(),
            &constantBufferTableSet_,
            &frameSync_,
            0, 0, backBufferWidth, backBufferHeight,
            initParam.max2DVerticesPerFrame_,
            initParam.max2DIndicesPerFrame_);

        s32 cascadeLevels = 2;
        shadowMap_.initialize(cascadeLevels, initParam.shadowMapSize_, initParam.shadowMapZNear_, initParam.shadowMapZFar_);
        shadowMap_.setSceneAABB(lmath::Vector4::construct(-100.0f, -100.0f, -20.0f, 0.0f), lmath::Vector4::construct(100.0f, 100.0f, 80.0f, 0.0f));
        {
            DepthStencil& shadowMapDepthStencil = renderContext_.getShadowMapDepthStencil();
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

            shadowMapDepthStencil.texture_ = lgfx::Texture::create2D(
                initParam.shadowMapSize_,
                initParam.shadowMapSize_,
                1, cascadeLevels,
                lgfx::Data_R32_TypeLess,
                lgfx::Usage_Default,
                lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                NULL);
            shadowMapDepthStencil.depthStencilView_ = shadowMapDepthStencil.texture_.createDSView(dsvDesc);
            shadowMapDepthStencil.shaderResourceViewDepth_ = shadowMapDepthStencil.texture_.createSRView(srvDesc);
        }

#ifdef LFW_ENABLE_GUI
        GUI::initialize();
#endif
    }

    void Renderer::terminate()
    {
#ifdef LFW_ENABLE_GUI
        GUI::terminate();
#endif

        for(s32 i=0; i<renderQueuePerCamera_.size(); ++i){
            renderQueuePerCamera_[i].destroy();
        }
        renderContext2D_.terminate();
        renderContext_.terminate();
        transientIndexBuffer_.terminate();
        transientVertexBuffer_.terminate();
        constantBufferTableSet_.terminate();
        frameSync_.terminate();
    }

    void Renderer::begin()
    {
        lgfx::GraphicsDeviceRef& device = lgfx::getDevice();
        frameSync_.begin(device);
        constantBufferTableSet_.begin();
        transientVertexBuffer_.begin();
        transientIndexBuffer_.begin();

#ifdef LFW_ENABLE_GUI
        if(GUI::valid()){
            GUI::getInstance().begin();
        }
#endif
    }

    void Renderer::update()
    {
        lgfx::GraphicsDeviceRef& device = lgfx::getDevice();
        traverseComponents();
        draw();

#ifdef LFW_ENABLE_GUI
        if(GUI::valid()){
            GUI::getInstance().render();
        }
#endif
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
            //ÉJÉÅÉâÇÃêîÇ™ïœÇÌÇ¡ÇΩ
            if(renderQueuePerCamera_.size() <= cameras.size()){
                s32 size = renderQueuePerCamera_.size();
                renderQueuePerCamera_.resize(cameras.size());
                for(s32 i=size; i<renderQueuePerCamera_.size(); ++i){
                    renderQueuePerCamera_[i].initialize();
                    renderQueuePerCamera_[i].setShadowMap(&shadowMap_);
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
            renderQueuePerCamera_[i].setCamera(&cameras[i].camera_);
            for(s32 j=0; j<components.size(); ++j){
                if(0 != (cameras[i].camera_.getLayerMask() & components[j]->getLayer())){
                    components[j]->addQueue(renderQueuePerCamera_[i]);
                }
            }
            renderQueuePerCamera_[i].sort();
        }

        //
        renderQueue2D_.clear();
        ComponentRenderer2DManager* renderer2DManager = ECSManager::getInstance().getComponentManager<ComponentRenderer2DManager>();
        ComponentRenderer2DManager::ComponentRendererArray& components2D = renderer2DManager->getComponents();
        for(s32 i=0; i<components2D.size(); ++i){
            components2D[i]->addQueue(renderQueue2D_);
        }
    }

    void Renderer::draw()
    {
        ComponentSceneElementManager* sceneManager = ECSManager::getInstance().getComponentManager<ComponentSceneElementManager>();

        lgfx::ContextRef& context = lgfx::getDevice();
        ComponentSceneElementManager::CameraArray& cameras = sceneManager->getCameras();
        ComponentSceneElementManager::LightArray& lights = sceneManager->getLights();

        if(0<lights.size()){
            lgfx::DepthStencilViewRef& depthStencilView = renderContext_.getShadowMapDepthStencil().depthStencilView_;
            for(s32 i=0; i<cameras.size(); ++i){
                Camera& camera = cameras[i].camera_;
                context.setRenderTargets(0, NULL, depthStencilView.get());
                context.setViewport(0.0f, 0.0f, static_cast<f32>(shadowMap_.getResolution()), static_cast<f32>(shadowMap_.getResolution()));
                context.clearDepthStencilView(depthStencilView.get(), lgfx::ClearFlag_Depth, 0.0f, 0);
                context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
                context.setRasterizerState(lgfx::ContextRef::Rasterizer_DepthMap);
                shadowMap_.update(camera, lights[0].light_);
                renderContext_.setPerShadowMapConstants();

                RenderQueue& queue = renderQueuePerCamera_[i];
                renderContext_.setRenderPath(RenderPath_Shadow);
                for(s32 j=0; j<queue[RenderPath_Shadow].size_; ++j){
                    queue[RenderPath_Shadow].entries_[j].component_->drawDepth(renderContext_);
                }
                i=cameras.size();
            }
        }

        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);
        renderContext_.setPerFrameConstants();

        RenderTask renderTask;
        renderTask.renderContext_ = &renderContext_;
        renderTask.lights_ = &lights;
        for(s32 i=0; i<cameras.size(); ++i){
            renderTask.renderQueue_ = &renderQueuePerCamera_[i];
            Camera& camera = cameras[i].camera_;
            ComponentCamera* componentCamera = sceneManager->getComponent<ComponentCamera>(cameras[i].componentIndex_);
            componentCamera->render(camera, renderTask);
        }

        context.restoreDefaultViewport();

        //Draw 2D
        RenderQueue2D::Queue& queue2D = renderQueue2D_.get();
        renderContext2D_.begin();
        for(s32 i=0; i<queue2D.size_; ++i){
            queue2D.entries_[i].component_->draw(renderContext2D_);
        }
        renderContext2D_.end();

        context.clearVSConstantBuffers(8);
        context.clearGSConstantBuffers(4);
        context.clearPSConstantBuffers(8);
        context.clearCSConstantBuffers(4);
        context.clearHSConstantBuffers(4);
        context.clearDSConstantBuffers(4);
    }
}
