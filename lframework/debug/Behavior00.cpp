/**
@file Behavior00.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "Behavior00.h"

#include <lframework/System.h>
#include <lframework/Timer.h>
#include <lframework/ecs/Entity.h>
#include <lframework/ecs/ComponentCamera.h>
#include <lframework/ecs/ComponentMeshRenderer.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceModel.h>

namespace debug
{
    Behavior00::Behavior00()
        :time_(0.0f)
    {

    }

    Behavior00::~Behavior00()
    {

    }

    void Behavior00::onCreate()
    {
        lcore::Log("Behavior00::onCreate");
        lfw::s32 setID = lfw::Resources::DefaultSet;
        lfw::Resource::pointer resource = lfw::System::getResources().load(setID, "sphere_bin.lm", lfw::ResourceType_Model);

        mesh_ = getEntity().addComponent<lfw::ComponentMeshRenderer>();
        lfw::Model::pointer model = resource->cast<lfw::ResourceModel>()->clone();
        mesh_->setMesh(model);
    }

    void Behavior00::onStart()
    {
        lcore::Log("Behavior00::onStart");
    }

    void Behavior00::update()
    {
        lfw::Timer& timer = lfw::System::getTimer();
        time_ += timer.getDeltaTime();
        if(1.0f<time_){
            time_ -= 1.0f;
        }
        lfw::f32 rad = PI2*time_;
        getEntity().getGeometric()->getRotation().setRotateZ(rad);
    }

    void Behavior00::onDestroy()
    {
        lcore::Log("Behavior00::onDestroy");
    }
}
