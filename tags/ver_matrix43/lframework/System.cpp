/**
@file System.cpp
@author t-sakai
@date 2010/11/28 create
*/
#include "System.h"
#include <lcore/lcore.h>
#include <lfont/lfont.h>

#include "render/RenderingSystem.h"
#include "scene/ShaderManager.h"
#include "anim/AnimationSystem.h"
#include "text/ASCIIRender.h"
#include "debug/DebugPrimitive.h"

namespace lframework
{
    System::InitParam::InitParam(const char* debugFont, u32 debugFontSize)
        :debugFont_(debugFont)
        ,debugFontSize_(debugFontSize)
    {}


    lrender::RenderingSystem* System::renderingSystem_ = NULL;
    lanim::AnimationSystem* System::animationSystem_ = NULL;
    ASCIIRender* System::debugAsciiRender_ = NULL;

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

//#if defined(_DEBUG)
#if 0
        if(sysParam.debugFont_){
            lfont::FontSystem::initialize();

            debugAsciiRender_ = LIME_NEW ASCIIRender;
            bool ret = ASCIIRender::create(
                *debugAsciiRender_,
                sysParam.debugFont_,
                sysParam.debugFontSize_,
                sysParam.debugFontSize_,
                256,
                lgraphics::Pool_Default);

            if(false == ret){
                LIME_DELETE(debugAsciiRender_);
            }
            lfont::FontSystem::terminate();
        }
#endif
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

        LIME_DELETE(debugAsciiRender_);

        lscene::ShaderManager::terminate();

    }

    void System::debugPrint(f32 , f32 , const char* , ...)
    {
//#if defined(_DEBUG)
#if 0
        if(debugAsciiRender_){
            LASSERT(format != NULL);
            va_list arg;
            va_start(arg, format);

            static const s32 BuffSize = 128;
            char str[BuffSize];
            int count = vsnprintf(str, BuffSize-1, format, arg);
            va_end(arg);

            if(count<0){
                count = BuffSize-1;
            }

            debugAsciiRender_->addString(x, y, 0.0f, lmath::Color(0xFFFFFFFFU), str, count);
        }
#endif
    }

    void System::drawDebugString()
    {
//#if defined(_DEBUG)
#if 0
        if(debugAsciiRender_){
            debugAsciiRender_->draw();
            debugAsciiRender_->clear();
        }
#endif
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
