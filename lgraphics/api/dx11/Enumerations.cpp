/**
@file Enumerations.cpp
@author t-sakai
@date 2013/02/27 create
*/
#include "Enumerations.h"

namespace lgraphics
{
    u32 getBitsPerPixel(DataFormat format)
    {
        switch(format)
        {
        case Data_R32G32B32A32_TypeLess:
        case Data_R32G32B32A32_Float:
        case Data_R32G32B32A32_UInt:
        case Data_R32G32B32A32_SInt:
            return 128;

        case Data_R32G32B32_TypeLess:
        case Data_R32G32B32_Float:
        case Data_R32G32B32_UInt:
        case Data_R32G32B32_SInt:
            return 96;

        case Data_R16G16B16A16_TypeLess:
        case Data_R16G16B16A16_Float:
        case Data_R16G16B16A16_UNorm:
        case Data_R16G16B16A16_UInt:
        case Data_R16G16B16A16_SNorm:
        case Data_R16G16B16A16_SInt:

        case Data_R32G32_TypeLess:
        case Data_R32G32_Float:
        case Data_R32G32_UInt:
        case Data_R32G32_SInt:

        case Data_R32G8X24_TypeLess:
        case Data_D32_Float_S8X24_UInt:
        case Data_R32_Float_X8X24_TypeLess:
        case Data_X32_TypeLess_G8X24_UInt:
            return 64;

        case Data_R10G10B10A2_TypeLess:
        case Data_R10G10B10A2_UNorm:
        case Data_R10G10B10A2_UInt:

        case Data_R11G11B10_Float:

        case Data_R8G8B8A8_TypeLess:
        case Data_R8G8B8A8_UNorm:
        case Data_R8G8B8A8_UNorm_SRGB:
        case Data_R8G8B8A8_UInt:
        case Data_R8G8B8A8_SNorm:
        case Data_R8G8B8A8_SInt:

        case Data_R16G16_TypeLess:
        case Data_R16G16_Float:
        case Data_R16G16_UNorm:
        case Data_R16G16_UInt:
        case Data_R16G16_SNorm:
        case Data_R16G16_SInt:

        case Data_R32_TypeLess:
        case Data_D32_Float:
        case Data_R32_Float:
        case Data_R32_UInt:
        case Data_R32_SInt:

        case Data_R24G8_TypeLess:
        case Data_D24_UNorm_S8_UInt:
        case Data_R24_UNorm_X8_TypeLess:
        case Data_X24_TypeLess_G8_UInt:
            return 32;

        case Data_R8G8_TypeLess:
        case Data_R8G8_UNorm:
        case Data_R8G8_UInt:
        case Data_R8G8_SNorm:
        case Data_R8G8_SInt:

        case Data_R16_TypeLess:
        case Data_R16_Float:
        case Data_D16_UNorm:
        case Data_R16_UNorm:
        case Data_R16_UInt:
        case Data_R16_SNorm:
        case Data_R16_SInt:
            return 16;

        case Data_R8_TypeLess:
        case Data_R8_UNorm:
        case Data_R8_UInt:
        case Data_R8_SNorm:
        case Data_R8_SInt:
        case Data_A8_UNorm:
            return 8;

        case Data_R1_UNorm:
            return 1;

        case Data_R9G9B9E5_SharedExp:

        case Data_R8G8_B8G8_UNorm:
        case Data_G8R8_G8B8_UNorm:
            return 32;

        case Data_BC1_TypeLess:
        case Data_BC1_UNorm:
        case Data_BC1_UNorm_SRGB:
            return 4;

        case Data_BC2_TypeLess:
        case Data_BC2_UNorm:
        case Data_BC2_UNorm_SRGB:

        case Data_BC3_TypeLess:
        case Data_BC3_UNorm:
        case Data_BC3_UNorm_SRGB:

        case Data_BC4_TypeLess:
        case Data_BC4_UNorm:
        case Data_BC4_SNorm:

        case Data_BC5_TypeLess:
        case Data_BC5_UNorm:
        case Data_BC5_SNorm:
            return 8;

        case Data_B5G6R5_UNorm:
        case Data_B5G5R5A1_UNorm:
            return 16;

        case Data_B8G8R8A8_UNorm:
        case Data_B8G8R8X8_UNorm:

        case Data_R10G10B10_XR_Bias_A2_UNorm:

        case Data_B8G8R8A8_TypeLess:
        case Data_B8G8R8A8_UNorm_SRGB:
        case Data_B8G8R8X8_TypeLess:
        case Data_B8G8R8X8_UNorm_SRGB:
            return 32;

        case Data_BC6H_TypeLess:
        case Data_BC6H_UF16:
        case Data_BC6H_SF16:

        case Data_BC7_TypeLess:
        case Data_BC7_UNorm:
        case Data_BC7_UNorm_SRGB:
            return 8;
        };
        return 0;
    }

}
