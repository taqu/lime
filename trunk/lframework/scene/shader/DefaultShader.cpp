/**
@file DefaultShader.cpp
@author t-sakai
@date 2010/12/13 create
*/
#include "DefaultShader.h"

#include <lmath/Matrix34.h>

#include <lgraphics/lgraphics.h>
#include <lgraphics/lgraphicsAPIInclude.h>
#include <lgraphics/api/SamplerState.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/api/GraphicsDeviceRef.h>

#include <lframework/scene/Scene.h>
#include <lframework/scene/Material.h>
#include <lframework/scene/Geometry.h>

#include <lframework/render/Drawable.h>
#include <lframework/render/Batch.h>

namespace lscene
{
    using namespace lgraphics;
    using namespace shader;

    namespace
    {
        /// パレット行列転送用バッファ
        lmath::Matrix34 TempPalette[lgraphics::LIME_MAX_SKINNING_MATRICES];
    }

    //---------------------------------------------------
    //---
    //--- DefaultShaderVS
    //---
    //---------------------------------------------------
    DefaultShaderVS::DefaultShaderVS(const lgraphics::VertexShaderRef& ref)
        :ShaderVSBase(ref)
    {
    }

    DefaultShaderVS::~DefaultShaderVS()
    {
    }

    void DefaultShaderVS::initialize()
    {
        LASSERT(shader_.valid());

        for(s32 i=0; i<ParamVS_Num; ++i){
            setHandle(params_[i], ParamVSNames[i]);
        }
    }

    void DefaultShaderVS::setParameters(const lrender::Batch& batch, const lscene::Scene& scene)
    {
        const lrender::Drawable* drawable = batch.getDrawable();
        const lscene::Geometry* geometry = batch.getGeometry();
        const lscene::Material* material = batch.getMaterial();


        lmath::Matrix44 mat( scene.getViewProjMatrix() );
        mat *= drawable->getWorldMatrix();

        mat.transpose();
        shader_.setMatrix(params_[ParamVS_WVP], mat, 0);

        if(params_[ParamVS_WV]>0){
            mat = scene.getViewMatrix();
            mat *= drawable->getWorldMatrix();
            mat.transpose();
            shader_.setMatrix(params_[ParamVS_WV], mat, 0);
        }


        if(geometry->getFlags().checkFlag( Geometry::GeomFlag_LightingVS )){
            const lscene::LightEnvironment& lightEnv = scene.getLightEnv();

            lmath::Matrix34 toLocal = drawable->getWorldMatrix();
            toLocal.invert();

            // ライト方向をローカルへ変換
            lmath::Vector3 lightDirection = lightEnv.getDirectionalLight().getDirection();
            lightDirection.mul33(lightDirection, toLocal);

            shader_.setVector3(params_[ParamVS_DLightDirection], lightDirection);
            //shader_.setVector4(params_[ParamVS_DLightColor], lightEnv.getDirectionalLight().getColor());

            lmath::Matrix34 cameraToLocal( scene.getViewMatrix() );
            cameraToLocal *= drawable->getWorldMatrix();
            cameraToLocal.invert();

            // カメラ位置をローカルへ変換
            lmath::Vector3 cameraPos(0.0f,0.0f,0.0f);
            cameraPos.mul(cameraToLocal, cameraPos);

            shader_.setVector3(params_[ParamVS_CameraPosition], cameraPos);


            if( material->getFlags().checkFlag(Material::MatFlag_TexGrad) ){

            }

            //マテリアル
            //shader_.setVector4(params_[ParamVS_Diffuse], material->diffuse_);
            shader_.setVector4(params_[ParamVS_Specular], material->specular_);
            //shader_.setVector3(params_[ParamVS_Ambient], material->ambient_);
            //shader_.setVector3(params_[ParamVS_Emissive], material->emissive_);

        }

        //スキニング。マトリックスセット
        if(geometry->getFlags().checkFlag(lscene::Geometry::GeomFlag_Skinning)){
#if defined(LIME_GL)
            if(drawable->getNumJointPoses()>0){
                const lmath::Matrix34* poses = drawable->getGlobalJointPoses();

                u8 count = geometry->getNumBonesInPalette();
                for(u8 i=0; i<count; ++i){
                    TempPalette[i] = poses[ geometry->getBoneInPalette(i) ];
                }

                shader_.setVector4Array(params_[ParamVS_MatrixPalette], reinterpret_cast<const lmath::Vector4*>(TempPalette), 3*count);
            }
#else
            const lmath::Matrix34* poses = drawable->getGlobalJointPoses();


            u8 count = geometry->getNumBonesInPalette() * 3;
#if 1
            lmath::Matrix34 palette[lgraphics::LIME_MAX_SKINNING_MATRICES];
            for(u32 i=0; i<count; ++i){
                palette[i].identity();
            }

            shader_.setVector4Array(params_[ParamVS_MatrixPalette], reinterpret_cast<const lmath::Vector4*>(palette), count);
#else
            shader_.setVector4Array(params_[ParamVS_MatrixPalette], reinterpret_cast<const lmath::Vector4*>(poses), count);
#endif

#endif
        }
    }


