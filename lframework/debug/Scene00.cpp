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

#include <lframework/System.h>
#include <lframework/input/Keyboard.h>
#include <lframework/input/Mouse.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/ecs/ComponentCanvas.h>
#include <lframework/ecs/ComponentLight.h>
#include <lframework/ecs/ComponentUIText.h>
#include <lframework/ecs/ComponentUIImage.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture2D.h>
#include <lframework/render/graph/RenderGraph.h>
#include <lframework/gui/imgui.h>

#include "MainCamera00.h"
#include "Behavior00.h"
#include "Particle00.h"
#include "VolumeParticle00.h"
#include "VolumeParticle01.h"
#include "Plane00.h"
#include "BGMID.h"
#include "SEID.h"
#include "ShaderID.h"

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
        lfw::ShaderManager& shaderManager = lfw::System::getResources().getShaderManager();
        shaderManager.loadVS(ShaderVS_ProceduralSpace, ShaderCompiledBytes::getSizeVS(ShaderVS_ProceduralSpace), ShaderCompiledBytes::getBytesVS(ShaderVS_ProceduralSpace));
        shaderManager.loadPS(ShaderPS_ProceduralSpace, ShaderCompiledBytes::getSizePS(ShaderPS_ProceduralSpace), ShaderCompiledBytes::getBytesPS(ShaderPS_ProceduralSpace));
        shaderManager.loadPS(ShaderPS_ProceduralTerrain00, ShaderCompiledBytes::getSizePS(ShaderPS_ProceduralTerrain00), ShaderCompiledBytes::getBytesPS(ShaderPS_ProceduralTerrain00));

        lsound::Context& context = lsound::Context::getInstance();
        context.loadResourcePack(0, "sound/BGM.pak", true);
        context.loadResourcePack(1, "sound/SE.pak", false);
        //bgmPlayer_ = context.createUserPlayer(0, BGM_BGM_MAOUDAMASHII_CYBER02);
        //bgmPlayer_->setFlag(lsound::PlayerFlag_Loop);
        //bgmPlayer_->setGain(0.5f);
        //bgmPlayer_->play();

        lfw::ECSManager& ecsManager = lfw::System::getECSManager();
        mainLight00Entity_ = ecsManager.requestCreateGeometric("MainLight00");
        lfw::ComponentLight* componentLight = mainLight00Entity_.addComponent<lfw::ComponentLight>();
        mainLight00Entity_.getGeometric()->getRotation().lookAt(lmath::normalize(lmath::Vector4::construct(0.5f, -1.0f, 0.0f, 0.0f)));
        lfw::Light& light = componentLight->getLight();
        light.setCastShadow(true);

        mainCamera00Entity_ = ecsManager.requestCreateGeometric("MainCamera00");
        debug::MainCamera00* mainCamera00 = mainCamera00Entity_.addComponent<debug::MainCamera00>();
        mainCamera00->initialize(60.0f, 0.01f, 100.0f, 800, 600, false);
        mainCamera00Entity_.getGeometric()->lookAt(lmath::Vector4::construct(0.0f, 2.0f, -4.0f, 0.0f), lmath::Vector4::construct(0.0f, 0.0f, 0.0f, 0.0f));

        lfw::ComponentGeometric* scene00Geometric = this->getEntity().getGeometric();
#if 1
        //behaviorEntities_[0] = application.getECSManager().requestCreateGeometric("Behavior00");
        //behaviorEntities_[0].addComponent<debug::Behavior00>();
        //behaviorEntities_[0].getGeometric()->setParent(scene00Geometric);
        //behaviorEntities_[0].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 0.0f, 0.0f));

        //behaviorEntities_[1] = application.getECSManager().requestCreateGeometric("Behavior01");
        //behaviorEntities_[1].addComponent<debug::Behavior00>();
        //behaviorEntities_[1].getGeometric()->setParent(scene00Geometric);
        //behaviorEntities_[1].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 2.0f, 0.0f));

        //behaviorEntities_[2] = application.getECSManager().requestCreateGeometric("Behavior02");
        //behaviorEntities_[2].addComponent<debug::Behavior00>();
        //behaviorEntities_[2].getGeometric()->setParent(scene00Geometric);
        //behaviorEntities_[2].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 2.0f, 4.0f, 0.0f));

        behaviorEntities_[3] = ecsManager.requestCreateGeometric("Behavior03");
        behaviorEntities_[3].addComponent<debug::Behavior00>();
        behaviorEntities_[3].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[3].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 0.0f, 2.0f, 0.0f));

        behaviorEntities_[4] = ecsManager.requestCreateGeometric("Behavior04");
        behaviorEntities_[4].addComponent<debug::Behavior00>();
        behaviorEntities_[4].getGeometric()->setParent(scene00Geometric);
        behaviorEntities_[4].getGeometric()->setPosition(lmath::Vector4::construct(0.0f, 0.0f, 0.0f, 0.0f));
        behaviorEntities_[4].getGeometric()->setScale(lmath::Vector3::construct(2.0f, 2.0f, 2.0f));
