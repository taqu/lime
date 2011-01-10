#ifndef INC_LGRAPHICS_DX11_ENUMERATIONS_H__
#define INC_LGRAPHICS_DX11_ENUMERATIONS_H__
/**
@file Enumerations.h
@author t-sakai
@date 2010/02/14 create
*/
#include "../lgraphicsAPIInclude.h"

namespace lgraphics
{
    // Direct3D9と共通
    enum PresentInterval
    {
        PresentInterval_Default = 1,
        PresentInterval_One = 1,
        PresentInterval_Two = 2,
        PresentInterval_Three = 3,
        PresentInterval_Four = 4,
        PresentInterval_Immediate = 1,
    };

    enum SwapEffect
    {
        SwapEffect_Discard = DXGI_SWAP_EFFECT_DISCARD,
        SwapEffect_Sequencial = DXGI_SWAP_EFFECT_SEQUENTIAL,
    };

    enum DriverType
    {
        DriverType_Unknown = D3D_DRIVER_TYPE_UNKNOWN,
        DriverType_Hardware = D3D_DRIVER_TYPE_HARDWARE,
        DriverType_Reference = D3D_DRIVER_TYPE_REFERENCE,
        DriverType_Null = D3D_DRIVER_TYPE_NULL,
        DriverType_Software = D3D_DRIVER_TYPE_SOFTWARE,
        DriverType_Warp = D3D_DRIVER_TYPE_WARP,
    };

    enum FutureLevel
    {
        FutureLevel_9_1 = D3D_FEATURE_LEVEL_9_1,
        FutureLevel_9_2 = D3D_FEATURE_LEVEL_9_2,
        FutureLevel_9_3 = D3D_FEATURE_LEVEL_9_3,
        FutureLevel_10_0 = D3D_FEATURE_LEVEL_10_0,
        FutureLevel_10_1 = D3D_FEATURE_LEVEL_10_1,
        FutureLevel_11_0 = D3D_FEATURE_LEVEL_11_0,
    };

#if 0
    //--------------------------------------------------------
    enum StateType
    {
        StateType_All,
        StateType_Pixel,
        StateType_Vertex,
        StateType_Num
    };

    //--------------------------------------------------------
    enum Pool
    {
        Pool_Default = D3DPOOL_DEFAULT,
        Pool_Managed = D3DPOOL_MANAGED,
        Pool_SystemMem = D3DPOOL_SYSTEMMEM,
        Pool_Scratch = D3DPOOL_SCRATCH,
        Pool_UserMem,
    };

    //--------------------------------------------------------
    enum Usage
    {
        Usage_None = 0,
        Usage_AutoGenMipMap = D3DUSAGE_AUTOGENMIPMAP,
        Usage_DepthStenceil = D3DUSAGE_DEPTHSTENCIL,
        Usage_Displacement = D3DUSAGE_DMAP,
        Usage_DoNotClip = D3DUSAGE_DONOTCLIP,
        Usage_Dynamic = D3DUSAGE_DYNAMIC,
        Usage_RenderTarget = D3DUSAGE_RENDERTARGET,
    };

    //--------------------------------------------------------
    enum Lock
    {
        Lock_None = 0,
        Lock_Discard = D3DLOCK_DISCARD,
        Lock_NoOverwrite = D3DLOCK_NOOVERWRITE,
    };

    //--------------------------------------------------------
    enum DeclType
    {
        DeclType_Float1 =0,
        DeclType_Float2,
        DeclType_Float3,
        DeclType_Float4,
        DeclType_Color,
        DeclType_UB4,
        DeclType_Short2,
        DeclType_Short4,
        DeclType_UB4N,
        DeclType_Short2N,
        DeclType_Short4N,
        DeclType_UShort2N,
        DeclType_UShort4N,
        DeclType_UDec3N,
        DeclType_Dec3N,
        DeclType_Float16_2,
        DeclType_Float16_4,
        DeclType_UnUsed,
    };

