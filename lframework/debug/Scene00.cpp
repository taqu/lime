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
#include <lframework/Application.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/ecs/ComponentLight.h>
#include "MainCamera00.h"
#include "Behavior00.h"

namespace debug
{
    Scene00::Scene00()
    {

    }

    Scene00::~Scene00()
    {

    }

    void Scene00::onCreate()
    {
        lfw::Application& application = lfw::Application::getInstance();

        mainLight00Entity_ = application.getECSManager().requestCreateGeometric("MainLight00");
        lfw::ComponentLight* mainLight00 = mainLight00Entity_.addComponent<lfw::ComponentLight>();
        mainLight00Entity_.getGeometric()->getRotation().lookAt(normalize(lmath::Vector4(0.2f, -1.0f, 0.0f, 0.0f)));

        mainCamera00Entity_ = application.getECSManager().requestCreateGeometric("MainCamera00");
        debug::MainCamera00* mainCamera00 = mainCamera00Entity_.addComponent<debug::MainCamera00>();
        mainCamera00->initializePerspective(60.0f, 0.01f, 100.0f, true);
        mainCamera00->initializeDeferred(800, 600);

        mainCamera00Entity_.getGeometric()->setPosition(lmath::Vector4(0.0f, 0.0f, -10.0f, 0.0f));

        behavior00Entity_ = application.getECSManager().requestCreateGeometric("Behavior00");
        behavior00Entity_.addComponent<debug::Behavior00>();
    }

    void Scene00::onStart()
    {
    }

    void Scene00::update()
    {
    }

    void Scene00::onDestroy()
    {
    }
}
