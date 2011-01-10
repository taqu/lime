#ifndef INC_LGRAPHICS_DX9_INITPARAM_H__
#define INC_LGRAPHICS_DX9_INITPARAM_H__
/**
@file InitParam.h
@author t-sakai
@date 2010/03/24 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

namespace lgraphics
{
    struct InitParam
    {
        InitParam()
            :type_(DriverType_Hardware),
            backBufferWidth_(1),
            backBufferHeight_(1),
            interval_(PresentInterval_One),
            windowed_(true),
            swapEffect_(SwapEffect_Discard),
            refreshRate_(60),
            windowHandle_(NULL)
        {
        }
        DriverType type_;
        u32 backBufferWidth_;
        u32 backBufferHeight_;
        PresentInterval interval_;
        bool windowed_;
        SwapEffect swapEffect_;
        u32 refreshRate_;
        HWND__ *windowHandle_;
    };
}
#endif //INC_LGRAPHICS_DX9_INITPARAM_H__
