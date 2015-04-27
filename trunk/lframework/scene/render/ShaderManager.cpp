/**
@file ShaderManager.cpp
@author t-sakai
@date 2014/12/15 create
*/
#include "ShaderManager.h"
#include <lcore/Search.h>
#include <lgraphics/api/ShaderRef.h>

namespace lscene
{
namespace lrender
{
    ShaderManager* ShaderManager::instance_ = NULL;

    void ShaderManager::initialize(s32 numVS, s32 numGS, s32 numPS, s32 numCS, s32 numDS, s32 numHS)
    {
        if(NULL == instance_){
            instance_ = LSCENE_NEW ShaderManager(numVS, numGS, numPS, numCS, numDS, numHS);
        }
    }

    void ShaderManager::terminate()
    {
        LSCENE_DELETE(instance_);
    }

    ShaderManager::ShaderManager(s32 numVS, s32 numGS, s32 numPS, s32 numCS, s32 numDS, s32 numHS)
        :numVS_(numVS)
        ,numGS_(numGS)
        ,numPS_(numPS)
        ,numCS_(numCS)
        ,numDS_(numDS)
        ,numHS_(numHS)
        ,vs_(NULL)
        ,gs_(NULL)
        ,ps_(NULL)
        ,cs_(NULL)
        ,ds_(NULL)
        ,hs_(NULL)
    {
        create(&vs_, numVS_);
        create(&gs_, numGS_);
        create(&ps_, numPS_);
        create(&cs_, numCS_);
        create(&ds_, numDS_);
        create(&hs_, numHS_);
    }

    ShaderManager::~ShaderManager()
    {
        for(s32 i=0; i<shaderSets_.size(); ++i){
            LSCENE_DELETE(shaderSets_[i]);
        }

        destroy(&hs_, numHS_);
        destroy(&ds_, numDS_);
        destroy(&cs_, numCS_);
        destroy(&ps_, numPS_);
        destroy(&gs_, numGS_);
        destroy(&vs_, numVS_);
    }

    lgraphics::VertexShaderRef* ShaderManager::getVS(s32 id)
    {
        LASSERT(0<=id && id<numVS_);
        return &vs_[id];
    }

    void ShaderManager::setVS(s32 id, lgraphics::VertexShaderRef& vs)
    {
        LASSERT(0<=id && id<numVS_);
        vs_[id] = vs;
    }

    lgraphics::GeometryShaderRef* ShaderManager::getGS(s32 id)
    {
        LASSERT(0<=id && id<numGS_);
        return &gs_[id];
    }

    void ShaderManager::setGS(s32 id, lgraphics::GeometryShaderRef& gs)
    {
        LASSERT(0<=id && id<numGS_);
        gs_[id] = gs;
    }

    lgraphics::PixelShaderRef* ShaderManager::getPS(s32 id)
    {
        LASSERT(0<=id && id<numPS_);
        return &ps_[id];
    }

    void ShaderManager::setPS(s32 id, lgraphics::PixelShaderRef& ps)
    {
        LASSERT(0<=id && id<numPS_);
        ps_[id] = ps;
    }

    lgraphics::ComputeShaderRef* ShaderManager::getCS(s32 id)
    {
        LASSERT(0<=id && id<numCS_);
        return &cs_[id];
    }

    void ShaderManager::setCS(s32 id, lgraphics::ComputeShaderRef& cs)
    {
        LASSERT(0<=id && id<numCS_);
        cs_[id] = cs;
    }

    lgraphics::DomainShaderRef* ShaderManager::getDS(s32 id)
    {
        LASSERT(0<=id && id<numDS_);
        return &ds_[id];
    }

    void ShaderManager::setDS(s32 id, lgraphics::DomainShaderRef& ds)
    {
        LASSERT(0<=id && id<numDS_);
        ds_[id] = ds;
    }

    lgraphics::HullShaderRef* ShaderManager::getHS(s32 id)
    {
        LASSERT(0<=id && id<numHS_);
        return &hs_[id];
    }

    void ShaderManager::setHS(s32 id, lgraphics::HullShaderRef& hs)
    {
        LASSERT(0<=id && id<numHS_);
        hs_[id] = hs;
    }

    ShaderSet* ShaderManager::addShaderSet(const ShaderSet& set)
    {
        ShaderSet* shaderSet = findShaderSet(set.key_);
        if(NULL != shaderSet){
            return shaderSet;
        }

        //‘}“üƒ\[ƒg
        s32 size = shaderSets_.size();
        shaderSets_.resize(size+1);
        s32 index=0;
        for(s32 i=0; i<size; ++i){
            if(shaderSets_[i]->key_<set.key_){
                index = i+1;
            }
        }

        for(s32 i=size; index<i; --i){
            shaderSets_[i] = shaderSets_[i-1];
        }
        shaderSets_[index] = LSCENE_NEW ShaderSet(set);

        return shaderSets_[index];
    }

    ShaderSet* ShaderManager::findShaderSet(s32 key)
    {
        s32 index = lcore::binarySearchIndex(shaderSets_.size(), shaderSets_.begin(), key, CompFunc());
        return (0<=index)? shaderSets_[index] : NULL;
    }

    s32 ShaderManager::CompFunc::operator()(const ShaderSet* v0, const s32& key) const
    {
        return v0->key_ - key;
    }
}
}
