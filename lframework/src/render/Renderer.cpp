/**
@file Renderer.cpp
@author t-sakai
@date 2016/11/10 create
*/
#include "render/Renderer.h"
#include <lgraphics/Graphics.h>
#include "System.h"
#include "render/RendererInitParam.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentRenderer.h"
#include "ecs/ComponentCamera.h"
#include "resource/Resources.h"
#include "resource/ShaderManager.h"
#include "render/graph/RenderGraph.h"
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

    void Renderer::initialize(const RendererInitParam& initParam)
    {
        frameSync_.initialize(initParam.numSyncFrames_);
        constantBufferTableSet_.initialize(&frameSync_, initParam.numSyncFrames_);
        transientVertexBuffer_.initialize(&frameSync_);
        transientIndexBuffer_.initialize(&frameSync_);

        renderContext_.initialize(&lgfx::getDevice());
        renderContext_.setConstantBuffer(&constantBufferTableSet_);
        renderContext_.getShadowMap().initialize(initParam.shadowMapCascades_, initParam.shadowMapSize_, initParam.shadowMapZNear_, initParam.shadowMapZFar_, initParam.shadowMapFormat_);

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
        renderContext_.terminate();
        transientIndexBuffer_.terminate();
        transientVertexBuffer_.terminate();
        constantBufferTableSet_.terminate();
        frameSync_.terminate();
    }

    void Renderer::resetCamera()
    {
        flags_.set(RendererFlag_ResetCamera);
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
        ECSManager& ecsManager = System::getECSManager();
        ComponentRendererManager* rendererManager = ecsManager.getComponentManager<ComponentRendererManager>();
        ComponentRendererManager::ComponentRendererArray& components = rendererManager->getComponents();
        ComponentSceneElementManager* sceneManager = ecsManager.getComponentManager<ComponentSceneElementManager>();

        lfw::ShadowMap& shadowMap = renderContext_.getShadowMap();

        CameraArray cameras = sceneManager->getCameras();
        if(flags_.check(RendererFlag_ResetCamera)){
            //カメラリセット
            flags_.reset(RendererFlag_ResetCamera);

            //レンダーグラフにあるリソースクリア
            System::getRenderGraph().clear();

            for(s32 i=0; i<cameras.size(); ++i){
                sceneManager->getComponent<ComponentCamera>(cameras.componentIndex(i))->resetRenderPasses();
            }

            if(renderQueuePerCamera_.size() <= cameras.size()){
                s32 size = renderQueuePerCamera_.size();
                renderQueuePerCamera_.resize(cameras.size());
                for(s32 i=size; i<renderQueuePerCamera_.size(); ++i){
                    renderQueuePerCamera_[i].initialize();
                    renderQueuePerCamera_[i].setShadowMap(&shadowMap);
                }
            } else{
                for(s32 i=cameras.size(); i<renderQueuePerCamera_.size(); ++i){
                    renderQueuePerCamera_[i].destroy();
                }
                renderQueuePerCamera_.resize(cameras.size());
            }
        }
 
        //カメラ毎にレンダーキュー作成
        if(renderContext_.getShadowMap().isAutoupdateAABB()){
            //Shadow MapのAABB更新
            lmath::lm128 sceneAABBMin = _mm_set1_ps(lcore::numeric_limits<f32>::maximum());
            lmath::lm128 sceneAABBMax = _mm_set1_ps(lcore::numeric_limits<f32>::lowest());
            for(s32 i=0; i<cameras.size(); ++i){
                lmath::lm128 bmin, bmax;

                renderQueuePerCamera_[i].clear();
                renderQueuePerCamera_[i].setCamera(&cameras[i]);
                for(s32 j=0; j<components.size(); ++j){
                    if(0 != (cameras[i].getLayerMask() & components[j]->getLayer())){
                        if(components[j]->addQueue(renderQueuePerCamera_[i])){
                            components[j]->getAABB(bmin, bmax);
                            sceneAABBMin = _mm_min_ps(bmin, sceneAABBMin);
                            sceneAABBMax = _mm_max_ps(bmax, sceneAABBMax);
                        }
                    }
                }
                renderQueuePerCamera_[i].sort();
            }
            lmath::Vector4& aabbMin = shadowMap.getSceneAABBMin();
            lmath::Vector4& aabbMax = shadowMap.getSceneAABBMax();
            _mm_storeu_ps(&aabbMin.x_, sceneAABBMin);
            _mm_storeu_ps(&aabbMax.x_, sceneAABBMax);

        } else{
            for(s32 i=0; i<cameras.size(); ++i){
                renderQueuePerCamera_[i].clear();
                renderQueuePerCamera_[i].setCamera(&cameras[i]);
                for(s32 j=0; j<components.size(); ++j){
                    if(0 != (cameras[i].getLayerMask() & components[j]->getLayer())){
                        components[j]->addQueue(renderQueuePerCamera_[i]);
                    }
                }
                renderQueuePerCamera_[i].sort();
            }
        }
    }

    void Renderer::draw()
    {
        ECSManager& ecsManager = System::getECSManager();
        ComponentSceneElementManager* sceneManager = ecsManager.getComponentManager<ComponentSceneElementManager>();

        lgfx::ContextRef& context = lgfx::getDevice();
        CameraArray cameras = sceneManager->getCameras();
        renderContext_.setLights(sceneManager->getLights());

        System::getRenderGraph().clearShared();

        renderContext_.setPerFrameConstants();
        for(s32 i=0; i<cameras.size(); ++i){
            renderContext_.setRenderQueue(&renderQueuePerCamera_[i]);
            ComponentCamera* componentCamera = sceneManager->getComponent<ComponentCamera>(cameras.componentIndex(i));
            componentCamera->render(renderContext_);
        }

        context.clearVSConstantBuffers(0, 8);
        context.clearGSConstantBuffers(0, 4);
        context.clearPSConstantBuffers(0, 8);
        context.clearCSConstantBuffers(0, 4);
        context.clearHSConstantBuffers(0, 4);
        context.clearDSConstantBuffers(0, 4);
    }
}
