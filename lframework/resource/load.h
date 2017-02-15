#ifndef INC_LFRAMEWORK_LOAD_H__
#define INC_LFRAMEWORK_LOAD_H__
/**
@file load.h
@author t-sakai
@date 2016/11/21 create
*/
#include "../lframework.h"

namespace lgfx
{
    class Texture2DRef;
    class Texture3DRef;
    class SamplerStateRef;
    class ShaderResourceViewRef;
}

namespace lfw
{
    static const u32 MaxVertices = 0xFFFFU;
    static const u8 MaxNodes = 0xFEU;
    static const u8 InvalidNode = 0xFFU;
    static const u32 MaxNameSize = LFW_CONFIG_NAMESIZE;
    static const u32 MaxNameLength = LFW_CONFIG_NAMELENGTH;
    static const u32 MaxFileNameSize = LFW_CONFIG_FILENAMESIZE;
    static const u32 MaxFileNameLength = LFW_CONFIG_FILENAMELENGTH;

    enum MaterialFlag
    {
        MaterialFlag_CastShadow = 0x01U << 0,
        MaterialFlag_ReceiveShadow = 0x01U << 1,
        MaterialFlag_RefractiveIndex = 0x01U << 2, //shadow_.w_ がrefractive indexか

        MaterialFlag_Transparent = 0x01U<<16,
    };


    enum Elem
    {
        Elem_Geometry =0,
        Elem_Material,
        Elem_Mesh,
        Elem_Node,
        Elem_Joint,
        Elem_Texture,
        Elem_Num,
    };

    /// 頂点属性
    enum VElem
    {
        VElem_Position = (0x01U << 0),
        VElem_Normal   = (0x01U << 1),
        VElem_Tangent  = (0x01U << 2),
        VElem_Binormal = (0x01U << 3),
        VElem_Color    = (0x01U << 4),
        VElem_Texcoord = (0x01U << 5),
        VElem_Bone     = (0x01U<<6),
        VElem_BoneWeight = (0x01U<<7),
        VElem_Num = 8,
    };

    enum VSize
    {
        VSize_Position = sizeof(f32)*3,
        VSize_Normal   = sizeof(u16)*4,
        VSize_Tangent  = sizeof(u16)*4,
        VSize_Binormal = sizeof(u16)*4,
        VSize_Color    = sizeof(u32),
        VSize_Texcoord = sizeof(u16)*2,
        VSize_Bone     = sizeof(u16)*2,
        VSize_BoneWeight = sizeof(u16)*2,
        VSize_Num = 8,
    };

    /**
    @note 総数を4の倍数にすること
    */
    enum TextureType
    {
        TextureType_Albedo =0,
        TextureType_Normal,
        TextureType_Specular,
        TextureType_Shadow,
        TextureType_Irradiance,
        TextureType_AmbientOcclusion,
        TextureType_Used,
        TextureType_Reserved0 = TextureType_Used,
        TextureType_Reserved1,
        TextureType_Num,
        TextureType_Invalid = -1,
    };

    enum RotationOrder
    {
        RotationOrder_EulerXYZ = 0,
        RotationOrder_EulerXZY,
        RotationOrder_EulerYZX,
        RotationOrder_EulerYXZ,
        RotationOrder_EulerZXY,
        RotationOrder_EulerZYX,
        RotationOrder_SphericXYZ,
    };

    //-------------------------------------
    //---
    //--- Element
    //---
    //-------------------------------------
    struct Element
    {
        u32 number_; /// 
        u32 offset_; /// ファイル先頭からのオフセット
    };

    //-------------------------------------
    //---
    //--- LoadHeader
    //---
    //-------------------------------------
    struct LoadHeader
    {
        u16 major_;
        u16 minor_;
        Element elems_[Elem_Num];
    };
}
#endif //INC_LFRAMEWORK_LOAD_H__
