/**
@file MainCamera00.cpp
@author t-sakai
@date 2016/12/09 create
*/
#include "MainCamera00.h"

namespace debug
{
    MainCamera00::MainCamera00()
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
}
