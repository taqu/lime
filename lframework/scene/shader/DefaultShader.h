#ifndef INC_LSCENE_DEFAULTSHADER_H__
#define INC_LSCENE_DEFAULTSHADER_H__
/**
@file DefaultShader.h
@author t-sakai
@date 2010/12/13 create
*/
#include <lcore/FixedArray.h>
#include <lmath/Matrix43.h>
#include <lgraphics/lgraphicscore.h>
#include <lframework/scene/ShaderBase.h>

namespace lgraphics
{
   class VertexShaderRef;
   class PixelShaderRef;
}

namespace lrender
{
    class Drawable;
    class Batch;
}

namespace lscene
{
    class Scene;
    class Material;
    class Geometry;

    //---------------------------------------------------
    //---
    //--- シェーダデータ
    //---
    //---------------------------------------------------
    namespace shader
    {
        /// デフォルトバーテックスシェーダソース
        extern const char DefaultVSSource[];
        extern const u32 DefaultVSSourceSize;

        /// Phongピクセルシェーダソース
        extern const char PhongPSSource[];
        extern const u32 PhongPSSourceSize;

        /// Toon
        extern const char ToonPSSource[];
        extern const u32 ToonPSSourceSize;

        /// 頂点フォーマットフラグ
        enum VertexFlag
        {
            VFlag_Normal = (0x01U << 0),
            VFlag_Color0 = (0x01U << 1),
            VFlag_Tex0   = (0x01U << 2),
            VFlag_Bone   = (0x01U << 3),
            VFlag_Weight = (0x01U << 4),
            VFlag_Pos0   = (0x01U << 5),
            VFlag_Num = 5,
            VFlag_None = 0,
        };

        /// バーテックスシェーダパラメータ
        enum ParamVS
        {
            ParamVS_WVP =0,
            ParamVS_DLightDirection,
            ParamVS_DLightColor,
            ParamVS_CameraPosition,
            ParamVS_Diffuse,
            ParamVS_Ambient,
            ParamVS_Specular,
            ParamVS_Emissive,
            ParamVS_MatrixPalette,
            ParamVS_Num,
        };

        /// ピクセルシェーダパラメータ
        enum ParamPS
        {
            ParamPS_DLightDirection =0,
            ParamPS_DLightColor,
            ParamPS_CameraPosition,
            ParamPS_Diffuse,
            ParamPS_Ambient,
            ParamPS_Specular,
            ParamPS_Emissive,
            ParamPS_Fresnel,
            ParamPS_Num,
        };

        /// ピクセルシェーダテクスチャ
        enum TexPS
        {
            TexPS_Albedo,
            TexPS_Albedo2,
            TexPS_Envmap,
            TexPS_Num,
        };

        extern const char* ParamVSNames[ParamVS_Num]; /// バーテックスシェーダパラメータ名
        extern const char* ParamPSNames[ParamPS_Num]; /// ピクセルシェーダパラメータ名
        extern const char* TexPSNames[TexPS_Num]; /// ピクセルシェーダテクスチャ名

        /// マクロ
        enum Macro
        {
            Macro_VNORMAL,
            Macro_VTEX0,
            Macro_VBONE,
            Macro_VWEIGHT,

            Macro_PCOLOR,
            Macro_PNORMAL,
            Macro_PTEX0,
            Macro_PPOS,

            Macro_VDIFFUSE,
            Macro_FRESNEL,
            Macro_SKINNING,

            Macro_TEXALBEDO,
            Macro_TEXSHADE,
            Macro_EMISSIVE,
            Macro_ALPHATEST,

            Macro_LightingVS,

            Macro_SPHEREMAP,
            Macro_SPHEREMAPADD,

            Macro_Num,
        };

        extern const char* MacroNames[Macro_Num]; /// マクロ名


        enum ShaderID
        {
            Shader_Phong =0,
            Shader_Toon,
            Shader_Num,
        };

        extern const u32 ShaderName[Shader_Num];

        ShaderID getShaderID(u32 shaderName);

        extern const char* ShaderPSSource[Shader_Num];
        extern const u32 ShaderPSSourceSize[Shader_Num];
    }

    //---------------------------------------------------
    //---
    //--- DefaultShaderVS
    //---
    //---------------------------------------------------
    class DefaultShaderVS : public lscene::ShaderVSBase
    {
    public:
        DefaultShaderVS(const lgraphics::VertexShaderRef& ref);
        virtual ~DefaultShaderVS();

        virtual void initialize();

        virtual void setParameters(const lrender::Batch& batch, const lscene::Scene& scene);

    private:

        lgraphics::HANDLE params_[shader::ParamVS_Num];
#if defined(LIME_GL)
        /**
        @brief 行列の要素を転置する。

        dstとsrcが同じ場合、結果を保障しない。
        */
        inline void toMatrix34(lmath::Matrix43& dst, const lmath::Matrix43& src);

        inline void setInitializeMatrix34(lmath::Matrix43& dst);

        lmath::Matrix43 palette_[lgraphics::LIME_MAX_SKINNING_MATRICES];
#endif
    };


    //---------------------------------------------------
    //---
    //--- DefaultShaderPS
    //---
    //---------------------------------------------------
    class DefaultShaderPS : public lscene::ShaderPSBase
    {
    public:
        DefaultShaderPS(const lgraphics::PixelShaderRef& ref);
        ~DefaultShaderPS();

        virtual void initialize();

        virtual void setParameters(const lrender::Batch& batch, const lscene::Scene& scene);
    private:
        lgraphics::HANDLE params_[shader::ParamPS_Num];
        u32 textures_[shader::TexPS_Num];
    };
}

#endif //INC_LSCENE_DEFAULTSHADER_H__
