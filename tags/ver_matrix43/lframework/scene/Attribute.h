#ifndef INC_LGRAPHICS_ATTRIBUTE_H__
#define INC_LGRAPHICS_ATTRIBUTE_H__
/**
@file Attribute.h
@author t-sakai
@date 2010/05/02 create
*/
namespace lgraphics
{
namespace attribute
{
    enum AttributeType
    {
        // 環境パラメータ
        Attribute_World = 0,
        Attribute_View,
        Attribute_Projection,
        Attribute_WVP, //ワールドｘビューｘプロジェクションマトリックス
        Attribute_DLDirection0,
        Attribute_DLDirection1,
        Attribute_DLColor0,
        Attribute_DLColor1,
        Attribute_CameraPosition,

        // 頂点
        Attribute_VPosition0,
        Attribute_VNormal0,
        Attribute_VTangent0,
        Attribute_VColor0,
        Attribute_VColor1,
        Attribute_VBone0,
        Attribute_VWeight0,
        Attribute_VTex2D0,
        Attribute_VTex2D1,
        Attribute_VTex2D2,
        Attribute_VTex2D3,

        // ピクセル
        Attribute_PRGBA0,
        Attribute_PDepth0,

        // マテリアル
        Attribute_Diffuse0,
        Attribute_Diffuse1,
        Attribute_Specular0,
        Attribute_Specular1,
        Attribute_Ambient0,
        Attribute_Ambient1,
        Attribute_Transparency0,

        // ジオメトリ
        Attribute_SkinningMat, //スキニング用行列
        Attribute_SkinningDq,  //スキニング用デュアルクウォータニオン

        // テクスチャ
        Attribute_Texture2D0,
        Attribute_Texture2D1,
        Attribute_Texture2D2,
        Attribute_Texture2D3,

        Attribute_Num,
        Attribute_Max = 127,
    };

   extern const char* AttributeNameTable[];
}
}
#endif //INC_LGRAPHICS_ATTRIBUTE_H__