    //--------------------------------------------------------
    enum DeclUsage
    {
        DeclUsage_Position = 0,
        DeclUsage_BlendWeight,
        DeclUsage_BlendIndicies,
        DeclUsage_Normal,
        DeclUsage_PSize,
        DeclUsage_Texcoord,
        DeclUsage_Tangent,
        DeclUsage_Binormal,
        DeclUsage_TessFactor,
        DeclUsage_PositionT,
        DeclUsage_Color,
        DeclUsage_Fog,
        DeclUsage_Depth,
        DeclUsage_Sample,
    };

    //--------------------------------------------------------
    enum PrimitiveType
    {
        Primitive_PointList = D3DPT_POINTLIST,
        Primitive_LineList = D3DPT_LINELIST,
        Primitive_LineStrip = D3DPT_LINESTRIP,
        Primitive_TriangleList = D3DPT_TRIANGLELIST,
        Primitive_TriangleStrip = D3DPT_TRIANGLESTRIP,
        //Primitive_TriangleFan = D3DPT_TRIANGLEFAN,
    };

#endif
    //--------------------------------------------------------
    /// データフォーマット
    enum DataFormat
    {
        Data_Unknown = DXGI_FORMAT_UNKNOWN,

        Data_R32G32B32A32_TypeLess = DXGI_FORMAT_R32G32B32A32_TYPELESS,
        Data_R32G32B32A32_Float = DXGI_FORMAT_R32G32B32A32_FLOAT,
        Data_R32G32B32A32_UInt = DXGI_FORMAT_R32G32B32A32_UINT,
        Data_R32G32B32A32_SInt = DXGI_FORMAT_R32G32B32A32_SINT,

        Data_R32G32B32_TypeLess = DXGI_FORMAT_R32G32B32_TYPELESS,
        Data_R32G32B32_Float = DXGI_FORMAT_R32G32B32_FLOAT,
        Data_R32G32B32_UInt = DXGI_FORMAT_R32G32B32_UINT,
        Data_R32G32B32_SInt = DXGI_FORMAT_R32G32B32_SINT,

        Data_R16G16B16_TypeLess = DXGI_FORMAT_R16G16B16A16_TYPELESS,
        Data_R16G16B16_Float = DXGI_FORMAT_R16G16B16A16_FLOAT,
        Data_R16G16B16_UNorm = DXGI_FORMAT_R16G16B16A16_UNORM,
        Data_R16G16B16_UInt = DXGI_FORMAT_R16G16B16A16_UINT,
        Data_R16G16B16_SNorm = DXGI_FORMAT_R16G16B16A16_SNORM,
        Data_R16G16B16_SInt = DXGI_FORMAT_R16G16B16A16_SINT,

        Data_R32G32_TypeLess = DXGI_FORMAT_R32G32_TYPELESS,
        Data_R32G32_Float = DXGI_FORMAT_R32G32_FLOAT,
        Data_R32G32_UInt = DXGI_FORMAT_R32G32_UINT,
        Data_R32G32_SInt = DXGI_FORMAT_R32G32_SINT,

        Data_R32G8X24_TypeLess = DXGI_FORMAT_R32G8X24_TYPELESS,
        Data_D32_Float_S8X24_UInt = DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
        Data_R32_Float_X8X24_TypeLess = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
        Data_X32_TypeLess_G8X24_UInt = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,

        Data_R10G10B10A2_TypeLess = DXGI_FORMAT_R10G10B10A2_TYPELESS,
        Data_R10G10B10A2_UNorm = DXGI_FORMAT_R10G10B10A2_UNORM,
        Data_R10G10B10A2_UInt = DXGI_FORMAT_R10G10B10A2_UINT,

        Data_R11G11B10_Float = DXGI_FORMAT_R11G11B10_FLOAT,

        Data_R8G8B8A8_TypeLess = DXGI_FORMAT_R8G8B8A8_TYPELESS,
        Data_R8G8B8A8_UNorm = DXGI_FORMAT_R8G8B8A8_UNORM,
        Data_R8G8B8A8_UNorm_SRGB = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        Data_R8G8B8A8_UInt = DXGI_FORMAT_R8G8B8A8_UINT,
        Data_R8G8B8A8_SNorm = DXGI_FORMAT_R8G8B8A8_SNORM,
        Data_R8G8B8A8_SInt = DXGI_FORMAT_R8G8B8A8_SINT,

