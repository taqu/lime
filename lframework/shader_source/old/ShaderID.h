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
        ShaderVS_Depth =0,
        ShaderVS_DepthSkinning,
        ShaderVS_NormalUV,
        ShaderVS_NormalNoUV,
        ShaderVS_NormalSkinning,
        ShaderVS_NormalShadow,
        ShaderVS_NormalNoUVShadow,
        ShaderVS_NormalSkinningShadow,
        ShaderVS_NormalMap,
        ShaderVS_NormalMapSkinning,
        ShaderVS_NormalMapShadow,
        ShaderVS_NormalMapSkinningShadow,

        ShaderVS_NormalMotion,
        ShaderVS_NormalNoUVMotion,
        ShaderVS_NormalSkinningMotion,
        ShaderVS_Num,
        ShaderVS_User = ShaderVS_Num,
    };

    enum ShaderGS
    {
        ShaderGS_Depth =0,
        ShaderGS_Num,
        ShaderGS_User = ShaderGS_Num,
    };

    enum ShaderPS
    {
        ShaderPS_NormalUV =0,
        ShaderPS_NormalNoUV,
        ShaderPS_NormalShadow,
        ShaderPS_NormalNoUVShadow,
        ShaderPS_NormalTexC,
        ShaderPS_NormalTexCShadow,
        ShaderPS_NormalTexCN,
        ShaderPS_NormalTexCNShadow,
        ShaderPS_NormalMotion,
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
        ShaderDS_Num,
        ShaderDS_User = ShaderDS_Num,
    };

    enum ShaderHS
    {
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
    private:
        static const u8* bytesVS_[];
        static const s32 sizeVS_[];
        static const u8* bytesGS_[];
        static const s32 sizeGS_[];
        static const u8* bytesPS_[];
        static const s32 sizePS_[];
    };
}
#endif //INC_LFRAMEWORK_SHADERID_H__
