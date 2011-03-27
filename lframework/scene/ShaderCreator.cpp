/**
@file ShaderCreator.cpp
@author t-sakai
@date 2010/12/13 create
*/
#include "lscene.h"
#include "ShaderCreator.h"

#include <lgraphics/lgraphics.h>
#include <lgraphics/api/GeometryBuffer.h>
#include <lgraphics/api/VertexDeclarationRef.h>
#include <lgraphics/api/ShaderKey.h>
#include <lgraphics/api/ShaderRef.h>

#include "ShaderManager.h"
#include "Geometry.h"
#include "Material.h"

#include "shader/DefaultShader.h"

namespace lscene
{
    using namespace shader;

    namespace
    {
        /**
        @brief 頂点フォーマットから入力フラグを作成
        */
        u32 createGeometryInputFlags(Geometry& geometry)
        {
            lgraphics::VertexElement elements[lgraphics::VertexDeclarationRef::MAX_ELEMENTS];

            lgraphics::GeometryBuffer::pointer& geomBuffer = geometry.getGeometryBuffer();

            u32 numElements = geomBuffer->getDecl().getNumElements();
            bool ret = geomBuffer->getDecl().getDecl(elements);
            if(!ret){
                return 0;
            }

            static u16 vflags[lgraphics::DeclUsage_Num] =
            {
                VFlag_None,
                VFlag_Weight,
                VFlag_Bone,
                VFlag_Normal,
                VFlag_None,
                VFlag_Tex0,
                VFlag_None,
                VFlag_None,
                VFlag_None,
                VFlag_None, //DeclUsage_PositionT
                VFlag_Color0,
                VFlag_None,
                VFlag_None,
                VFlag_None,
            };

            u32 flags = 0;
            for(u32 i=0; i<numElements; ++i){
                u16 flag = vflags[ elements[i].getUsage() ];
                flag <<= elements[i].getUsageIndex();

                flags |= flag;
            }
            return flags;
        }


        /**
        @brief
        */
        void addVertexDeclarationMacrosVS(lgraphics::VertexDeclarationRef& vdecl, const char** macros, u32& numMacros)
        {
            lgraphics::VertexElement elements[lgraphics::VertexDeclarationRef::MAX_ELEMENTS];
            if(false == vdecl.getDecl(elements)){
                return;
            }

            Macro toMacro[lgraphics::DeclUsage_Num] =
            {
                Macro_Num,
                Macro_VWEIGHT,
                Macro_VBONE,
                Macro_VNORMAL,
                Macro_Num,
                Macro_VTEX0,

                Macro_Num,
                Macro_Num,
                Macro_Num,
                Macro_Num,
                Macro_Num,
                Macro_Num,
                Macro_Num,
                Macro_Num,
            };
            for(u32 i=0; i<vdecl.getNumElements(); ++i){
                u8 usage = elements[i].getUsage();
                if(toMacro[usage] != Macro_Num){
                    macros[numMacros++] = MacroNames[ toMacro[usage] ];

                    if(usage == lgraphics::DeclUsage_Normal){
                        macros[numMacros++] = MacroNames[Macro_PNORMAL];
                    }
                }
            }
        }
    }

#if defined(LIME_GLES2)
    void ShaderCreator::create(
            Geometry& geometry,
            Material& material,
            ShaderManager& manager,
            ShaderVSBase*& retVS,
            ShaderPSBase*& retPS) const
    {
        // TODO:とりあえず仮で頂点フォーマット決めうちフル
        //----------------------------------------------------------
        u32 inputVSFlags = createGeometryInputFlags(geometry) | shader::VFlag_Pos0;
        u32 inputPSFlags = inputVSFlags;

        u32 featureVSFlags = geometry.getFlags().get();
        u32 featurePSFlags = material.getFlags().get();

        lgraphics::ShaderKey vsKey(0, (inputVSFlags<<16), featureVSFlags); //頂点シェーダのフラグは上位ワードへ
        lgraphics::ShaderKey psKey(material.getShaderID(), inputPSFlags, featurePSFlags);

        // すでに登録されているか
        lgraphics::Descriptor* vsDesc = manager.findVS(vsKey);

        lgraphics::Descriptor* psDesc = manager.findPS(psKey);


        //VSないから作るよ
        if(vsDesc == NULL){
            // 頂点フォーマットとピクセルシェーダへの出力、機能によってマクロセット
            const char* macros[Macro_Num];
            u32 numMacros = 0;

            // 入力のマクロ生成
            addVertexDeclarationMacrosVS(geometry.getGeometryBuffer()->getDecl(), macros, numMacros);

            if(featurePSFlags & Material::MatFlag_Fresnel
                || featurePSFlags & Material::MatFlag_TexGrad){
                macros[numMacros++] = MacroNames[Macro_PPOS];
            }

            if(inputVSFlags & VFlag_Tex0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }

            // 頂点カラーがあるか、拡散反射を頂点で計算するなら色出力
            if( (featureVSFlags & Geometry::GeomFlag_DiffuseVS)
                && (inputVSFlags & VFlag_Normal))
            {
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                    macros[numMacros++] = MacroNames[Macro_VDIFFUSE];
            }else{
                if(inputVSFlags & VFlag_Color0){
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                }
            }

            //スキニングあり？
            if(geometry.getFlags().checkFlag(lscene::Geometry::GeomFlag_Skinning)){
                macros[numMacros++] = MacroNames[Macro_SKINNING];
            }

            //シェーダ作成
            vsDesc = lgraphics::Shader::createVertexShaderFromMemory(
                shader::DefaultVSSource,
                shader::DefaultVSSourceSize,
                numMacros,
                macros);
            manager.addVS(vsKey, vsDesc);
#if 1
        }
#else
            lcore::Log("=== create vs ===");
            for(u32 i=0; i<numMacros; ++i){
                lcore::Log("%s", macros[i]);
            }
        }else{
            lcore::Log("=== not create vs ===");
        }
#endif

