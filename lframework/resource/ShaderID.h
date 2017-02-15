#ifndef INC_LFRAMEWORK_SHADERID_H__
#define INC_LFRAMEWORK_SHADERID_H__
/**
@file ShaderID.h
@author t-sakai
@date 2016/11/12 create
*/
#include "../lframework.h"

namespace lfw
{
    enum ShaderVS
    {
        ShaderVS_LightDepth =0,
        ShaderVS_LightDepthSkinning,

        ShaderVS_Deferred,
        ShaderVS_DeferredSkinning,
        ShaderVS_DeferredUV,
        ShaderVS_DeferredUVSkinning,
        ShaderVS_DeferredNUV,
        ShaderVS_DeferredNUVSkinning,

        ShaderVS_Forward,
        ShaderVS_ForwardShadow,
        ShaderVS_ForwardSkinning,
        ShaderVS_ForwardSkinningShadow,
        ShaderVS_ForwardUV,
        ShaderVS_ForwardUVShadow,
        ShaderVS_ForwardUVSkinning,
        ShaderVS_ForwardUVSkinningShadow,
        ShaderVS_ForwardNUV,
        ShaderVS_ForwardNUVShadow,
        ShaderVS_ForwardNUVSkinning,
        ShaderVS_ForwardNUVSkinningShadow,

        ShaderVS_FullQuad,
        ShaderVS_Sprite2D,
        ShaderVS_Particle,
        ShaderVS_VolumeParticle,

        ShaderVS_UI,
        ShaderVS_Num,
        ShaderVS_User = ShaderVS_Num,
    };

    enum ShaderGS
    {
        ShaderGS_LighDepth =0,
        ShaderGS_Particle,
        ShaderGS_Num,
        ShaderGS_User = ShaderGS_Num,
    };

    enum ShaderPS
    {
        ShaderPS_Deferred =0,
        ShaderPS_DeferredUV,
        ShaderPS_DeferredTexC,
        ShaderPS_DeferredTexCN,
        ShaderPS_DeferredShadowAccumulating,
        ShaderPS_DeferredLighting,

        ShaderPS_Forward,
        ShaderPS_ForwardShadow,
        ShaderPS_ForwardUV,
        ShaderPS_ForwardUVShadow,
        ShaderPS_ForwardTexC,
        ShaderPS_ForwardTexCShadow,
        ShaderPS_ForwardTexCN,
        ShaderPS_ForwardTexCNShadow,

        ShaderPS_Copy,
        ShaderPS_Sprite2D,
        ShaderPS_Font,
        ShaderPS_FontOutline,
        ShaderPS_Particle,
        ShaderPS_VolumeParticle,

        ShaderPS_UI,
        ShaderPS_Num,
        ShaderPS_User = ShaderPS_Num,
    };

    enum ShaderCS
    {
        ShaderCS_Num,
        ShaderCS_User = ShaderCS_Num,
    };

    enum ShaderDS
    {
        ShaderDS_VolumeParticle,
        ShaderDS_Num,
        ShaderDS_User = ShaderDS_Num,
    };

    enum ShaderHS
    {
        ShaderHS_VolumeParticle,
        ShaderHS_Num,
        ShaderHS_User = ShaderHS_Num,
    };

    class DefaultShaderCompiledBytes
    {
    public:
        static s32 getSizeVS(s32 id);
        static const u8* getBytesVS(s32 id);
        static s32 getSizeGS(s32 id);
        static const u8* getBytesGS(s32 id);
        static s32 getSizePS(s32 id);
        static const u8* getBytesPS(s32 id);
        static s32 getSizeCS(s32 id);
        static const u8* getBytesCS(s32 id);
        static s32 getSizeDS(s32 id);
        static const u8* getBytesDS(s32 id);
        static s32 getSizeHS(s32 id);
        static const u8* getBytesHS(s32 id);
    private:
        static const u8* bytesVS_[];
        static const s32 sizeVS_[];
        static const u8* bytesGS_[];
        static const s32 sizeGS_[];
        static const u8* bytesPS_[];
        static const s32 sizePS_[];
        //static const u8* bytesCS_[];
        //static const s32 sizeCS_[];
        static const u8* bytesDS_[];
        static const s32 sizeDS_[];
        static const u8* bytesHS_[];
        static const s32 sizeHS_[];
    };
}
#endif //INC_LFRAMEWORK_SHADERID_H__
