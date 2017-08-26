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

    const u8 shaderVS_DeferredLighting[] =
    {
#include "../../shader/DeferredLightingVS.txt"
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

    const u8 shaderVS_FullQuad[] =
    {
#include "../../shader/FullQuadVS.txt"
    };

    const u8 shaderVS_FullQuadInstanced[] =
    {
#include "../../shader/FullQuadInstancedVS.txt"
    };

    const u8 shaderVS_Sprite2D[] =
    {
#include "../../shader/Sprite2DVS.txt"
    };

    const u8 shaderVS_Particle[] =
    {
#include "../../shader/ParticleVS.txt"
    };

    const u8 shaderVS_VolumeParticle[] =
    {
#include "../../shader/VolumeParticleVS.txt"
    };

    const u8 shaderVS_UI[] =
    {
#include "../../shader/UIVS.txt"
    };

    // GS
    //-------------------------------------------------------
    const u8 shaderGS_LightDepth[] =
    {
#include "../../shader/LightDepthGS.txt"
    };

    const u8 shaderGS_Particle[] =
    {
#include "../../shader/ParticleGS.txt"
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

    const u8 shaderPS_DeferredShadowAccumulating[] =
    {
#include "../../shader/DeferredShadowAccumulatingPS.txt"
    };

    const u8 shaderPS_DeferredLightingDirectional[] =
    {
#include "../../shader/DeferredLightingDirectionalPS.txt"
    };

    const u8 shaderPS_DeferredLightingPoint[] =
    {
#include "../../shader/DeferredLightingPointPS.txt"
    };
    const u8 shaderPS_DeferredLightingSpot[] =
    {
#include "../../shader/DeferredLightingSpotPS.txt"
    };

    const u8 shaderPS_DeferredIntegration[] =
    {
#include "../../shader/DeferredIntegrationPS.txt"
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

    const u8 shaderPS_Copy[] =
    {
#include "../../shader/CopyPS.txt"
    };

    const u8 shaderPS_Sprite2D[] =
    {
#include "../../shader/Sprite2DPS.txt"
    };

    const u8 shaderPS_Font[] =
    {
#include "../../shader/FontPS.txt"
    };

    const u8 shaderPS_FontOutline[] =
    {
#include "../../shader/FontOutlinePS.txt"
    };

    const u8 shaderPS_Particle[] =
    {
#include "../../shader/ParticlePS.txt"
    };

    const u8 shaderPS_VolumeParticle[] =
    {
#include "../../shader/VolumeParticlePS.txt"
    };

    const u8 shaderPS_VolumeSphere[] =
    {
#include "../../shader/VolumeSpherePS.txt"
    };

    const u8 shaderPS_UI[] =
    {
#include "../../shader/UIPS.txt"
    };

    // CS
    //-------------------------------------------------------
    const u8 shaderCS_LinearDepth[] =
    {
#include "../../shader/LinearDepthCS.txt"
    };

    const u8 shaderCS_CameraMotion[] =
    {
#include "../../shader/CameraMotionCS.txt"
    };
    
    // DS
    //-------------------------------------------------------
    const u8 shaderDS_VolumeParticle[] =
    {
#include "../../shader/VolumeParticleDS.txt"
    };

    // HS
    //-------------------------------------------------------
    const u8 shaderHS_VolumeParticle[] =
    {
#include "../../shader/VolumeParticleHS.txt"
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
        shaderVS_DeferredLighting,

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

        shaderVS_FullQuad,
        shaderVS_FullQuadInstanced,
        shaderVS_Sprite2D,
        shaderVS_Particle,
        shaderVS_VolumeParticle,

        shaderVS_UI,
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
        sizeof(shaderVS_DeferredLighting),

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

        sizeof(shaderVS_FullQuad),
        sizeof(shaderVS_FullQuadInstanced),
        sizeof(shaderVS_Sprite2D),
        sizeof(shaderVS_Particle),
        sizeof(shaderVS_VolumeParticle),

        sizeof(shaderVS_UI),
    };

    //-------------------------------------------------------
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
        shaderGS_LightDepth,
        shaderGS_Particle,
    };

    const s32 DefaultShaderCompiledBytes::sizeGS_[] =
    {
        sizeof(shaderGS_LightDepth),
        sizeof(shaderGS_Particle),
    };

    //-------------------------------------------------------
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
        shaderPS_DeferredShadowAccumulating,
        shaderPS_DeferredLightingDirectional,
        shaderPS_DeferredLightingPoint,
        shaderPS_DeferredLightingSpot,
        shaderPS_DeferredIntegration,

        shaderPS_Forward,
        shaderPS_ForwardShadow,
        shaderPS_ForwardUV,
        shaderPS_ForwardUVShadow,
        shaderPS_ForwardTexC,
        shaderPS_ForwardTexCShadow,
        shaderPS_ForwardTexCN,
        shaderPS_ForwardTexCNShadow,

        shaderPS_Copy,
        shaderPS_Sprite2D,
        shaderPS_Font,
        shaderPS_FontOutline,
        shaderPS_Particle,
        shaderPS_VolumeParticle,
        shaderPS_VolumeSphere,

        shaderPS_UI,
    };

    const s32 DefaultShaderCompiledBytes::sizePS_[] =
    {
        sizeof(shaderPS_Deferred),
        sizeof(shaderPS_DeferredUV),
        sizeof(shaderPS_DeferredTexC),
        sizeof(shaderPS_DeferredTexCN),
        sizeof(shaderPS_DeferredShadowAccumulating),
        sizeof(shaderPS_DeferredLightingDirectional),
        sizeof(shaderPS_DeferredLightingPoint),
        sizeof(shaderPS_DeferredLightingSpot),
        sizeof(shaderPS_DeferredIntegration),

        sizeof(shaderPS_Forward),
        sizeof(shaderPS_ForwardShadow),
        sizeof(shaderPS_ForwardUV),
        sizeof(shaderPS_ForwardUVShadow),
        sizeof(shaderPS_ForwardTexC),
        sizeof(shaderPS_ForwardTexCShadow),
        sizeof(shaderPS_ForwardTexCN),
        sizeof(shaderPS_ForwardTexCNShadow),

        sizeof(shaderPS_Copy),
        sizeof(shaderPS_Sprite2D),
        sizeof(shaderPS_Font),
        sizeof(shaderPS_FontOutline),
        sizeof(shaderPS_Particle),
        sizeof(shaderPS_VolumeParticle),
        sizeof(shaderPS_VolumeSphere),

        sizeof(shaderPS_UI),
    };

    //-------------------------------------------------------
    s32 DefaultShaderCompiledBytes::getSizeCS(s32 id)
    {
        LASSERT(0<=id && id<ShaderCS_Num);
        return sizeCS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesCS(s32 id)
    {
        LASSERT(0<=id && id<ShaderCS_Num);
        return bytesCS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesCS_[] =
    {
        shaderCS_LinearDepth,
        shaderCS_CameraMotion,
    };

    const s32 DefaultShaderCompiledBytes::sizeCS_[] =
    {
        sizeof(shaderCS_LinearDepth),
        sizeof(shaderCS_CameraMotion),
    };

    //-------------------------------------------------------
    s32 DefaultShaderCompiledBytes::getSizeDS(s32 id)
    {
        LASSERT(0<=id && id<ShaderDS_Num);
        return sizeDS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesDS(s32 id)
    {
        LASSERT(0<=id && id<ShaderDS_Num);
        return bytesDS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesDS_[] =
    {
        shaderDS_VolumeParticle,
    };

    const s32 DefaultShaderCompiledBytes::sizeDS_[] =
    {
        sizeof(shaderDS_VolumeParticle),
    };

    //-------------------------------------------------------
    s32 DefaultShaderCompiledBytes::getSizeHS(s32 id)
    {
        LASSERT(0<=id && id<ShaderHS_Num);
        return sizeHS_[id];
    }

    const u8* DefaultShaderCompiledBytes::getBytesHS(s32 id)
    {
        LASSERT(0<=id && id<ShaderHS_Num);
        return bytesHS_[id];
    }

    const u8* DefaultShaderCompiledBytes::bytesHS_[] =
    {
        shaderHS_VolumeParticle,
    };

    const s32 DefaultShaderCompiledBytes::sizeHS_[] =
    {
        sizeof(shaderHS_VolumeParticle),
    };
}