        //PSないから作るよ
        if(psDesc == NULL){
            const char* macros[Macro_Num];
            u32 numMacros = 0;

            // 頂点カラーがあるか、拡散反射を頂点で計算するなら色出力
            if( (featureVSFlags & Geometry::GeomFlag_DiffuseVS)
                && (inputVSFlags & VFlag_Normal))
            {
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
            }else{
                if(inputVSFlags & VFlag_Color0){
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                }
            }

            if(inputVSFlags & VFlag_Normal){ //頂点フォーマットに法線があれば
                macros[numMacros++] = MacroNames[Macro_PNORMAL];
            }
            if((inputVSFlags & VFlag_Tex0) && material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }

            if(featurePSFlags & Material::MatFlag_Fresnel){
                macros[numMacros++] = MacroNames[Macro_PPOS];
                macros[numMacros++] = MacroNames[Macro_FRESNEL];
            }

            //発光
            if(featurePSFlags & Material::MatFlag_Emissive){
                macros[numMacros++] = MacroNames[Macro_EMISSIVE];
            }

            // 法線あり、テクスチャでグラディエーションフラグ、テクスチャありなら
            if(inputVSFlags & VFlag_Normal
                && featurePSFlags & Material::MatFlag_TexGrad
                && material.getTextureNum()>0)
            {
                macros[numMacros++] = MacroNames[Macro_PPOS];
                macros[numMacros++] = MacroNames[Macro_TEXSHADE];
            }

            shader::ShaderID shaderID = shader::getShaderID( material.getShaderID() );
            const char* source = shader::ShaderPSSource[ shaderID ];
            u32 sourceSize = shader::ShaderPSSourceSize[ shaderID ];

            psDesc = lgraphics::Shader::createPixelShaderFromMemory(
                source,
                sourceSize,
                numMacros,
                macros);

            manager.addPS(psKey, psDesc);
#if 1
        }
#else
            lcore::Log("=== create ps ===");
            for(u32 i=0; i<numMacros; ++i){
                lcore::Log("%s", macros[i]);
            }
        }else{
            lcore::Log("=== not create ps ===");
        }
#endif

        ShaderVSBase* vs = NULL;
        ShaderPSBase* ps = NULL;
        if(false == manager.findProgram(vsDesc, psDesc, vs, ps)){
            //プログラムがなかったから作るよ
            lgraphics::VertexShaderRef vsRef;
            lgraphics::PixelShaderRef psRef;
            lgraphics::VertexDeclarationRef& decl = geometry.getGeometryBuffer()->getDecl();
            lgraphics::Shader::linkShader(vsRef, psRef, decl, vsDesc, psDesc);

            //頂点属性インデックスをattribute変数にバインドする
            decl.bindAttributes(vsRef.getProgramID()->id_);

            vs = LIME_NEW DefaultShaderVS(vsRef);
            ps = LIME_NEW DefaultShaderPS(psRef);

            manager.addProgram(vs, ps);
            vs->initialize();
            ps->initialize();
        }

        retVS = vs;
        retPS = ps;
    }


