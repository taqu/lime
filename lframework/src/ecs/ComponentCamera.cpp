/**
@file ComponentCamera.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "ecs/ComponentCamera.h"

#include <lmath/Vector3.h>
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lgraphics/ViewRef.h>

#include "System.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/Entity.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ComponentRenderer.h"

#include "resource/Resources.h"
#include "resource/ShaderManager.h"

#include "render/Renderer.h"
#include "render/RenderContext.h"
#include "render/RenderQueue.h"

#include "render/graph/RenderGraph.h"
#include "render/graph/RenderPassGBuffer.h"
#include "render/graph/RenderPassLighting.h"
#include "render/graph/RenderPassIntegration.h"
#include "render/graph/RenderPassTransparent.h"
#include "render/graph/RenderPassUI.h"

namespace lfw
{
    namespace
    {
        inline ComponentSceneElementManager* getManager()
        {
            return System::getECSManager().getComponentManager<ComponentSceneElementManager>();
        }
    }

    ComponentCamera::ComponentCamera()
    {
    }

    ComponentCamera::~ComponentCamera()
    {
    }

    Entity& ComponentCamera::getEntity()
    {
        ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    const Entity& ComponentCamera::getEntity() const
    {
        ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getData(getID()).entity_;
    }

    u32 ComponentCamera::getType() const
    {
        ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getData(getID()).type();
    }

    void ComponentCamera::onCreate()
    {
        System::getRenderer().resetCamera();
    }

    void ComponentCamera::onStart()
    {
    }

    void ComponentCamera::update()
    {
    }

    void ComponentCamera::onDestroy()
    {
        System::getRenderer().resetCamera();
    }

#if 0
    void ComponentCamera::render(lfw::Camera& camera, RenderTask& renderTask)
    {
        RenderContext& renderContext = *renderTask.renderContext_;
        lgfx::ContextRef& context = renderContext.getContext();

        renderContext.setCamera(&camera);
        renderContext.setPerCameraConstants(camera);
        renderContext.resetDefaultSamplerSet();

        RenderQueue& queue = *renderTask.renderQueue_;

        camera.beginDeferred(context);

        //Opaque path
        renderContext.setRenderPath(RenderPath_Opaque);
        for(s32 j=0; j<queue[RenderPath_Opaque].size_; ++j){
            queue[RenderPath_Opaque].entries_[j].component_->drawOpaque(renderContext);
        }
        camera.endDeferred(context);

        //LinearZ
        camera.constructLinearZ(context);

        //Camera Motion
        camera.addCameraMotion(context);

        //Opaque Lighting
        deferredLighting(camera, renderTask);

        //Transparent path
        context.setRenderTargets(1, camera.getDeferredLightingRenderTarget().renderTargetView_, camera.getDepthStencil().depthStencilView_.get());
        renderContext.setRenderPath(RenderPath_Transparent);
        renderContext.resetDefaultSamplerSet();
        for(s32 j=0; j<queue[RenderPath_Transparent].size_; ++j){
            queue[RenderPath_Transparent].entries_[j].component_->drawTransparent(renderContext);
        }
        context.restoreDefaultRenderTargets();

        //-----------------------------------------------------
        ShaderManager& shaderManager = Resources::getInstance().getShaderManager();

        lgfx::VertexShaderRef& fullQuadVS = shaderManager.getVS(ShaderVS_FullQuad);
        lgfx::PixelShaderRef& copyPS = shaderManager.getPS(ShaderPS_Copy);

        context.setVertexShader(fullQuadVS);
        context.setPixelShader(copyPS);
        context.setPSResources(0, 1, camera.getDeferredLightingRenderTarget().shaderResourceView_);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DDisableWDisable);
        context.clearVertexBuffers(0, 1);
        context.clearIndexBuffers();
        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.draw(3, 0);

        context.setInputLayout(NULL);
        context.clearPSResources(8);
        context.clearPSSamplers(8);
        context.setPixelShader(NULL);
        context.setVertexShader(NULL);
    }

    void ComponentCamera::deferredLighting(lfw::Camera& camera, RenderTask& renderTask)
    {
        RenderContext& renderContext = *renderTask.renderContext_;
        lgfx::ContextRef& context = renderContext.getContext();

        //Clear shadow accumulation buffer
        context.setRenderTargets(1, camera.getShadowAccumulatingRenderTarget().renderTargetView_, camera.getDepthStencil().depthStencilView_);
        context.clearRenderTargetView(camera.getShadowAccumulatingRenderTarget().renderTargetView_, lgfx::ContextRef::Zero);

        context.setPSSamplers(0, camera.getDeferredSamplerSet().getNumSamplers(), camera.getDeferredSamplerSet().getSamplers());

        context.clearVertexBuffers(0, 1);
        context.setPrimitiveTopology(lgfx::Primitive_TriangleList);
        context.setDepthStencilState(camera.getDeferredDepthStencilState(), 0);
        context.setBlendState(lgfx::ContextRef::BlendState_NoAlpha);

        //Set shader resources
        context.setPSResources(0, camera.getNumRenderTargets(), camera.getSRViews());
        context.setPSResources(7, 1, renderContext.getShadowMapDepthStencil().shaderResourceViewDepth_);

        context.setVertexShader(camera.getFullQuadVS());
        context.setGeometryShader(NULL);
        context.setPixelShader(camera.getDeferredShadowAccumulatingPS());

        //Draw full quad
        context.draw(3, 0);

        //Clear accumulation buffer
        context.setRenderTargets(1, camera.getDeferredLightingRenderTarget().renderTargetView_, camera.getDepthStencil().depthStencilView_);
        context.clearRenderTargetView(camera.getDeferredLightingRenderTarget().renderTargetView_, lgfx::ContextRef::Zero);

        context.setPixelShader(camera.getDeferredLightingPS());
        context.setBlendState(lgfx::ContextRef::BlendState_Add);
        context.setPSResources(7, 1, camera.getShadowAccumulatingRenderTarget().shaderResourceView_);

        ComponentSceneElementManager::LightArray& lights = *renderTask.lights_;
        for(s32 j=0; j<lights.size(); ++j){
            renderContext.beginDeferredLighting(lights[j].light_);
            context.draw(3, 0);
            renderContext.endDeferredLighting(lights[j].light_);
        }
        context.clearPSResources(8);
    }
#endif

    void ComponentCamera::initialize(
        f32 fovyDegree,
        f32 znear,
        f32 zfar,
        s32 width,
        s32 height,
        bool jitter)
    {
        ComponentSceneElementManager* componentManager = getManager();
        Camera& camera = componentManager->getCamera(getID());

        camera.perspectiveFov(fovyDegree*DEG_TO_RAD, static_cast<f32>(width)/height, znear, zfar);

        camera.setViewport(0, 0, width, height);
        camera.setJitter(jitter);
        if(jitter){
            f32 jw = 0.125f/width;
            f32 jh = 0.125f/height;
            camera.setJitterSize(jw, jh);
        }
        System::getRenderer().resetCamera();
    }

    void ComponentCamera::resetRenderPasses()
    {
        ComponentSceneElementManager* componentManager = getManager();
        Camera& camera = componentManager->getCamera(getID());

        Camera::RenderPassArray& renderPasses = camera.getRenderPasses();
        if(0<renderPasses.size()){
            for(s32 i=0; i<renderPasses.size(); ++i){
                renderPasses[i]->create(camera);
            }
            return;
        }

        //GBuffer
        graph::RenderPassGBuffer* renderPassGBuffer = LNEW graph::RenderPassGBuffer();
        camera.addRenderPass(renderPassGBuffer);

        graph::RenderPassLighting* renderPassLighting = LNEW graph::RenderPassLighting();
        camera.addRenderPass(renderPassLighting);

        graph::RenderPassTransparent* renderPassTransparent = LNEW graph::RenderPassTransparent();
        camera.addRenderPass(renderPassTransparent);

        graph::RenderPassIntegration* renderPassIntegration = LNEW graph::RenderPassIntegration();
        camera.addRenderPass(renderPassIntegration);

        graph::RenderPassUI* renderPassUI = LNEW graph::RenderPassUI();
        camera.addRenderPass(renderPassUI);

        for(s32 i=0; i<renderPasses.size(); ++i){
            renderPasses[i]->create(camera);
        }
    }

    void ComponentCamera::render(RenderContext& renderContext)
    {
        ComponentSceneElementManager* componentManager = getManager();
        Camera& camera = componentManager->getCamera(getID());

        renderContext.setPerCameraConstants(camera);

        lgfx::ContextRef& context = renderContext.getContext();

        context.setViewport(camera.getViewport());
        context.setDepthStencilState(lgfx::ContextRef::DepthStencil_DEnableWEnableReverseZ);
        context.setRasterizerState(lgfx::ContextRef::Rasterizer_FillSolid);

        Camera::RenderPassArray& passes = camera.getRenderPasses();
        for(s32 i=0; i<passes.size(); ++i){
            passes[i]->execute(renderContext, camera);
        }
    }

    const lgfx::Viewport& ComponentCamera::getViewport() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getViewport();
    }

    void ComponentCamera::setViewport(s32 x, s32 y, s32 width, s32 height)
    {
        ComponentSceneElementManager* componentManager = getManager();
        componentManager->getCamera(getID()).setViewport(x, y, width, height);
    }

    s32 ComponentCamera::getSortLayer() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getSortLayer();
    }

    void ComponentCamera::setSortLayer(s32 layer)
    {
        ComponentSceneElementManager* componentManager = getManager();
        componentManager->getCamera(getID()).setSortLayer(layer);
        componentManager->sortCameras();
    }

    s32 ComponentCamera::getTargetLayerMask() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getLayerMask();
    }

    void ComponentCamera::setTargetLayerMask(s32 layerMask)
    {
        ComponentSceneElementManager* componentManager = getManager();
        componentManager->getCamera(getID()).setLayerMask(layerMask);
    }

    f32 ComponentCamera::getZNear() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getZNear();
    }

    f32 ComponentCamera::getZFar() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getZFar();
    }

    f32 ComponentCamera::getAspect() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getAspect();
    }

    f32 ComponentCamera::getFovy() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getFovy();
    }

    const lmath::Matrix44& ComponentCamera::getProjMatrix() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getProjMatrix();
    }

    // “§Ž‹“Š‰e
    void ComponentCamera::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.perspective(width, height, znear, zfar);
    }

    // “§Ž‹“Š‰e
    void ComponentCamera::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.perspectiveFov(fovy, aspect, znear, zfar);
    }

    // “§Ž‹“Š‰e
    void ComponentCamera::perspectiveLens(f32 focalLength, f32 frameHeight, f32 aspect, f32 znear, f32 zfar)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.perspectiveLens(focalLength, frameHeight, aspect, znear, zfar);
    }

    // •½s“Š‰e
    void ComponentCamera::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.ortho(width, height, znear, zfar);
    }

    void ComponentCamera::lookAt(const lmath::Vector3& eye, const lmath::Vector3& at)
    {
        Entity entity = getEntity();
        entity.getGeometric()->getRotation().lookAt(eye, at);
        entity.getGeometric()->setPosition(lmath::Vector4::construct(eye));
    }

    void ComponentCamera::lookAt(const lmath::Vector4& eye, const lmath::Vector4& at)
    {
        Entity entity = getEntity();
        entity.getGeometric()->getRotation().lookAt(eye, at);
        entity.getGeometric()->setPosition(eye);
    }

    void ComponentCamera::setJitter(bool enable)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.setJitter(enable);
    }

    bool ComponentCamera::isJitter() const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.isJitter();
    }

    void ComponentCamera::setJitterSize(f32 width, f32 height)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.setJitterSize(width, height);
    }
}
