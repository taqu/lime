/**
@file Text00.cpp
@author t-sakai
@date 2016/12/25 create
*/
#include "Text00.h"
#include <lcore/Random.h>
#include <lframework/resource/Resources.h>
#include <lframework/resource/ResourceTexture2D.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentLogicalManager.h>
#include <lframework/ecs/ComponentGeometricManager.h>

namespace debug
{
    Text00::Text00()
    {

    }

    Text00::~Text00()
    {

    }

    void Text00::onCreate()
    {
        ComponentFontRenderer::onCreate();
        setFont(0);
        setLeft(400.0f);
        getText().assign("testjT\nline2");
        updateText();
    }

    void Text00::onStart()
    {
    }

    void Text00::update()
    {
        lcore::String& text = getText();
        text.clear();

        text.assign("[Logical]\n");
        lfw::ComponentLogicalManager* logicalManager = lfw::Application::getInstance().getECSManager().getComponentManager<lfw::ComponentLogicalManager>();
        logicalManager->traverse(traverse, this);

        text.append("[Geometric]\n");
        lfw::ComponentGeometricManager* geometricManager = lfw::Application::getInstance().getECSManager().getComponentManager<lfw::ComponentGeometricManager>();
        geometricManager->traverse(traverse, this);
        updateText();
    }

    void Text00::postUpdate()
    {
    }

    void Text00::onDestroy()
    {
    }

    bool Text00::traverse(lfw::s32 depth, const lfw::ComponentLogical& logical, void* userData)
    {
        Text00* text00 = reinterpret_cast<Text00*>(userData);
        lcore::String& text = text00->getText();
        for(lfw::s32 i=0; i<=depth; ++i){
            text.append(' ');
        }
        text.append(logical.getName().size(), logical.getName().c_str());
        text.append(lcore::CharLF);
        return true;
    }

    bool Text00::traverse(lfw::s32 depth, const lfw::ComponentGeometric& geometric, void* userData)
    {
        Text00* text00 = reinterpret_cast<Text00*>(userData);
        lcore::String& text = text00->getText();
        for(lfw::s32 i=0; i<=depth; ++i){
            text.append(' ');
        }
        text.append(geometric.getName().size(), geometric.getName().c_str());
        text.append(lcore::CharLF);
        return true;
    }
}
