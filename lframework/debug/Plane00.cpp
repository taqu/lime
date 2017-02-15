/**
@file Plane00.cpp
@author t-sakai
@date 2017/02/14 create
*/
#include "Plane00.h"
#include <lframework/ecs/Entity.h>
#include <lframework/ecs/ComponentCamera.h>
#include <lframework/ecs/ComponentMeshRenderer.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceModel.h>
#include <lframework/resource/PrimitiveGeometry.h>

namespace debug
{
    Plane00::Plane00()
        :model_(NULL)
    {

    }

    Plane00::~Plane00()
    {

    }

    void Plane00::onCreate()
    {
        model_ = lfw::PrimitiveGeometry::createPlane(10.0f);

        mesh_ = getEntity().addComponent<lfw::ComponentMeshRenderer>();
        mesh_->setMesh(model_->get());
    }

    void Plane00::onStart()
    {
    }

    void Plane00::update()
    {
    }

    void Plane00::onDestroy()
    {
        if(NULL != model_){
            intrusive_ptr_release(model_);
            model_ = NULL;
        }
    }
}
