/**
@file Application.cpp
@author t-sakai
@date 2010/02/14 create
*/
#include "Application.h"
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>

namespace lframework
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    bool Application::create(lgraphics::InitParam& param, const char* title)
    {
        //ウィンドウサイズ、ビューポートサイズはバックバッファと同じにする。

        //ウィンドウ作成。デバイスのパラメータにウィンドウハンドルを渡す
        lgraphics::Window::InitParam windowParam;
        windowParam.width_ = param.backBufferWidth_;
        windowParam.height_ = param.backBufferHeight_;
        windowParam.setTitle(title, lcore::strlen(title)+1);
        bool ret = window_.initialize(windowParam, true);
        if(false == ret){
            return false;
        }

        param.windowHandle_ = window_.getHandle().hWnd_;

        if(false == lgraphics::Graphics::initialize(param)){
            window_.terminate();
            return false;
        }
        lgraphics::Graphics::getDevice().setViewport(0, 0, param.backBufferWidth_, param.backBufferHeight_);
        return true;
    }

    // 処理ループ実行
    void Application::run()
    {
        initialize();

        for(;;){
            if(false == window_.peekMessage()){
                break;
            }
            update();

            //アニメーションシステム更新
            //コリジョンシステム更新
            //ポストコリジョン更新
            //描画
        }

        terminate();

        lgraphics::Graphics::terminate();
        window_.terminate();
    }

}