    //---------------------------------------------------
    //---
    //--- DefaultShaderPS
    //---
    //---------------------------------------------------
    DefaultShaderPS::DefaultShaderPS(const lgraphics::PixelShaderRef& ref)
        :ShaderPSBase(ref)
    {
    }

    DefaultShaderPS::~DefaultShaderPS()
    {
    }

    void DefaultShaderPS::initialize()
    {
        LASSERT(shader_.valid());

        for(s32 i=0; i<ParamPS_Num; ++i){
            setHandle(params_[i], ParamPSNames[i]);
        }

        for(s32 i=0; i<TexPS_Num; ++i){
            setSampler(textures_[i], TexPSNames[i]);
        }
    }

    void DefaultShaderPS::setParameters(const lrender::Batch& batch, const lscene::Scene& scene)
    {
        const lrender::Drawable* drawable = batch.getDrawable();
        const lscene::Material* material = batch.getMaterial();
        const lscene::LightEnvironment& lightEnv = scene.getLightEnv();

        if(material->getFlags().checkFlag( lscene::Material::MatFlag_LightingVS )){
            shader_.setVector4(params_[ParamPS_DLightColor], lightEnv.getDirectionalLight().getColor());

            //マテリアル
            shader_.setVector4(params_[ParamPS_Diffuse], material->diffuse_);
            shader_.setVector3(params_[ParamPS_Ambient], material->ambient_);
            shader_.setVector3(params_[ParamPS_Emissive], material->emissive_);

        }else{

            lmath::Matrix34 toLocal = drawable->getWorldMatrix();
            toLocal.invert();

            // ライト方向をローカルへ変換
            lmath::Vector3 lightDirection = lightEnv.getDirectionalLight().getDirection();
            lightDirection.mul33(toLocal, lightDirection);

            shader_.setVector3(params_[ParamPS_DLightDirection], lightDirection);
            shader_.setVector4(params_[ParamPS_DLightColor], lightEnv.getDirectionalLight().getColor());

            lmath::Matrix34 cameraToLocal( scene.getViewMatrix() );
            cameraToLocal *= drawable->getWorldMatrix();
            cameraToLocal.invert();

            // カメラ位置をローカルへ変換
            lmath::Vector3 cameraPos(0.0f,0.0f,0.0f);
            cameraPos.mul(cameraToLocal, cameraPos);

            shader_.setVector3(params_[ParamPS_CameraPosition], cameraPos);


            if( material->getFlags().checkFlag(Material::MatFlag_Fresnel) ){

                shader_.setFloat(params_[ParamPS_Fresnel], material->reflectance_);

            }else if( material->getFlags().checkFlag(Material::MatFlag_TexGrad) ){

            }

            //マテリアル
            shader_.setVector4(params_[ParamPS_Diffuse], material->diffuse_);
            shader_.setVector4(params_[ParamPS_Specular], material->specular_);
            shader_.setVector3(params_[ParamPS_Ambient], material->ambient_);
            shader_.setVector3(params_[ParamPS_Emissive], material->emissive_);
        }

        //TODO:なんかもういろいろ
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

        //テクスチャ
        for(u32 i=0; i<material->getTextureNum(); ++i){
            const lgraphics::TextureRef& tex = material->getTexture(i);
            if(tex.valid() == false){
                continue;
            }

#if defined(LIME_GLES2)
            device.setTexture(i, tex.getTextureID(), textures_[i]);
#else
            const lgraphics::SamplerState& samplerState = material->getSamplerState(i);
            samplerState.apply( textures_[i] );
            tex.attach( i );
#endif
        }
    }