        Data_R16G16_TypeLess = DXGI_FORMAT_R16G16_TYPELESS,
        Data_R16G16_Float = DXGI_FORMAT_R16G16_FLOAT,
        Data_R16G16_UNorm = DXGI_FORMAT_R16G16_UNORM,
        Data_R16G16_UInt = DXGI_FORMAT_R16G16_UINT,
        Data_R16G16_SNorm = DXGI_FORMAT_R16G16_SNORM,
        Data_R16G16_SInt = DXGI_FORMAT_R16G16_SINT,

        Data_R32_TypeLess = DXGI_FORMAT_R32_TYPELESS,
        Data_D32_Float = DXGI_FORMAT_D32_FLOAT,
        Data_R32_Float = DXGI_FORMAT_R32_FLOAT,
        Data_R32_UInt = DXGI_FORMAT_R32_UINT,
        Data_R32_SInt = DXGI_FORMAT_R32_SINT,

        Data_R24G8_TypeLess = DXGI_FORMAT_R24G8_TYPELESS,
        Data_D24_UNorm_S8_UInt = DXGI_FORMAT_D24_UNORM_S8_UINT,
        Data_R24_UNorm_X8_TypeLess = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
        Data_X24_TypeLess_G8_UInt = DXGI_FORMAT_X24_TYPELESS_G8_UINT,

        Data_R8G8_TypeLess = DXGI_FORMAT_R8G8_TYPELESS,
        Data_R8G8_UNorm = DXGI_FORMAT_R8G8_UNORM,
        Data_R8G8_UInt = DXGI_FORMAT_R8G8_UINT,
        Data_R8G8_SNorm = DXGI_FORMAT_R8G8_SNORM,
        Data_R8G8_SInt = DXGI_FORMAT_R8G8_SINT,

        Data_R16_TypeLess = DXGI_FORMAT_R16_TYPELESS,
        Data_R16_Float = DXGI_FORMAT_R16_FLOAT,
        Data_D16_UNorm = DXGI_FORMAT_D16_UNORM,
        Data_R16_UNorm = DXGI_FORMAT_R16_UNORM,
        Data_R16_UInt = DXGI_FORMAT_R16_UINT,
        Data_R16_SNorm = DXGI_FORMAT_R16_SNORM,
        Data_R16_SInt = DXGI_FORMAT_R16_SINT,

        Data_R8_TypeLess = DXGI_FORMAT_R8_TYPELESS,
        Data_R8_UNorm = DXGI_FORMAT_R8_UNORM,
        Data_R8_UInt = DXGI_FORMAT_R8_UINT,
        Data_R8_SNorm = DXGI_FORMAT_R8_SNORM,
        Data_R8_SInt = DXGI_FORMAT_R8_SINT,
        Data_A8_UNorm = DXGI_FORMAT_A8_UNORM,

        Data_R1_UNorm = DXGI_FORMAT_R1_UNORM,

        Data_R9G9B9E5_SharedExp = DXGI_FORMAT_R9G9B9E5_SHAREDEXP,

        Data_R8G8_B8G8_UNorm = DXGI_FORMAT_R8G8_B8G8_UNORM,
        Data_G8R8_G8B8_UNorm = DXGI_FORMAT_G8R8_G8B8_UNORM,

        Data_BC1_TypeLess = DXGI_FORMAT_BC1_TYPELESS,
        Data_BC1_UNorm = DXGI_FORMAT_BC1_UNORM,
        Data_BC1_UNorm_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,

        Data_BC2_TypeLess = DXGI_FORMAT_BC2_TYPELESS,
        Data_BC2_UNorm = DXGI_FORMAT_BC2_UNORM,
        Data_BC2_UNorm_SRGB = DXGI_FORMAT_BC2_UNORM_SRGB,

