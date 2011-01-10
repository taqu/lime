/**
@file MessageDriven.cpp
@author t-sakai
@date 2010/02/21 create
*/
#include "MessageDriven.h"
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>

namespace lframework
{
    MessageDriven::MessageDriven()
    {
    }

    MessageDriven::~MessageDriven()
    {
    }

    bool MessageDriven::create(lgraphics::Window::InitParam& windowParam, lgraphics::InitParam& param, const char* /*title*/)
    {
        //ウィンドウサイズ、ビューポートサイズはバックバッファと同じにする。

        //ウィンドウ作成。デバイスのパラメータにウィンドウハンドルを渡す
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
    void MessageDriven::run()
    {
        initialize();

        for(;;){
            if(false == window_.getMessage()){
                break;
            }
        }

        terminate();

        lgraphics::Graphics::terminate();
        window_.terminate();
    }

}