    //---------------------------------------------------
    //---
    //--- シェーダデータ
    //---
    //---------------------------------------------------
namespace shader
{
    // デフォルトバーテックスシェーダソース
    const char DefaultVSSource[] =
#if defined(LIME_GLES2)
#include "SimpleVS.glsl"
#else
#include "SimpleVS.hlsl"
#endif
        ;

    const u32 DefaultVSSourceSize = sizeof(DefaultVSSource);

    // Phongピクセルシェーダソース
    const char PhongPSSource[] =
#if defined(LIME_GLES2)
#include "ModifiedPhongPS.glsl"
#else
#include "ModifiedPhongPS.hlsl"
#endif
        ;
    const u32 PhongPSSourceSize = sizeof(PhongPSSource);

    // Toonピクセルシェーダソース
    const char ToonPSSource[] =
#if defined(LIME_GLES2)
#include "ToonPS.glsl"
#else
#include "ToonPS.hlsl"
#endif
        ;
    const u32 ToonPSSourceSize = sizeof(ToonPSSource);

    // バーテックスシェーダパラメータ名
    const char* ParamVSNames[ParamVS_Num] =
    {
        "mwvp",
        "mwv",
        "dlDir",
        "dlColor",
        "camPos",
        "diffuse",
        "ambient",
        "specular",
        "emissive",
        "palette",
    };

    // ピクセルシェーダパラメータ名
    const char* ParamPSNames[ParamPS_Num] =
    {
        "dlDir",
        "dlColor",
        "camPos",
        "diffuse",
        "ambient",
        "specular",
        "emissive",
        "fresnel",
    };

    // ピクセルシェーダテクスチャ名
    const char* TexPSNames[TexPS_Num] =
    {
        "texAlbedo",
        "texAlbedo2",
        "texSphere",
    };

    // マクロ名
#if defined(LIME_GLES2)
    const char* MacroNames[Macro_Num] =
    {
        "#define VNORMAL\n",
        "#define VTEX0\n",
        "#define VBONE\n",
        "#define VWEIGHT\n",
        
        "#define PCOLOR\n",
        "#define PNORMAL\n",
        "#define PTEX0\n",
        "#define PPOS\n",

        "#define VDIFFUSE\n",
        "#define FRESNEL\n",
        "#define SKINNING\n",

        "#define TEXALBEDO\n",
        "#define TEXSHADE\n",

        "#define EMISSIVE\n",

        "#define ALPHATEST\n",
        "#define LIGHTVS\n",

        "#define SPHEREMAP\n",
        "#define SPHEREMAPADD\n",
    };
#else
    const char* MacroNames[Macro_Num] =
    {
        "VNORMAL",
        "VTEX0",
        "VBONE",
        "VWEIGHT",
        
        "PCOLOR",
        "PNORMAL",
        "PTEX0",
        "PPOS",

        "VDIFFUSE",
        "FRESNEL",
        "SKINNING",

        "TEXALBEDO",
        "TEXSHADE",

        "EMISSIVE",

        "ALPHATEST",
        "LIGHTVS",

        "SPHEREMAP",
        "SPHEREMAPADD",
    };
#endif



    const u32 ShaderName[Shader_Num] =
    {
        'phon',
        'toon',
    };

    ShaderID getShaderID(u32 shaderName)
    {
        for(u32 i=0; i<Shader_Num; ++i){
            if(shaderName == ShaderName[i]){
                return static_cast<ShaderID>(i);
            }
        }
        return Shader_Phong;
    }


    const char* ShaderPSSource[Shader_Num] =
    {
        PhongPSSource,
        ToonPSSource,
    };

    const u32 ShaderPSSourceSize[Shader_Num] =
    {
        PhongPSSourceSize,
        ToonPSSourceSize,
    };
}

}
