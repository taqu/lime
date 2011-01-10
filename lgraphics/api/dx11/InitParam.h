#ifndef INC_LGRAPHICS_DX11_INITPARAM_H__
#define INC_LGRAPHICS_DX11_INITPARAM_H__
/**
@file InitParam.h
@author t-sakai
@date 2010/03/24 create
*/
#include "lgraphicscore.h"

namespace lgraphics
{
    struct InitParam
    {
        InitParam()
            :
            level_(FutureLevel_11_0),
            type_(DriverType_Hardware),
            backBufferWidth_(1),
            backBufferHeight_(1),
            isClearColor_(false),
            isClearDepth_(true),
            isClearStencil_(true),
            interval_(PresentInterval_One),
            windowed_(true),
            swapEffect_(SwapEffect_Discard),
            clearColor_(0xFF808080),
            clearDepth_(1.0f),
            clearStencil_(0),
            refreshRate_(60),
            windowHandle_(NULL)
        {
        }

        FutureLevel level_;
        DriverType type_;
        u32 backBufferWidth_;
        u32 backBufferHeight_;
        bool isClearColor_;
        bool isClearDepth_;
        bool isClearStencil_;
        PresentInterval interval_;
        bool windowed_;
        SwapEffect swapEffect_;
        u32 clearColor_;
        f32 clearDepth_;
        u32 clearStencil_;
        u32 refreshRate_;
        HWND__ *windowHandle_;
    };
}

#endif //INC_LGRAPHICS_DX11_INITPARAM_H__
