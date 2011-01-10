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
                u16 flag = vflags[ elements[i].usage_ ];
                flag <<= elements[i].usageIndex_;

                flags |= flag;
            }
            return flags;
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
        inputVSFlags <<= 16; //頂点シェーダのフラグは上位ワードへ

        u32 featureVSFlags = geometry.getFlags().get();
        u32 featurePSFlags = material.getFlags().get();

        lgraphics::ShaderKey vsKey(inputVSFlags, featureVSFlags);
        lgraphics::ShaderKey psKey(inputPSFlags, featurePSFlags);

        // すでに登録されているか
        lgraphics::Descriptor* vsDesc = manager.findVS(vsKey);

        lgraphics::Descriptor* psDesc = manager.findPS(psKey);


        //VSないから作るよ
        if(vsDesc == NULL){
            // 頂点フォーマットとピクセルシェーダへの出力、機能によってマクロセット
            const char* macros[Macro_Num];
            u32 numMacros = 0;
            macros[numMacros++] = MacroNames[Macro_VNORMAL];
            macros[numMacros++] = MacroNames[Macro_VTEX0];
            macros[numMacros++] = MacroNames[Macro_VBONE];
            macros[numMacros++] = MacroNames[Macro_VWEIGHT];

            //macros[numMacros++] = MacroNames[Macro_PCOLOR];
            macros[numMacros++] = MacroNames[Macro_PNORMAL];
            if(material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }
            macros[numMacros++] = MacroNames[Macro_PPOS];

            macros[numMacros++] = MacroNames[Macro_VDIFFUSE];

            //スキニングあり？
            if(geometry.getFlags().checkFlag(lscene::Geometry::GeomFlag_Skinning)){
                macros[numMacros++] = MacroNames[Macro_SKINNING];
            }

            vsDesc = lgraphics::Shader::createVertexShaderFromMemory(
                shader::DefaultVSSource,
                shader::DefaultVSSourceSize,
                numMacros,
                macros);
            manager.addVS(vsKey, vsDesc);
        }

        //PSないから作るよ
        if(psDesc == NULL){
            const char* macros[Macro_Num];
            u32 numMacros = 0;
            //macros[numMacros++] = MacroNames[Macro_PCOLOR];
            macros[numMacros++] = MacroNames[Macro_PNORMAL];
            if(material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }
            macros[numMacros++] = MacroNames[Macro_PPOS];

            //macros[numMacros++] = MacroNames[Macro_FRESNEL];

            psDesc = lgraphics::Shader::createPixelShaderFromMemory(
                shader::DefaultPSSource,
                shader::DefaultPSSourceSize,
                numMacros,
                macros);

            manager.addPS(psKey, psDesc);
        }

        ShaderVSBase* vs = NULL;
        ShaderPSBase* ps = NULL;
        if(false == manager.findProgram(vsDesc, psDesc, vs, ps)){
            //プログラムがなかったから作るよ
            lgraphics::VertexShaderRef vsRef;
            lgraphics::PixelShaderRef psRef;
            lgraphics::VertexDeclarationRef& decl = geometry.getGeometryBuffer()->getDecl();
            bool ret = lgraphics::Shader::linkShader(vsRef, psRef, decl, vsDesc, psDesc);
            LASSERT(ret);
            if(ret){
                lcore::Log("Shader success to link");
            }

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
        inputVSFlags <<= 16; //頂点シェーダのフラグは上位ワードへ

        u32 featureVSFlags = geometry.getFlags().get();
        u32 featurePSFlags = material.getFlags().get();

        lgraphics::ShaderKey vsKey(inputVSFlags, featureVSFlags);
        lgraphics::ShaderKey psKey(inputPSFlags, featurePSFlags);

        // すでに登録されているか
        ShaderVSBase *vs = manager.findVS(vsKey);

        ShaderPSBase *ps = manager.findPS(psKey);


        //VSないから作るよ
        if(vs == NULL){
            // 頂点フォーマットとピクセルシェーダへの出力、機能によってマクロセット
            const char* macros[Macro_Num];
            u32 numMacros = 0;
            macros[numMacros++] = MacroNames[Macro_VNORMAL];
            macros[numMacros++] = MacroNames[Macro_VTEX0];
            macros[numMacros++] = MacroNames[Macro_VBONE];
            macros[numMacros++] = MacroNames[Macro_VWEIGHT];

            //macros[numMacros++] = MacroNames[Macro_PCOLOR];
            macros[numMacros++] = MacroNames[Macro_PNORMAL];
            if(material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }
            macros[numMacros++] = MacroNames[Macro_PPOS];

            macros[numMacros++] = MacroNames[Macro_VDIFFUSE];

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
            //macros[numMacros++] = MacroNames[Macro_PCOLOR];
            macros[numMacros++] = MacroNames[Macro_PNORMAL];
            if(material.getTextureNum()>0){
                macros[numMacros++] = MacroNames[Macro_PTEX0];
            }
            macros[numMacros++] = MacroNames[Macro_PPOS];

            //macros[numMacros++] = MacroNames[Macro_FRESNEL];

            lgraphics::PixelShaderRef psRef = lgraphics::Shader::createPixelShaderFromMemory(
                shader::DefaultPSSource,
                shader::DefaultPSSourceSize,
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
