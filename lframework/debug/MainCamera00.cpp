/**
@file MainCamera00.cpp
@author t-sakai
@date 2016/12/09 create
*/
#include "MainCamera00.h"
#include <lframework/render/graph/RenderGraph.h>
#include "RenderPassProcedural.h"
#include "RenderPassTerrain.h"

namespace debug
{
    MainCamera00::MainCamera00()
        :renderPass_(NULL)
    {

    }

    MainCamera00::~MainCamera00()
    {

    }

    void MainCamera00::onCreate()
    {
    }

    void MainCamera00::onStart()
    {
        ComponentCamera::onStart();
        lcore::Log("MainCamera00::onStart");
    }

    void MainCamera00::update()
    {
        lcore::Log("MainCamera00::update");
    }

    void MainCamera00::onDestroy()
    {
        lcore::Log("MainCamera00::onDestroy");
        ComponentCamera::onDestroy();
    }

    void MainCamera00::resetRenderPasses()
    {
        ComponentCamera::resetRenderPasses();
        lfw::s32 passIndex;

        passIndex = findRenderPass(lfw::graph::RenderPassID_GBuffer);
        RenderPassTerrain* renderPassTerrain = LNEW debug::RenderPassTerrain;
        renderPassTerrain->set(debug::RenderPassTerrain::Type_Land);
        reinterpret_cast<lfw::graph::RenderPassGBuffer*>(getRenderPass(passIndex))->addSubPass(renderPassTerrain);

        passIndex = findRenderPass(lfw::graph::RenderPassID_Transparent);
        renderPass_ = LNEW debug::RenderPassProcedural;
        renderPass_->set(debug::RenderPassProcedural::Type_Space);
        addRenderPass(passIndex, renderPass_);

    }
}
