#ifndef INC_LSCENE_LRENDER_SHADERID_H__
#define INC_LSCENE_LRENDER_SHADERID_H__
/**
@file ShaderID.h
@author t-sakai
@date 2014/12/15 create
*/
namespace lscene
{
namespace lrender
{
    enum ShaderVS
    {
        ShaderVS_Depth =0,
        ShaderVS_DepthSkinning,
        ShaderVS_Normal,
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
        ShaderPS_Normal =0,
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
}
}
#endif //INC_LSCENE_LRENDER_SHADERID_H__
