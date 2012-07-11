/**
@file System.cpp
@author t-sakai
@date 2010/11/28 create
*/
#include "System.h"
#include <lcore/lcore.h>

#include "render/RenderingSystem.h"
#include "scene/ShaderManager.h"
#include <lframework/anim/AnimationSystem.h>
#include "debug/DebugPrimitive.h"

namespace lframework
{
    System::InitParam::InitParam(const char* debugFont, u32 debugFontSize)
        :debugFont_(debugFont)
        ,debugFontSize_(debugFontSize)
    {}


    lrender::RenderingSystem* System::renderingSystem_ = NULL;
    lanim::AnimationSystem* System::animationSystem_ = NULL;

#if defined(_DEBUG)
    debug::DebugPrimitive* System::debugPrimitive_ = NULL;
#endif

    void System::initialize(const InitParam&, const lanim::InitParam& animParam)
    {
        lscene::ShaderManager::initialize(64);

        if(renderingSystem_ == NULL){
            renderingSystem_ = LIME_NEW lrender::RenderingSystem;
            renderingSystem_->initialize();
        }

        if(animationSystem_ == NULL){
            animationSystem_ = LIME_NEW lanim::AnimationSystem;
            animationSystem_->initialize(animParam);
        }

#if defined(_DEBUG)
        if(debugPrimitive_ == NULL){
            debugPrimitive_ = LIME_NEW debug::DebugPrimitive;
            debugPrimitive_->initialize(1024);
            //debugPrimitive_->attach();
        }
#endif
    }

    void System::terminate()
    {
#if defined(_DEBUG)
        if(debugPrimitive_){
            debugPrimitive_->terminate();
            LIME_DELETE(debugPrimitive_);
        }
#endif

        if(animationSystem_){
            animationSystem_->terminate();
            LIME_DELETE(animationSystem_);
        }

        if(renderingSystem_){
            renderingSystem_->terminate();
            LIME_DELETE(renderingSystem_);
        }

        lscene::ShaderManager::terminate();

    }


#if defined(_DEBUG)
    void System::debugDraw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color)
    {
        debugPrimitive_->draw(v0, v1, v2, color);
    }

    void System::debugDrawClear()
    {
        debugPrimitive_->clear();
    }

    void System::attachDebugDraw()
    {
        debugPrimitive_->attach();
    }

    void System::detachDebugDraw()
    {
        debugPrimitive_->detach();
    }
#endif
}
