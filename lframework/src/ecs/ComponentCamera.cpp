/**
@file ComponentCamera.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "ecs/ComponentCamera.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/Entity.h"
#include <lgraphics/ViewRef.h>

namespace lfw
{
    namespace
    {
        inline ComponentSceneElementManager* getManager()
        {
            return ECSManager::getInstance().getComponentManager<ComponentSceneElementManager>();
        }
    }

    ComponentCamera::ComponentCamera()
    {
    }

    ComponentCamera::~ComponentCamera()
    {
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
        lcore::Log("onCreate");
    }

    void ComponentCamera::onStart()
    {
        lcore::Log("onStart");
    }

    void ComponentCamera::update()
    {
        lcore::Log("update");
    }

    void ComponentCamera::onDestroy()
    {
        lcore::Log("onDestroy");
    }

    void ComponentCamera::initializePerspective(
        f32 fovyDegree,
        f32 znear,
        f32 zfar,
        bool jitter)
    {
        ComponentSceneElementManager* componentManager = getManager();
        Camera& camera = componentManager->getCamera(getID());

        f32 width,height;
        camera.getRTSizeF32(width, height);

        camera.perspectiveFov(fovyDegree*DEG_TO_RAD, static_cast<f32>(width)/height, znear, zfar);

        camera.setViewport(0.0f, 0.0f, width, height);
        camera.setJitter(jitter);
        if(jitter){
            f32 jw = 0.5f/width;
            f32 jh = 0.5f/height;
            camera.setJitterSize(jw, jh);
        }
    }

    void ComponentCamera::initializeDeferred(s32 width, s32 height)
    {
        width = (width+TextureBlockMask)&~TextureBlockMask;
        height = (height+TextureBlockMask)&~TextureBlockMask;

        RenderTarget targets[5];
        DepthStencil depthStencil;
        lgfx::RTVDesc rtvDesc;
        lgfx::DSVDesc dsvDesc;
        lgfx::SRVDesc srvDesc;

        rtvDesc.dimension_ = lgfx::ViewRTVDimension_Texture2D;
        rtvDesc.tex2D_.mipSlice_ = 0;

        dsvDesc.format_ = lgfx::Data_D24_UNorm_S8_UInt;
        dsvDesc.dimension_ = lgfx::ViewDSVDimension_Texture2D;
        dsvDesc.tex2D_.mipSlice_ = 0;

        srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2D;
        srvDesc.tex2D_.mipLevels_ = 1;
        srvDesc.tex2D_.mostDetailedMip_ = 0;

        //Albedo
        //------------------------------------------------
        rtvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
        srvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
        targets[0].texture_ = lgfx::Texture::create2D(
            width,
            height,
            1, 1,
            lgfx::Data_R8G8B8A8_UNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);
        targets[0].renderTargetView_ = targets[0].texture_.createRTView(rtvDesc);
        targets[0].shaderResourceView_ = targets[0].texture_.createSRView(srvDesc);

        //Specular
        //------------------------------------------------
        rtvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
        srvDesc.format_ = lgfx::Data_R8G8B8A8_UNorm;
        targets[1].texture_ = lgfx::Texture::create2D(
            width,
            height,
            1, 1,
            lgfx::Data_R8G8B8A8_UNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);
        targets[1].renderTargetView_ = targets[1].texture_.createRTView(rtvDesc);
        targets[1].shaderResourceView_ = targets[1].texture_.createSRView(srvDesc);

        //Normal
        //------------------------------------------------
        rtvDesc.format_ = lgfx::Data_R16G16B16A16_SNorm;
        srvDesc.format_ = lgfx::Data_R16G16B16A16_SNorm;
        targets[2].texture_ = lgfx::Texture::create2D(
            width,
            height,
            1, 1,
            lgfx::Data_R16G16B16A16_SNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);
        targets[2].renderTargetView_ = targets[2].texture_.createRTView(rtvDesc);
        targets[2].shaderResourceView_ = targets[2].texture_.createSRView(srvDesc);

        //Velocity
        //------------------------------------------------
        rtvDesc.format_ = lgfx::Data_R16G16_Float;
        srvDesc.format_ = lgfx::Data_R16G16_Float;
        targets[3].texture_ = lgfx::Texture::create2D(
            width,
            height,
            1, 1,
            lgfx::Data_R16G16_Float,
            lgfx::Usage_Default,
            lgfx::BindFlag_RenderTarget|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);
        targets[3].renderTargetView_ = targets[3].texture_.createRTView(rtvDesc);
        targets[3].shaderResourceView_ = targets[3].texture_.createSRView(srvDesc);

        //Depth Stencil
        //------------------------------------------------
        depthStencil.texture_ = lgfx::Texture::create2D(
            width,
            height,
            1, 1,
            lgfx::Data_R24G8_TypeLess,
            lgfx::Usage_Default,
            lgfx::BindFlag_DepthStencil|lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            NULL);
        depthStencil.depthStencilView_ = depthStencil.texture_.createDSView(dsvDesc);
        srvDesc.format_ = lgfx::Data_R24_UNorm_X8_TypeLess;
        depthStencil.shaderResourceViewDepth_ = depthStencil.texture_.createSRView(srvDesc);
        srvDesc.format_ = lgfx::Data_X24_TypeLess_G8_UInt;
        depthStencil.shaderResourceViewStencil_ = depthStencil.texture_.createSRView(srvDesc);

        lgfx::Viewport viewport;
        viewport.TopLeftX = viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<f32>(width);
        viewport.Height = static_cast<f32>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        setRenderTargets(viewport, 5, targets, &depthStencil);
        getManager()->getCamera(getID()).setRenderType(RenderType_Deferred);
    }

    const D3D11_VIEWPORT& ComponentCamera::getViewport() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        return componentManager->getCamera(getID()).getViewport();
    }

    void ComponentCamera::setViewport(f32 x, f32 y, f32 width, f32 height)
    {
        ComponentSceneElementManager* componentManager = getManager();
        componentManager->getCamera(getID()).setViewport(x, y, width, height);
    }

    bool ComponentCamera::isUseCameraRenderTarget() const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.isUseCameraRenderTarget();
    }

    s32 ComponentCamera::getScreenWidth() const
    {
        const Camera& camera = getManager()->getCamera(getID());
        f32 width = camera.isUseCameraRenderTarget()? camera.getViewport().Width : lgfx::getDevice().getDefaultViewport().Width;
        return static_cast<s32>(width);
    }

    s32 ComponentCamera::getScreenHeight() const
    {
        const ComponentSceneElementManager* componentManager = getManager();
        const Camera& camera = componentManager->getCamera(getID());

        f32 height = camera.isUseCameraRenderTarget()? camera.getViewport().Height : lgfx::getDevice().getDefaultViewport().Height;
        return static_cast<s32>(height);
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

    s16 ComponentCamera::getNumRenderTargets() const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getNumRenderTargets();
    }

    void ComponentCamera::setRenderTargets(const D3D11_VIEWPORT& viewport, s8 numTargets, const RenderTarget* targets, const DepthStencil* depthStencil)
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.setRenderTargets(viewport, numTargets, targets, depthStencil);
    }

    void ComponentCamera::clearRenderTargets()
    {
        Camera& camera = getManager()->getCamera(getID());
        camera.clearRenderTargets();
    }

    const lgfx::Texture2DRef& ComponentCamera::getRTTexture(s32 index) const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getRTTexture(index);
    }

    const lgfx::RenderTargetViewRef& ComponentCamera::getRTView(s32 index) const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getRTView(index);
    }

    const lgfx::ShaderResourceViewRef& ComponentCamera::getSRView(s32 index) const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getSRView(index);
    }

    const lgfx::UnorderedAccessViewRef& ComponentCamera::getUAView(s32 index) const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getUAView(index);
    }

    const DepthStencil& ComponentCamera::getDepthStencil() const
    {
        const Camera& camera = getManager()->getCamera(getID());
        return camera.getDepthStencil();
    }
}
