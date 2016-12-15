/**
@file ShaderManager.cpp
@author t-sakai
@date 2016/11/20 create
*/
#include "resource/ShaderManager.h"
#include <lcore/io/VirtualFileSystem.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/BlobRef.h>
#include "render/ShaderSet.h"
#include "render/Mesh.h"
#include "render/Material.h"

namespace lfw
{
#define LSHADERMANAGER_LOADFILE \
size = load(file);\
if(size<=0){\
    return false;\
}\

#define LSHADERMANAGER_LOADBLOB(BUFFER)\
size = unpack(size, (BUFFER));\
if(size<=0){\
    return false;\
}\

#define LSHADERMANAGER_COMPILE(TYPE,VAR,BUFFER)\
    lgfx::##TYPE##ShaderRef VAR = lgfx::Shader::create##TYPE##ShaderFromBinary((BUFFER), size);\
    if(!##VAR##.valid()){\
        return false;\
    }\
    if(VAR##_.size()<=id){\
        VAR##_.resize(id+1);\
    }\
    VAR##_[id] = VAR;\
    return true;\

#define LSHADERMANAGER_DEINE(TYPE,SAFIX,VAR)\
    lgfx::##TYPE##ShaderRef& ShaderManager::get##SAFIX##(s32 id)\
    {\
        LASSERT(0<=id && id<##VAR##_.size());\
        return VAR##_[id];\
    }\
    void ShaderManager::add##SAFIX##(s32 id, lgfx::##TYPE##ShaderRef& VAR)\
    {\
        LASSERT(0<=id);\
        if(VAR##_.size()<=id){\
            VAR##_.resize(id+1);\
        }\
        VAR##_[id] = VAR;\
    }\
    bool ShaderManager::load##SAFIX##(s32 id, lcore::FileProxy& file)\
    {\
        LASSERT(0<=id);\
        s32 size;\
        LSHADERMANAGER_LOADFILE;\
        LSHADERMANAGER_LOADBLOB(source_);\
        LSHADERMANAGER_COMPILE(TYPE, VAR, buffer_);\
    }\
    bool ShaderManager::load##SAFIX##(s32 id, s32 size, const u8* mem)\
    {\
        LASSERT(0<=id);\
        LSHADERMANAGER_LOADBLOB(mem);\
        LSHADERMANAGER_COMPILE(TYPE, VAR, buffer_);\
    }\
    void ShaderManager::release##SAFIX##(s32 id)\
    {\
        LASSERT(0<=id);\
        if(id<##VAR##_.size()){\
            VAR##_[id] = lgfx::##TYPE##ShaderRef();\
        }\
    }\


    ShaderManager::ShaderManager()
        :sourceSize_(0)
        ,source_(NULL)
        ,bufferSize_(0)
        ,buffer_(NULL)
        ,shaderSetCreateFunc_(NULL)
        ,shaderSetsSize_(0)
        ,shaderSets_(NULL)
    {
    }

    ShaderManager::~ShaderManager()
    {
        LDELETE_ARRAY(buffer_);
        LDELETE_ARRAY(source_);

        for(s32 i=0; i<shaderSetsSize_; ++i){
            LDELETE(shaderSets_[i]);
        }
        LDELETE_ARRAY(shaderSets_);
    }

    bool ShaderManager::loadDefaultShaders()
    {
        bool result = true;
        for(s32 i=0; i<ShaderVS_Num; ++i){
            result &= loadVS(i, DefaultShaderCompiledBytes::getSizeVS(i), DefaultShaderCompiledBytes::getBytesVS(i));
        }
        for(s32 i=0; i<ShaderGS_Num; ++i){
            result &= loadGS(i, DefaultShaderCompiledBytes::getSizeGS(i), DefaultShaderCompiledBytes::getBytesGS(i));
        }
        for(s32 i=0; i<ShaderPS_Num; ++i){
            result &= loadPS(i, DefaultShaderCompiledBytes::getSizePS(i), DefaultShaderCompiledBytes::getBytesPS(i));
        }
        return result;
    }

    LSHADERMANAGER_DEINE(Vertex,VS,vs)
    LSHADERMANAGER_DEINE(Domain,DS,ds)
    LSHADERMANAGER_DEINE(Hull,HS,hs)
    LSHADERMANAGER_DEINE(Geometry,GS,gs)
    LSHADERMANAGER_DEINE(Pixel,PS,ps)
    LSHADERMANAGER_DEINE(Compute,CS,cs)

    bool ShaderManager::loadGSStreamOutput(
            s32 id,
            lcore::FileProxy& file,
            const lgfx::StreamOutputDeclarationEntry* entries,
            u32 numEntries,
            const u32* strides,
            u32 numStrides)
    {
        LASSERT(0<=id);
        s32 size;
        LSHADERMANAGER_LOADFILE;
        LSHADERMANAGER_LOADBLOB(source_);
        lgfx::GeometryShaderRef gs = lgfx::Shader::createGeometryShaderWithStreamOutputFromBinary(
            buffer_,
            size,
            entries,
            numEntries,
            strides,
            numStrides,
            0);
        if(!gs.valid()){
            return false;
        }
        if(gs_.size()<=id){
            gs_.resize(id+1);
        }
        gs_[id] = gs;
        return true;
    }

    bool ShaderManager::loadGSStreamOutput(
        s32 id,
        s32 size,
        const u8* mem,
        const lgfx::StreamOutputDeclarationEntry* entries,
        u32 numEntries,
        const u32* strides,
        u32 numStrides)
    {
        LASSERT(0<=id);
        LSHADERMANAGER_LOADBLOB(mem);
        lgfx::GeometryShaderRef gs = lgfx::Shader::createGeometryShaderWithStreamOutputFromBinary(
            buffer_,
            size,
            entries,
            numEntries,
            strides,
            numStrides,
            0);
        if(!gs.valid()){
            return false;
        }
        if(gs_.size()<=id){
            gs_.resize(id+1);
        }
        gs_[id] = gs;
        return true;
    }

    void ShaderManager::releaseTemporaryResources()
    {
        LDELETE_ARRAY(buffer_);
        bufferSize_ = 0;
        LDELETE_ARRAY(source_);
        sourceSize_ = 0;
    }

    void ShaderManager::setShaderSetCreateFunc(ShaderSetCreateFunc func)
    {
        shaderSetCreateFunc_ = func;
    }

    void ShaderManager::setShaderSet(Mesh& mesh, ShaderSetFlag flags)
    {
        ShaderSet shaderSet;
        if(NULL != shaderSetCreateFunc_){
            shaderSetCreateFunc_(shaderSet, mesh, flags);
        } else{
            defaultShaderSetCreateFunc(shaderSet, mesh, flags);
        }
        mesh.setShaderSet(addShaderSet(shaderSet));
    }

    s32 ShaderManager::load(lcore::FileProxy& file)
    {
        s32 size = static_cast<s32>(file.getUncompressedSize());
        if(size<sourceSize_){
            sourceSize_ = incSize(size);
            LDELETE_ARRAY(source_);
            source_ = LNEW u8[sourceSize_];
        }
        return static_cast<s32>(file.read(0, size, source_));
    }

    s32 ShaderManager::unpack(s32 size, const u8* mem)
    {
        lgfx::BlobPackInfo info;
        if(!lgfx::getPackBlobStatus(info, size, mem)){
            return 0;
        }
        if(bufferSize_<info.uncompressedSize_){
            bufferSize_ = incSize(info.uncompressedSize_);
            LDELETE_ARRAY(buffer_);
            buffer_ = LNEW u8[bufferSize_];
        }
        return lgfx::unpackBlob(bufferSize_, buffer_, size, mem);
    }

    ShaderSet* ShaderManager::addShaderSet(ShaderSet& shaderSet)
    {
        LASSERT(0<=shaderSet.id_);
        if(shaderSetsSize_<=shaderSet.id_){
            s32 size = shaderSetsSize_ + 32;
            ShaderSet** sets = LNEW ShaderSet*[size];
            for(s32 i=0; i<shaderSetsSize_; ++i){
                sets[i] = shaderSets_[i];
            }
            for(s32 i=shaderSetsSize_; i<size; ++i){
                sets[i] = NULL;
            }
            LDELETE_ARRAY(shaderSets_);
            shaderSetsSize_ = size;
            shaderSets_ = sets;
        }
        if(NULL == shaderSets_[shaderSet.id_]){
            shaderSets_[shaderSet.id_] = LNEW ShaderSet(shaderSet);
        }
        return shaderSets_[shaderSet.id_];
    }

    void ShaderManager::defaultShaderSetCreateFunc(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFlag flags)
    {
        const Material* material = mesh.getMaterial();
        if(material->isTransparent()){
            s32 type = ShaderSetType_Forward;
            if(mesh.hasUV()){
                type = ShaderSetType_ForwardUV;

                if(material->hasTexture(TextureType::TextureType_Albedo) && material->hasTexture(TextureType::TextureType_Normal)){
                    type = ShaderSetType_ForwardTexCN;
                } else if(material->hasTexture(TextureType::TextureType_Albedo)){
                    type = ShaderSetType_ForwardTexC;
                }
            }
            if(flags & ShaderSetFlag_Skinning){
                type += ShaderSetType_ForwardSkinning-ShaderSetType_Forward;
            }

            bool receiveShadow = material->isRecieveShadow();
            if(receiveShadow){
                type += ShaderSetType_ForwardShadow-ShaderSetType_Forward;
            }

            shaderSet.id_ = type;
            switch(type)
            {
            case ShaderSetType_Forward:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_Forward);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_Forward);
                break;

            case ShaderSetType_ForwardUV:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardUV);
                break;

            case ShaderSetType_ForwardTexC:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexC);
                break;