        Data_BC3_TypeLess = DXGI_FORMAT_BC3_TYPELESS,
        Data_BC3_UNorm = DXGI_FORMAT_BC3_UNORM,
        Data_BC3_UNorm_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,

        Data_BC4_TypeLess = DXGI_FORMAT_BC4_TYPELESS,
        Data_BC4_UNorm = DXGI_FORMAT_BC4_UNORM,
        Data_BC4_SNorm = DXGI_FORMAT_BC4_SNORM,

        Data_BC5_TypeLess = DXGI_FORMAT_BC5_TYPELESS,
        Data_BC5_UNorm = DXGI_FORMAT_BC5_UNORM,
        Data_BC5_SNorm = DXGI_FORMAT_BC5_SNORM,

        Data_B5G6R5_UNorm = DXGI_FORMAT_B5G6R5_UNORM,
        Data_B5G5R5A1_UNorm = DXGI_FORMAT_B5G5R5A1_UNORM,

        Data_B8G8R8A8_UNorm = DXGI_FORMAT_B8G8R8A8_UNORM,
        Data_B8G8R8X8_UNorm = DXGI_FORMAT_B8G8R8X8_UNORM,

        Data_R10G10B10_XR_Bias_A2_UNorm = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,

        Data_B8G8R8A8_TypeLess = DXGI_FORMAT_B8G8R8A8_TYPELESS,
        Data_B8G8R8A8_UNorm_SRGB = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
        Data_B8G8R8X8_TypeLess = DXGI_FORMAT_B8G8R8X8_TYPELESS,
        Data_B8G8R8X8_UNorm_SRGB = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,

        Data_BC6H_TypeLess = DXGI_FORMAT_BC6H_TYPELESS,
        Data_BC6H_UF16 = DXGI_FORMAT_BC6H_UF16,
        Data_BC6H_SF16 = DXGI_FORMAT_BC6H_SF16,

        Data_BC7_TypeLess = DXGI_FORMAT_BC7_TYPELESS,
        Data_BC7_UNorm = DXGI_FORMAT_BC7_UNORM,
        Data_BC7_UNorm_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,

        Data_Force_UInt = DXGI_FORMAT_FORCE_UINT,
    };

    enum ScanlineOrder
    {
        ScanlineOrder_Unspecified = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED ,
        ScanlineOrder_Progressive = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ,
        ScanlineOrder_UpperFirst  = DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST ,
        ScanlineOrder_LowerFirst  = DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST ,
    };

    enum Scaling
    {
        Scaling_Unspecified = DXGI_MODE_SCALING_UNSPECIFIED,
        Scaling_Centered    = DXGI_MODE_SCALING_CENTERED,
        Scaling_Stretched   = DXGI_MODE_SCALING_STRETCHED,
    };

    enum Usage
    {
        Usage_BackBuffer       = DXGI_USAGE_BACK_BUFFER,
        Usage_DiscardOnPresent = DXGI_USAGE_DISCARD_ON_PRESENT,
        Usage_ReadOnly         = DXGI_USAGE_READ_ONLY,
        Usage_RenderTargetOutput = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        Usage_ShaderInput      = DXGI_USAGE_SHADER_INPUT,
        Usage_Shared           = DXGI_USAGE_SHARED,
        Usage_UnorderedAccess  = DXGI_USAGE_UNORDERED_ACCESS,
    };

    enum SwapChainFlag
    {
        SwapChainFlag_Nonprerotated = DXGI_SWAP_CHAIN_FLAG_NONPREROTATED,
        SwapChainFlag_AllowModeSwitch = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
        SwapChainFlag_GDICompatible = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE,
    };


#if 0


