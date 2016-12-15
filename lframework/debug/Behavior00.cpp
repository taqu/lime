/**
@file Behavior00.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "Behavior00.h"
#include <lframework/ecs/Entity.h>
#include <lframework/ecs/ComponentCamera.h>
#include <lframework/ecs/ComponentMeshRenderer.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceModel.h>

namespace debug
{
    Behavior00::Behavior00()
    {

    }

    Behavior00::~Behavior00()
    {

    }

    void Behavior00::onCreate()
    {
        lcore::Log("Behavior00::onCreate");
        lfw::s32 setID = lfw::Resources::DefaultSet;
        lfw::Resource::pointer resource = lfw::Resources::getInstance().load(setID, "sphere_bin.lm", lfw::ResourceType_Model);

        mesh_ = getEntity().addComponent<lfw::ComponentMeshRenderer>();
        mesh_->setMesh(resource->cast<lfw::ResourceModel>()->get());
    }

    void Behavior00::onStart()
    {
        lcore::Log("Behavior00::onStart");
    }

    void Behavior00::update()
    {
        //lcore::Log("Behavior00::update");
    }

    void Behavior00::onDestroy()
    {
        lcore::Log("Behavior00::onDestroy");
    }
}