            case ShaderSetType_ForwardTexCN:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardNUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCN);
                break;

            case ShaderSetType_ForwardSkinning:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_Forward);
                break;

            case ShaderSetType_ForwardUVSkinning:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardUV);
                break;

            case ShaderSetType_ForwardSkinningTexC:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexC);
                break;

            case ShaderSetType_ForwardSkinningTexCN:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardNUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCN);
                break;

            case ShaderSetType_ForwardShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardShadow);
                break;

            case ShaderSetType_ForwardUVShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardUVShadow);
                break;

            case ShaderSetType_ForwardTexCShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCShadow);
                break;

            case ShaderSetType_ForwardTexCNShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardNUVShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCNShadow);
                break;

            case ShaderSetType_ForwardSkinningShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardSkinningShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardShadow);
                break;

            case ShaderSetType_ForwardUVSkinningShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVSkinningShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardUVShadow);
                break;

            case ShaderSetType_ForwardSkinningTexCShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardUVSkinningShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCShadow);
                break;

            case ShaderSetType_ForwardSkinningTexCNShadow:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_ForwardNUVSkinningShadow);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_ForwardTexCNShadow);
                break;

            default:
                LASSERT(false);
                break;
            };
            return;

        }else{//if(material->isTransparent())
            s32 type = ShaderSetType_Deferred;
            if(mesh.hasUV()){
                type = ShaderSetType_DeferredUV;

                if(material->hasTexture(TextureType::TextureType_Albedo) && material->hasTexture(TextureType::TextureType_Normal)){
                    type = ShaderSetType_DeferredTexCN;
                } else if(material->hasTexture(TextureType::TextureType_Albedo)){
                    type = ShaderSetType_DeferredTexC;
                }
            }
            if(flags & ShaderSetFlag_Skinning){
                type += ShaderSetType_DeferredSkinning-ShaderSetType_Deferred;
            }

            shaderSet.id_ = type;
            switch(type)
            {
            case ShaderSetType_Deferred:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_Deferred);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_Deferred);
                break;

            case ShaderSetType_DeferredUV:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredUV);
                break;

            case ShaderSetType_DeferredTexC:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredTexC);
                break;

            case ShaderSetType_DeferredTexCN:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredNUV);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredTexCN);
                break;

            case ShaderSetType_DeferredSkinning:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_Deferred);
                break;

            case ShaderSetType_DeferredUVSkinning:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredUV);
                break;

            case ShaderSetType_DeferredSkinningTexC:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredTexC);
                break;

            case ShaderSetType_DeferredSkinningTexCN:
                shaderSet.vs_ = getVS(ShaderVS::ShaderVS_DeferredNUVSkinning);
                shaderSet.ps_ = getPS(ShaderPS::ShaderPS_DeferredTexCN);
                break;

            default:
                LASSERT(false);
                break;
            };
        }//if(material->isTransparent())
    }
}