    //--------------------------------------------------------
    enum MutiSampleType
    {
        MutiSample_None = D3DMULTISAMPLE_NONE,
        MutiSample_NonMaskable = D3DMULTISAMPLE_NONMASKABLE,
        MutiSample_2 = D3DMULTISAMPLE_2_SAMPLES,
        MutiSample_3 = D3DMULTISAMPLE_3_SAMPLES,
        MutiSample_4 = D3DMULTISAMPLE_4_SAMPLES,
        MutiSample_5 = D3DMULTISAMPLE_5_SAMPLES,
        MutiSample_6 = D3DMULTISAMPLE_6_SAMPLES,
        MutiSample_7 = D3DMULTISAMPLE_7_SAMPLES,
        MutiSample_8 = D3DMULTISAMPLE_8_SAMPLES,
        MutiSample_9 = D3DMULTISAMPLE_9_SAMPLES,
        MutiSample_10 = D3DMULTISAMPLE_10_SAMPLES,
        MutiSample_11 = D3DMULTISAMPLE_11_SAMPLES,
        MutiSample_12 = D3DMULTISAMPLE_12_SAMPLES,
        MutiSample_13 = D3DMULTISAMPLE_13_SAMPLES,
        MutiSample_14 = D3DMULTISAMPLE_14_SAMPLES,
        MutiSample_15 = D3DMULTISAMPLE_15_SAMPLES,
        MutiSample_16 = D3DMULTISAMPLE_16_SAMPLES,
    };

    //--------------------------------------------------------
    enum TextureType
    {
        TexType_Diffuse =0,
        TexType_Specular,
        TexType_Ambient,
        TexType_Emissive,
        TexType_Height,
        TexType_Normal,
        TexType_Shininess,
        TexType_Opacity,
        TexType_Displacement,
        TexType_LightMap,
        TexType_Reflection,
        TexType_User,
    };

    //--------------------------------------------------------
    enum TextureAddress
    {
        TexAddress_Wrap = D3DTADDRESS_WRAP,
        TexAddress_Mirror = D3DTADDRESS_MIRROR,
        TexAddress_Clamp = D3DTADDRESS_CLAMP,
        TexAddress_Border = D3DTADDRESS_BORDER,
        TexAddress_Mirroronce = D3DTADDRESS_MIRRORONCE,
        TexAddress_None,
    };

    //--------------------------------------------------------
    enum TextureFilterType
    {
        TexFilter_None = D3DTEXF_NONE,
        TexFilter_Point = D3DTEXF_POINT,
        TexFilter_Linear = D3DTEXF_LINEAR,
        TexFilter_Anisotropic = D3DTEXF_ANISOTROPIC,
        TexFilter_PyramidalQuad = D3DTEXF_PYRAMIDALQUAD,
        TexFilter_GaussianQuad = D3DTEXF_GAUSSIANQUAD,
        TexFilter_ConvolutionMono = D3DTEXF_CONVOLUTIONMONO,
        TexFilter_Num,
    };

    //--------------------------------------------------------
    enum CullMode
    {
        CullMode_None = D3DCULL_NONE,
        CullMode_CW = D3DCULL_CW,
        CullMode_CCW = D3DCULL_CCW,
    };

    //--------------------------------------------------------
    enum BlendType
    {
        Blend_Zero = D3DBLEND_ZERO,
        Blend_One = D3DBLEND_ONE,
        Blend_SrcAlpha = D3DBLEND_SRCALPHA,
        Blend_InvSrcAlpha = D3DBLEND_INVSRCALPHA,
        Blend_DestAlpha = D3DBLEND_DESTALPHA,
        Blend_InvDestAlpha = D3DBLEND_INVDESTALPHA,
    };

    //--------------------------------------------------------
    enum CmpFunc
    {
        Cmp_Never = D3DCMP_NEVER,
        Cmp_Less = D3DCMP_LESS,
        Cmp_Equal = D3DCMP_EQUAL,
        Cmp_LessEqual = D3DCMP_LESSEQUAL,
        Cmp_Greater = D3DCMP_GREATER,
        Cmp_NotEqual = D3DCMP_NOTEQUAL,
        Cmp_GreaterEqual = D3DCMP_GREATEREQUAL,
        Cmp_Always = D3DCMP_ALWAYS,
    };

#endif
}


#endif //INC_LGRAPHICS_DX11_ENUMERATIONS_H__
