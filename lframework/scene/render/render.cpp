/**
@file render.cpp
@author t-sakai
@date 2014/12/16 create
*/
#include "render.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Material.h"
#include "ShaderSet.h"
#include "ShaderManager.h"

namespace lscene
{
namespace lrender
{
    ShaderSetFactory::CreateFunc ShaderSetFactory::create_ = ShaderSetFactory::defaultCreateFunc;

    void ShaderSetFactory::defaultCreateFunc(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFactoryFlag flags)
    {
        ShaderManager& shaderManager = ShaderManager::getInstance();

        s32 type = DefaultShaderType_Normal;
        
        const Material* material = mesh.getMaterial();

        if(mesh.hasUV()){
            if(material->hasTexture(lrender::Tex_Albedo) && material->hasTexture(lrender::Tex_Normal)){
                type = DefaultShaderType_NormalTexCN;
            }else if(material->hasTexture(lrender::Tex_Albedo)){
                type = DefaultShaderType_NormalTexC;
            }

            //if(mesh.hasBoneIndex()){
            //    type += DefaultShaderType_NormalSkinning;
            //}

            if(flags & ShaderSetFactoryFlag_Skinning){
                type += DefaultShaderType_NormalSkinning;
            }
        }else{
            type = DefaultShaderType_NormalNoUV;
        }

        bool receiveShadow = material->isRecieveShadow();

        shaderSet.key_ = type;
        switch(type)
        {
        case DefaultShaderType_Normal:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalShadow) : shaderManager.getVS(ShaderVS::ShaderVS_Normal);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalShadow) : shaderManager.getPS(ShaderPS::ShaderPS_Normal);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_Normal);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_Normal);
            break;

        case DefaultShaderType_NormalNoUV:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalNoUVShadow) : shaderManager.getVS(ShaderVS::ShaderVS_NormalNoUV);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalNoUVShadow) : shaderManager.getPS(ShaderPS::ShaderPS_NormalNoUV);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_NormalNoUV);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_NormalNoUV);
            break;

        case DefaultShaderType_NormalTexC:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalShadow) : shaderManager.getVS(ShaderVS::ShaderVS_Normal);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCShadow) : shaderManager.getPS(ShaderPS::ShaderPS_NormalTexC);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_Normal);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_NormalTexC);
            break;

        case DefaultShaderType_NormalTexCN:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalMapShadow) : shaderManager.getVS(ShaderVS::ShaderVS_NormalMap);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCNShadow) : shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCN);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_NormalMap);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCN);
            break;

        case DefaultShaderType_NormalSkinning:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinningShadow) : shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinning);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalShadow) : shaderManager.getPS(ShaderPS::ShaderPS_Normal);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinning);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_Normal);
            break;

        case DefaultShaderType_NormalSkinningTexC:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinningShadow) : shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinning);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCShadow) : shaderManager.getPS(ShaderPS::ShaderPS_NormalTexC);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_NormalSkinning);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_NormalTexC);
            break;

        case DefaultShaderType_NormalSkinningTexCN:
            shaderSet.vs_ = (receiveShadow)? shaderManager.getVS(ShaderVS::ShaderVS_NormalMapSkinningShadow) : shaderManager.getVS(ShaderVS::ShaderVS_NormalMapSkinning);
            shaderSet.gs_ = NULL;
            shaderSet.ps_ = (receiveShadow)? shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCNShadow) : shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCN);

            shaderSet.lodVS_ = shaderManager.getVS(ShaderVS::ShaderVS_NormalMapSkinning);
            shaderSet.lodGS_ = NULL;
            shaderSet.lodPS_ = shaderManager.getPS(ShaderPS::ShaderPS_NormalTexCN);
            break;

        default:
            LASSERT(false);
            break;
        };
    }

    void ShaderSetFactory::setShaderSet(Mesh& mesh, ShaderSetFactoryFlag flags)
    {
        ShaderSet shaderSet;
        create_(shaderSet, mesh, flags);
        ShaderManager& shaderManager = ShaderManager::getInstance();
        mesh.setShaderSet( shaderManager.addShaderSet(shaderSet) );
    }
}
}