#endif

        lfw::Entity plane00Entity = ecsManager.requestCreateGeometric("Plane00");
        plane00Entity.addComponent<debug::Plane00>();
        plane00Entity.getGeometric()->setParent(scene00Geometric);
        plane00Entity.getGeometric()->setPosition(lmath::Vector4::construct(0.0f, -2.0f, 0.0f, 0.0f));
        plane00Entity.getGeometric()->setScale(lmath::Vector3::construct(10.0f, 10.0f, 10.0f));

        particle00_ = ecsManager.requestCreateGeometric("Particle00");
        particle00_.addComponent<Particle00>();

        {
            volumeParticle00_ = ecsManager.requestCreateGeometric("VolumeParticle00");
            VolumeParticle00* volumeParticle00 = volumeParticle00_.addComponent<VolumeParticle00>();

            volumeParticle01_ = ecsManager.requestCreateGeometric("VolumeParticle01");
            VolumeParticle01* volumeParticle01 = volumeParticle01_.addComponent<VolumeParticle01>();
            volumeParticle01_.getGeometric()->setPosition(lmath::Vector4::construct(1.0f, 0.0f, 0.0f, 0.0f));
        }


        //
        lfw::Entity canvasEntity = ecsManager.requestCreateGeometric("Canvas");
        lfw::ComponentCanvas* canvas = canvasEntity.addComponent<lfw::ComponentCanvas>();
        canvas->setScreen(200, 200, 400, 400);

        lfw::Entity entityText00 = ecsManager.requestCreateGeometric("Text00");
        entityText00.getGeometric()->setParent(canvasEntity.getGeometric());
        lfw::ComponentUIText* UIText00 = entityText00.addComponent<lfw::ComponentUIText>();
        UIText00->setFont(0);
        UIText00->setRect(200.0f, 0.0f, 100.0f, 400.0f);
        UIText00->text().printf("test00\ntest00\ntest00");
        UIText00->updateText();

        {
            lfw::s32 setID=0;
            lfw::ResourceTexture2D* texGradient = lfw::System::getResources().load(setID, "gradient.dds", lfw::ResourceType::ResourceType_Texture2D)->cast<lfw::ResourceTexture2D>();
            lfw::Entity entityImage00 = ecsManager.requestCreateGeometric("Image00");
            entityImage00.getGeometric()->setParent(canvasEntity.getGeometric());
            lfw::ComponentUIImage* UIImage00 = entityImage00.addComponent<lfw::ComponentUIImage>();
            UIImage00->setImage(texGradient->getShaderResourceView());
            UIImage00->setRect(10.0f, 10.0f, 64.0f, 64.0f);
            UIImage00->setUVRect(0.0f, 0.0f, 1.0f, 1.0f);
            entityImage00.getGeometric()->setFirstSibling();
        }
        count_ = 0;
    }

    void Scene00::onStart()
    {
        volumeParticle00_.resetFlag(lfw::EntityFlag_UpdateSelf);
    }

    void Scene00::update()
    {
        linput::Input& input = lfw::System::getInput();
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
        forward *= 0.075f;
        lmath::Vector4 right = rotate(cameraRotation, lmath::Vector4::Right);
        right *= 0.075f;

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

        //if(count_<1024){
        //    ++count_;

        //    for(int i=0; i<64; ++i){
        //        lfw::ECSManager& ecsManager = lfw::System::getECSManager();
        //        lfw::Entity entity = ecsManager.requestCreateGeometric("");
        //        entity.addComponent<Behavior00>();
        //        entity.getGeometric()->setParent(this->getEntity().getGeometric());
        //    }
        //    count_ += 64;
        //}
    }

    void Scene00::onDestroy()
    {
        lsound::Context& context = lsound::Context::getInstance();
        context.destroyUserPlayer(bgmPlayer_);
        bgmPlayer_ = NULL;
    }
}
