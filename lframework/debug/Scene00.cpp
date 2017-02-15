/**
@file Scene00.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "Scene00.h"
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lgraphics/Graphics.h>
#include <lgraphics/GraphicsDeviceRef.h>
#include <lsound/Context.h>
#include <lsound/UserPlayer.h>

#include <lframework/Application.h>
#include <lframework/input/Keyboard.h>
#include <lframework/input/Mouse.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/ecs/ComponentLight.h>
#include <lframework/ecs/ComponentSprite2D.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture2D.h>

#include <lframework/gui/imgui.h>

#include "MainCamera00.h"
#include "Behavior00.h"
#include "Particle00.h"
#include "VolumeParticle00.h"
#include "Plane00.h"
#include "Text00.h"
#include "BGMID.h"
#include "SEID.h"

namespace debug
{
    Scene00::Scene00()
        :bgmPlayer_(NULL)
        ,roty_(0.0f)
        ,rotx_(0.0f)
        ,showTestWindow_(true)
    {

    }

    Scene00::~Scene00()
    {

    }

    void Scene00::onCreate()
    {
        lsound::Context& context = lsound::Context::getInstance();
        lfw::Application& application = lfw::Application::getInstance();
        context.loadResourcePack(0, "sound/BGM.pak", true);
        context.loadResourcePack(1, "sound/SE.pak", false);
        //bgmPlayer_ = context.createUserPlayer(0, BGM_BGM_MAOUDAMASHII_CYBER02);
        //bgmPlayer_->setFlag(lsound::PlayerFlag_Loop);
        //bgmPlayer_->setGain(0.5f);
        //bgmPlayer_->play();

        mainLight00Entity_ = application.getECSManager().requestCreateGeometric("MainLight00");
        mainLight00Entity_.addComponent<lfw::ComponentLight>();
        mainLight00Entity_.getGeometric()->getRotation().lookAt(lmath::Vector4::construct(normalize(lmath::Vector4::construct(0.2f, -1.0f, 0.0f, 0.0f))));

        mainCamera00Entity_ = application.getECSManager().requestCreateGeometric("MainCamera00");
        debug::MainCamera00* mainCamera00 = mainCamera00Entity_.addComponent<debug::MainCamera00>();
        mainCamera00->initializePerspective(60.0f, 0.01f, 100.0f, true);
        mainCamera00->initializeDeferred(800, 600);
        mainCamera00Entity_.getGeometric()->setPosition(lmath::Vector4::construct(0.5f, 1.5f, -2.0f, 0.0f));

        lfw::ComponentGeometric* scene00Geometric = this->getEntity().getGeometric();

#if 1
        behaviorEntities_[0] = application.getECSManager().requestCreateGeometric("Behavior00");
        behaviorEntities_[0].addComponent<debug::Behavior00>();
        behaviorEntities_[0].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[0].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 0.0f, 0.0f));

        behaviorEntities_[1] = application.getECSManager().requestCreateGeometric("Behavior01");
        behaviorEntities_[1].addComponent<debug::Behavior00>();
        behaviorEntities_[1].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[1].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 2.0f, 0.0f));

        behaviorEntities_[2] = application.getECSManager().requestCreateGeometric("Behavior02");
        behaviorEntities_[2].addComponent<debug::Behavior00>();
        behaviorEntities_[2].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[2].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 4.0f, 0.0f));

        behaviorEntities_[3] = application.getECSManager().requestCreateGeometric("Behavior03");
        behaviorEntities_[3].addComponent<debug::Behavior00>();
        behaviorEntities_[3].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[3].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 6.0f, 0.0f));

        behaviorEntities_[4] = application.getECSManager().requestCreateGeometric("Behavior04");
        behaviorEntities_[4].addComponent<debug::Behavior00>();
        behaviorEntities_[4].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[4].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 8.0f, 0.0f));
        behaviorEntities_[4].getGeometric()->setScale(lmath::Vector3::construct(2.0f, 2.0f, 2.0f));
#endif

        lfw::Entity plane00Entity = application.getECSManager().requestCreateGeometric("Plane00");
        plane00Entity.addComponent<debug::Plane00>();
        plane00Entity.getGeometric()->setParent(scene00Geometric);
        plane00Entity.getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 0.0f, 0.0f, 0.0f));
        plane00Entity.getGeometric()->setScale(lmath::Vector3::construct(10.0f, 10.0f, 10.0f));

        sprite00_ = application.getECSManager().requestCreateGeometric("Sprite00");
        lfw::ComponentSprite2D* sprite2d00 = sprite00_.addComponent<lfw::ComponentSprite2D>();
        lfw::ResourceTexture2D* texWhite = lfw::Resources::getInstance().getEmptyTextureWhite()->cast<lfw::ResourceTexture2D>();
        sprite2d00->setTexture(texWhite->get(), texWhite->getShaderResourceView());
        //sprite2d00->setRect(lmath::Vector4(-1.0f, 1.0f, -0.9f, 0.9f));
        sprite2d00->setScreenRect(0.0f, 0.0f, 20.0f, 20.0f);
        sprite2d00->setTexcoord(0.0f, 0.0f, 1.0f, 1.0f);

        particle00_ = application.getECSManager().requestCreateGeometric("Particle00");
        particle00_.addComponent<Particle00>();

        //lfw::s32 setID=0;
        //lfw::ResourceTexture2D* texGradient = lfw::Resources::getInstance().load(setID, "gradient.dds", lfw::ResourceType::ResourceType_Texture2D)->cast<lfw::ResourceTexture2D>();
        //volumeParticle00_ = application.getECSManager().requestCreateGeometric("VolumeParticle00");
        //VolumeParticle00* volumeParticle = volumeParticle00_.addComponent<VolumeParticle00>();
        //volumeParticle->setTexture(texGradient->get(), texGradient->getShaderResourceView());


        lfw::Entity entityText00 = application.getECSManager().requestCreateGeometric("Text00");
        entityText00.addComponent<Text00>();
    }

    void Scene00::onStart()
    {
    }

    void Scene00::update()
    {
        lfw::Application& application = lfw::Application::getInstance();
        linput::Input& input = application.getInput();
        const linput::Keyboard* keyboard = input.getKeyboard();
        const linput::Mouse* mouse = input.getMouse();

        if(keyboard->isClick(linput::Key_A)){
            lsound::Context& context = lsound::Context::getInstance();
            context.play(1, debug::SE_MISC_MENU);
        }

        lmath::Vector4& cameraTranslation = mainCamera00Entity_.getGeometric()->getPosition();
        lmath::Quaternion& cameraRotation = mainCamera00Entity_.getGeometric()->getRotation();
        if(mouse->isOn(linput::MouseButton_1)){
            roty_ += mouse->getDuration(linput::MouseAxis_X)*0.05f;
            lcore::clampRotate0(roty_, PI2);
            rotx_ += mouse->getDuration(linput::MouseAxis_Y)*0.01f;
            rotx_ = lcore::clamp(rotx_, -89.0f*DEG_TO_RAD, 89.0f*DEG_TO_RAD);
            lmath::Quaternion rotationY = lmath::Quaternion::rotateY(roty_);
            lmath::Quaternion rotationX = lmath::Quaternion::rotateX(rotx_);
            rotationX *= rotationY;
            cameraRotation = rotationX;
        }

        lmath::Vector4 forward = rotate(cameraRotation, lmath::Vector4::Forward);
        forward *= 0.1f;
        lmath::Vector4 right = rotate(cameraRotation, lmath::Vector4::Right);
        right *= 0.1f;

        if(keyboard->isOn(linput::Key_W)){
            cameraTranslation += forward;
        }else if(keyboard->isOn(linput::Key_S)){
            cameraTranslation -= forward;
        }
        if(keyboard->isOn(linput::Key_D)){
            cameraTranslation += right;
        }else if(keyboard->isOn(linput::Key_A)){
            cameraTranslation -= right;
        }

        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowTestWindow(&showTestWindow_);
    }

    void Scene00::onDestroy()
    {
        lsound::Context& context = lsound::Context::getInstance();
        context.destroyUserPlayer(bgmPlayer_);
        bgmPlayer_ = NULL;
    }
}
