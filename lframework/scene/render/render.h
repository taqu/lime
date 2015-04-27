#ifndef INC_LSCENE_LRENDER_RENDER_H__
#define INC_LSCENE_LRENDER_RENDER_H__
/**
@file render.h
@author t-sakai
@date 2014/12/5 create
*/
#include <lcore/lcore.h>
#include <lgraphics/lgraphics.h>
#include "../anim/lanim.h"
#include "../lscene.h"

namespace lscene
{
namespace lrender
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    static const u32 MaxVertices = 0xFFFFU;

    static const u8 MaxNodes = 0xFEU;
    static const u8 InvalidNodeIndex = 0xFFU;

    static const s32 MaxCascades = lscene::MaxCascades;

    typedef u8 MeshIndexType;

    enum Texture
    {
        Tex_Albedo = 0,
        Tex_Normal = 1,
        Tex_Num,
    };

    class Mesh;
    struct ShaderSet;

    enum DefaultShaderTexIndex
    {
        DefaultShaderTexIndex_Albedo = 0,
        DefaultShaderTexIndex_Normal = 1,
        DefaultShaderTexIndex_Shadow = 2,
    };

    enum DefaultShaderType
    {
        DefaultShaderType_Normal =0,
        DefaultShaderType_NormalNoUV,
        DefaultShaderType_NormalTexC,
        DefaultShaderType_NormalTexCN,
        DefaultShaderType_NormalSkinning,
        DefaultShaderType_NormalSkinningTexC,
        DefaultShaderType_NormalSkinningTexCN,

        DefaultShaderType_NormalMotion,
        DefaultShaderType_NormalNoUVMotion,
        DefaultShaderType_NormalTexCMotion,
        DefaultShaderType_NormalTexCNMotion,
        DefaultShaderType_NormalSkinningMotion,
        DefaultShaderType_NormalSkinningTexCMotion,
        DefaultShaderType_NormalSkinningTexCNMotion,
    };

    enum ShaderSetFactoryFlag
    {
        ShaderSetFactoryFlag_None = 0,
        ShaderSetFactoryFlag_Skinning = (0x01U<<0),
    };

    struct ShaderSetFactory
    {
        typedef void(*CreateFunc)(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFactoryFlag flags);
        static void defaultCreateFunc(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFactoryFlag flags);
        static void setShaderSet(Mesh& mesh, ShaderSetFactoryFlag flags);

        static CreateFunc create_;
    };

//#define RENDER_ENABLE_INDIPENDENT_ALPHA
}
}
#endif //INC_LSCENE_LRENDER_RENDER_H__
