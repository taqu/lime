/**
@file ShaderID.cpp
@author t-sakai
@date 2016/11/12 create
*/
#include "resource/ShaderID.h"

namespace lfw
{
namespace
{
    // VS
    //-------------------------------------------------------
    const u8 shaderVS_Depth[] =
    {
#include "../../shader/DepthVS.txt"
    };

    const u8 shaderVS_DepthSkinning[] =
    {
#include "../../shader/DepthSkinningVS.txt"
    };

    const u8 shaderVS_NormalUV[] =
    {
#include "../../shader/NormalUVVS.txt"
    };

    const u8 shaderVS_NormalNoUV[] =
    {
#include "../../shader/NormalNoUVVS.txt"
    };

    const u8 shaderVS_NormalSkinning[] =
    {
#include "../../shader/NormalSkinningVS.txt"
    };

    const u8 shaderVS_NormalShadow[] =
    {
#include "../../shader/NormalShadowVS.txt"
    };

    const u8 shaderVS_NormalNoUVShadow[] =
    {
#include "../../shader/NormalNoUVShadowVS.txt"
    };

    const u8 shaderVS_NormalSkinningShadow[] =
    {
#include "../../shader/NormalSkinningShadowVS.txt"
    };

    const u8 shaderVS_NormalMap[] =
    {
#include "../../shader/NormalMapVS.txt"
    };

    const u8 shaderVS_NormalMapSkinning[] =
    {
#include "../../shader/NormalMapSkinningVS.txt"
    };

    const u8 shaderVS_NormalMapShadow[] =
    {
#include "../../shader/NormalMapShadowVS.txt"
    };

    const u8 shaderVS_NormalMapSkinningShadow[] =
    {
#include "../../shader/NormalMapSkinningShadowVS.txt"
    };

    const u8 shaderVS_NormalMotion[] =
    {
#include "../../shader/NormalMotionVS.txt"
    };

    const u8 shaderVS_NormalNoUVMotion[] =
    {
#include "../../shader/NormalNoUVMotionVS.txt"
    };

    const u8 shaderVS_NormalSkinningMotion[] =
    {
#include "../../shader/NormalSkinningMotionVS.txt"
    };

    // GS
    //-------------------------------------------------------
    const u8 shaderGS_Depth[] =
    {
#include "../../shader/DepthGS.txt"
    };

    // PS
    //-------------------------------------------------------
    const u8 shaderPS_NormalUV[] =
    {
#include "../../shader/NormalUVPS.txt"
    };

    const u8 shaderPS_NormalNoUV[] =
    {
#include "../../shader/NormalNoUVPS.txt"
    };

    const u8 shaderPS_NormalShadow[] =
    {
#include "../../shader/NormalShadowPS.txt"
    };

    const u8 shaderPS_NormalNoUVShadow[] =
    {
#include "../../shader/NormalNoUVShadowPS.txt"
    };

    const u8 shaderPS_NormalTexC[] =
    {
#include "../../shader/NormalTexCPS.txt"
    };

    const u8 shaderPS_NormalTexCShadow[] =
    {
#include "../../shader/NormalTexCShadowPS.txt"
    };

    const u8 shaderPS_NormalTexCN[] =
    {
#include "../../shader/NormalTexCNPS.txt"
    };

    const u8 shaderPS_NormalTexCNShadow[] =
    {
#include "../../shader/NormalTexCNShadowPS.txt"
    };

    const u8 shaderPS_NormalMotion[] =
    {
#include "../../shader/NormalMotionPS.txt"
    };
}

    //-------------------------------------------------------
    //---
    //--- DefaultShaderCompiledBytes
    //---
    //-------------------------------------------------------
    s32 DefaultShaderCompiledBytes::getSizeVS(s32 id)
    {
        LASSERT(0<=id && id<ShaderVS_Num);
        return sizeVS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesVS(s32 id)
    {
        LASSERT(0<=id && id<ShaderVS_Num);
        return bytesVS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesVS_[] =
    {
        shaderVS_Depth,
        shaderVS_DepthSkinning,
        shaderVS_NormalUV,
        shaderVS_NormalNoUV,
        shaderVS_NormalSkinning,
        shaderVS_NormalShadow,
        shaderVS_NormalNoUVShadow,
        shaderVS_NormalSkinningShadow,
        shaderVS_NormalMap,
        shaderVS_NormalMapSkinning,
        shaderVS_NormalMapShadow,
        shaderVS_NormalMapSkinningShadow,
        shaderVS_NormalMotion,
        shaderVS_NormalNoUVMotion,
        shaderVS_NormalSkinningMotion,
    };

    const s32 DefaultShaderCompiledBytes::sizeVS_[] =
    {
        sizeof(shaderVS_Depth),
        sizeof(shaderVS_DepthSkinning),
        sizeof(shaderVS_NormalUV),
        sizeof(shaderVS_NormalNoUV),
        sizeof(shaderVS_NormalSkinning),
        sizeof(shaderVS_NormalShadow),
        sizeof(shaderVS_NormalNoUVShadow),
        sizeof(shaderVS_NormalSkinningShadow),
        sizeof(shaderVS_NormalMap),
        sizeof(shaderVS_NormalMapSkinning),
        sizeof(shaderVS_NormalMapShadow),
        sizeof(shaderVS_NormalMapSkinningShadow),
        sizeof(shaderVS_NormalMotion),
        sizeof(shaderVS_NormalNoUVMotion),
        sizeof(shaderVS_NormalSkinningMotion),
    };

    s32 DefaultShaderCompiledBytes::getSizeGS(s32 id)
    {
        LASSERT(0<=id && id<ShaderGS_Num);
        return sizeGS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesGS(s32 id)
    {
        LASSERT(0<=id && id<ShaderGS_Num);
        return bytesGS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesGS_[] =
    {
        shaderGS_Depth,
    };

    const s32 DefaultShaderCompiledBytes::sizeGS_[] =
    {
        sizeof(shaderGS_Depth),
    };

    s32 DefaultShaderCompiledBytes::getSizePS(s32 id)
    {
        LASSERT(0<=id && id<ShaderPS_Num);
        return sizePS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesPS(s32 id)
    {
        LASSERT(0<=id && id<ShaderPS_Num);
        return bytesPS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesPS_[] =
    {
        shaderPS_NormalUV,
        shaderPS_NormalNoUV,
        shaderPS_NormalShadow,
        shaderPS_NormalNoUVShadow,
        shaderPS_NormalTexC,
        shaderPS_NormalTexCShadow,
        shaderPS_NormalTexCN,
        shaderPS_NormalTexCNShadow,
        shaderPS_NormalMotion,
    };

    const s32 DefaultShaderCompiledBytes::sizePS_[] =
    {
        sizeof(shaderPS_NormalUV),
        sizeof(shaderPS_NormalNoUV),
        sizeof(shaderPS_NormalShadow),
        sizeof(shaderPS_NormalNoUVShadow),
        sizeof(shaderPS_NormalTexC),
        sizeof(shaderPS_NormalTexCShadow),
        sizeof(shaderPS_NormalTexCN),
        sizeof(shaderPS_NormalTexCNShadow),
        sizeof(shaderPS_NormalMotion),
    };
}