#elif defined(LIME_DX9)
    void ShaderCreator::create(
            Geometry& geometry,
            Material& material,
            ShaderManager& manager,
            ShaderVSBase*& retVS,
            ShaderPSBase*& retPS) const
    {
        // TODO:とりあえず仮で頂点フォーマット決めうちフル
        //----------------------------------------------------------
        u32 inputVSFlags = createGeometryInputFlags(geometry) | shader::VFlag_Pos0;
        u32 inputPSFlags = inputVSFlags;

        u32 featureVSFlags = geometry.getFlags().get();
        u32 featurePSFlags = material.getFlags().get();

        lgraphics::ShaderKey vsKey(0, (inputVSFlags<<16), featureVSFlags); //頂点シェーダのフラグは上位ワードへ
        lgraphics::ShaderKey psKey(material.getShaderID(), inputPSFlags, featurePSFlags);

        // すでに登録されているか
        ShaderVSBase *vs = manager.findVS(vsKey);

        ShaderPSBase *ps = manager.findPS(psKey);


        //VSないから作るよ
        if(vs == NULL){
            // 頂点フォーマットとピクセルシェーダへの出力、機能によってマクロセット
            const char* macros[Macro_Num];
            u32 numMacros = 0;

            // 入力のマクロ生成
            addVertexDeclarationMacrosVS(geometry.getGeometryBuffer()->getDecl(), macros, numMacros);

            if(featurePSFlags & Material::MatFlag_Fresnel
                || featurePSFlags & Material::MatFlag_TexGrad){
                macros[numMacros++] = MacroNames[Macro_PPOS];
            }

            if(inputVSFlags & VFlag_Tex0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }

            // 頂点カラーがあるか、拡散反射を頂点で計算するなら色出力
            if( (featureVSFlags & Geometry::GeomFlag_DiffuseVS)
                && (inputVSFlags & VFlag_Normal))
            {
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                    macros[numMacros++] = MacroNames[Macro_VDIFFUSE];
            }else{
                if(inputVSFlags & VFlag_Color0){
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                }
            }

            //スキニングあり？
            if(geometry.getFlags().checkFlag(lscene::Geometry::GeomFlag_Skinning)){
                macros[numMacros++] = MacroNames[Macro_SKINNING];
            }

            lgraphics::VertexShaderRef vsRef = lgraphics::Shader::createVertexShaderFromMemory(
                shader::DefaultVSSource,
                shader::DefaultVSSourceSize,
                numMacros,
                macros);
            LASSERT(vsRef.valid());

            vsRef.setKey(vsKey); //キーセット

            vs = LIME_NEW DefaultShaderVS(vsRef);
            vs->initialize();
            manager.addVS(vs);
        }

        //PSないから作るよ
        if(ps == NULL){
            const char* macros[Macro_Num];
            u32 numMacros = 0;

            macros[numMacros++] = MacroNames[Macro_PPOS];

            // 頂点カラーがあるか、拡散反射を頂点で計算するなら色出力
            if( (featureVSFlags & Geometry::GeomFlag_DiffuseVS)
                && (inputVSFlags & VFlag_Normal))
            {
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
            }else{
                if(inputVSFlags & VFlag_Color0){
                    macros[numMacros++] = MacroNames[Macro_PCOLOR];
                }
            }

            if(inputVSFlags & VFlag_Normal){ //頂点フォーマットに法線があれば
                macros[numMacros++] = MacroNames[Macro_PNORMAL];
            }
            if((inputVSFlags & VFlag_Tex0) && material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }

            if(featurePSFlags & Material::MatFlag_Fresnel){
                macros[numMacros++] = MacroNames[Macro_PPOS];
                macros[numMacros++] = MacroNames[Macro_FRESNEL];
            }

            // 法線あり、テクスチャでグラディエーションフラグ、テクスチャありなら
            if(inputVSFlags & VFlag_Normal
                && featurePSFlags & Material::MatFlag_TexGrad
                && material.getTextureNum()>0)
            {
                macros[numMacros++] = MacroNames[Macro_PPOS];
                macros[numMacros++] = MacroNames[Macro_TEXSHADE];
            }

            shader::ShaderID shaderID = shader::getShaderID( material.getShaderID() );
            const char* source = shader::ShaderPSSource[ shaderID ];
            u32 sourceSize = shader::ShaderPSSourceSize[ shaderID ];

            lgraphics::PixelShaderRef psRef = lgraphics::Shader::createPixelShaderFromMemory(
                source,
                sourceSize,
                numMacros,
                macros);
            LASSERT(psRef.valid());

            psRef.setKey(psKey); //キーセット

            ps = LIME_NEW DefaultShaderPS(psRef);
            ps->initialize();
            manager.addPS(ps);
        }

        retVS = vs;
        retPS = ps;
    }
#else
#endif
}
