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
    const u8 shaderVS_LightDepth[] =
    {
#include "../../shader/LightDepthVS.txt"
    };

    const u8 shaderVS_LightDepthSkinning[] =
    {
#include "../../shader/LightDepthSkinningVS.txt"
    };

    //-------------------------------------------
    const u8 shaderVS_Deferred[] =
    {
#include "../../shader/DeferredVS.txt"
    };

    const u8 shaderVS_DeferredSkinning[] =
    {
#include "../../shader/DeferredSkinningVS.txt"
    };

    const u8 shaderVS_DeferredUV[] =
    {
#include "../../shader/DeferredUVVS.txt"
    };

    const u8 shaderVS_DeferredUVSkinning[] =
    {
#include "../../shader/DeferredUVSkinningVS.txt"
    };

    const u8 shaderVS_DeferredNUV[] =
    {
#include "../../shader/DeferredNUVVS.txt"
    };

    const u8 shaderVS_DeferredNUVSkinning[] =
    {
#include "../../shader/DeferredNUVSkinningVS.txt"
    };

    //-------------------------------------------
    const u8 shaderVS_Forward[] =
    {
#include "../../shader/ForwardVS.txt"
    };

    const u8 shaderVS_ForwardShadow[] =
    {
#include "../../shader/ForwardShadowVS.txt"
    };

    const u8 shaderVS_ForwardSkinning[] =
    {
#include "../../shader/ForwardSkinningVS.txt"
    };

    const u8 shaderVS_ForwardSkinningShadow[] =
    {
#include "../../shader/ForwardSkinningShadowVS.txt"
    };

    const u8 shaderVS_ForwardUV[] =
    {
#include "../../shader/ForwardUVVS.txt"
    };

    const u8 shaderVS_ForwardUVShadow[] =
    {
#include "../../shader/ForwardUVShadowVS.txt"
    };

    const u8 shaderVS_ForwardUVSkinning[] =
    {
#include "../../shader/ForwardUVSkinningVS.txt"
    };

    const u8 shaderVS_ForwardUVSkinningShadow[] =
    {
#include "../../shader/ForwardUVSkinningShadowVS.txt"
    };

    const u8 shaderVS_ForwardNUV[] =
    {
#include "../../shader/ForwardNUVVS.txt"
    };

    const u8 shaderVS_ForwardNUVShadow[] =
    {
#include "../../shader/ForwardNUVShadowVS.txt"
    };

    const u8 shaderVS_ForwardNUVSkinning[] =
    {
#include "../../shader/ForwardNUVSkinningVS.txt"
    };

    const u8 shaderVS_ForwardNUVSkinningShadow[] =
    {
#include "../../shader/ForwardNUVSkinningShadowVS.txt"
    };

    // GS
    //-------------------------------------------------------
    const u8 shaderGS_LighDepth[] =
    {
#include "../../shader/LightDepthGS.txt"
    };

    // PS
    //-------------------------------------------------------
    const u8 shaderPS_Deferred[] =
    {
#include "../../shader/DeferredPS.txt"
    };

    const u8 shaderPS_DeferredUV[] =
    {
#include "../../shader/DeferredUVPS.txt"
    };

    const u8 shaderPS_DeferredTexC[] =
    {
#include "../../shader/DeferredTexCPS.txt"
    };

    const u8 shaderPS_DeferredTexCN[] =
    {
#include "../../shader/DeferredTexCNPS.txt"
    };

    //-------------------------------------------
    const u8 shaderPS_Forward[] =
    {
#include "../../shader/ForwardPS.txt"
    };

    const u8 shaderPS_ForwardShadow[] =
    {
#include "../../shader/ForwardShadowPS.txt"
    };

    const u8 shaderPS_ForwardUV[] =
    {
#include "../../shader/ForwardUVPS.txt"
    };

    const u8 shaderPS_ForwardUVShadow[] =
    {
#include "../../shader/ForwardUVShadowPS.txt"
    };

    const u8 shaderPS_ForwardTexC[] =
    {
#include "../../shader/ForwardTexCPS.txt"
    };

    const u8 shaderPS_ForwardTexCShadow[] =
    {
#include "../../shader/ForwardTexCShadowPS.txt"
    };

    const u8 shaderPS_ForwardTexCN[] =
    {
#include "../../shader/ForwardTexCNPS.txt"
    };

    const u8 shaderPS_ForwardTexCNShadow[] =
    {
#include "../../shader/ForwardTexCNShadowPS.txt"
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
        shaderVS_LightDepth,
        shaderVS_LightDepthSkinning,

        shaderVS_Deferred,
        shaderVS_DeferredSkinning,
        shaderVS_DeferredUV,
        shaderVS_DeferredUVSkinning,
        shaderVS_DeferredNUV,
        shaderVS_DeferredNUVSkinning,

        shaderVS_Forward,
        shaderVS_ForwardShadow,
        shaderVS_ForwardSkinning,
        shaderVS_ForwardSkinningShadow,
        shaderVS_ForwardUV,
        shaderVS_ForwardUVShadow,
        shaderVS_ForwardUVSkinning,
        shaderVS_ForwardUVSkinningShadow,
        shaderVS_ForwardNUV,
        shaderVS_ForwardNUVShadow,
        shaderVS_ForwardNUVSkinning,
        shaderVS_ForwardNUVSkinningShadow,
    };

    const s32 DefaultShaderCompiledBytes::sizeVS_[] =
    {
        sizeof(shaderVS_LightDepth),
        sizeof(shaderVS_LightDepthSkinning),

        sizeof(shaderVS_Deferred),
        sizeof(shaderVS_DeferredSkinning),
        sizeof(shaderVS_DeferredUV),
        sizeof(shaderVS_DeferredUVSkinning),
        sizeof(shaderVS_DeferredNUV),
        sizeof(shaderVS_DeferredNUVSkinning),

        sizeof(shaderVS_Forward),
        sizeof(shaderVS_ForwardShadow),
        sizeof(shaderVS_ForwardSkinning),
        sizeof(shaderVS_ForwardSkinningShadow),
        sizeof(shaderVS_ForwardUV),
        sizeof(shaderVS_ForwardUVShadow),
        sizeof(shaderVS_ForwardUVSkinning),
        sizeof(shaderVS_ForwardUVSkinningShadow),
        sizeof(shaderVS_ForwardNUV),
        sizeof(shaderVS_ForwardNUVShadow),
        sizeof(shaderVS_ForwardNUVSkinning),
        sizeof(shaderVS_ForwardNUVSkinningShadow),
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
        shaderGS_LighDepth,
    };

    const s32 DefaultShaderCompiledBytes::sizeGS_[] =
    {
        sizeof(shaderGS_LighDepth),
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
        shaderPS_Deferred,
        shaderPS_DeferredUV,
        shaderPS_DeferredTexC,
        shaderPS_DeferredTexCN,

        shaderPS_Forward,
        shaderPS_ForwardShadow,
        shaderPS_ForwardUV,
        shaderPS_ForwardUVShadow,
        shaderPS_ForwardTexC,
        shaderPS_ForwardTexCShadow,
        shaderPS_ForwardTexCN,
        shaderPS_ForwardTexCNShadow,
    };

    const s32 DefaultShaderCompiledBytes::sizePS_[] =
    {
        sizeof(shaderPS_Deferred),
        sizeof(shaderPS_DeferredUV),
        sizeof(shaderPS_DeferredTexC),
        sizeof(shaderPS_DeferredTexCN),

        sizeof(shaderPS_Forward),
        sizeof(shaderPS_ForwardShadow),
        sizeof(shaderPS_ForwardUV),
        sizeof(shaderPS_ForwardUVShadow),
        sizeof(shaderPS_ForwardTexC),
        sizeof(shaderPS_ForwardTexCShadow),
        sizeof(shaderPS_ForwardTexCN),
        sizeof(shaderPS_ForwardTexCNShadow),
    };
}
