#ifndef INC_LGRAPHICS_DX11_INITPARAM_H__
#define INC_LGRAPHICS_DX11_INITPARAM_H__
/**
@file InitParam.h
@author t-sakai
@date 2010/03/24 create
*/
#include "lgraphics.h"
#include "Enumerations.h"

namespace lgfx
{
    struct InitParam
    {
        enum HardwareLevel
        {
            HardwareLevel_11 = 1,
            HardwareLevel_10_1,
            HardwareLevel_10,
        };

        enum Flag
        {
            Flag_DepthStencilShaderResource = (0x01<<0),
        };

        InitParam()
            :type_(DriverType_Hardware)
            ,backBufferWidth_(800)
            ,backBufferHeight_(600)
            ,format_(Data_R8G8B8A8_UNorm_SRGB)
            ,depthStencilFormat_(Data_D24_UNorm_S8_UInt)
            ,shadowMapFormat_(Data_D32_Float)
            ,refreshRate_(60)
            ,windowHandle_(NULL)
            ,windowed_(1)
            ,allowSoftwareDevice_(0)
            ,interval_(PresentInterval_One)
            ,swapEffect_(DXGISwapEffect_Discard)
            ,supportHardwareLevel_(HardwareLevel_11)
            ,deviceFlags_(0)
            ,flags_(0)
            ,maximumFrameLatency_(3)
            //,depthBias_(-0.0008f)
            //,slopeScaledDepthBias_(1.0f)
            //,depthBiasClamp_(0.005f)
            ,depthBias_(0.0f)
            ,slopeScaledDepthBias_(0.0f)
            ,depthBiasClamp_(0.0f)
        {
        }

        DriverType type_;
        u32 backBufferWidth_;
        u32 backBufferHeight_;
        u32 format_;
        u32 depthStencilFormat_;
        u32 shadowMapFormat_;
        u32 refreshRate_;
        HWND__ *windowHandle_;
        s16 windowed_;
        s16 allowSoftwareDevice_;
        s32 supportHardwareLevel_;
        u32 deviceFlags_;
        u32 flags_;
        u32 maximumFrameLatency_;
        f32 depthBias_;
        f32 slopeScaledDepthBias_;
        f32 depthBiasClamp_;
        PresentInterval interval_;
        DXGISwapEffect swapEffect_;
    };
}

#endif //INC_LGRAPHICS_DX11_INITPARAM_H__
