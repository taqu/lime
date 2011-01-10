/**
@file ShaderManager.cpp
@author t-sakai
@date 2009/08/16
*/
#include "ShaderManager.h"

#include <lgraphics/api/ShaderKey.h>
#include <lgraphics/api/GraphicsDeviceRef.h>

#include "ShaderCreator.h"
#include "ShaderBase.h"

namespace lscene
{
    ShaderManager* ShaderManager::instance_ = NULL;

    ShaderManager::ShaderManager(
        u32 size,
        lgraphics::ShaderKey& emptyKey
#if defined(LIME_GLES2)
        ,ProgramKey& emptyProgramKey
#endif
        )
        :vsMap_(size, emptyKey)
        ,psMap_(size, emptyKey)
#if defined(LIME_GLES2)
        ,programMap_(size, emptyProgramKey)
#endif
        ,creator_(LIME_NEW ShaderCreator)
    {
    }

    ShaderManager::~ShaderManager()
    {
#if defined(LIME_GLES2)
        for(ProgramMap::size_type i = programMap_.begin();
            i != programMap_.end();
            i = programMap_.next(i))
        {
            ProgramValue& value = programMap_.getValue(i);
            LIME_DELETE(value.vs_);
            LIME_DELETE(value.ps_);
        }


        for(ShaderMap::size_type i = vsMap_.begin();
            i != vsMap_.end();
            i = vsMap_.next(i))
        {
            lgraphics::Descriptor* desc = vsMap_.getValue(i);
            desc->release();
        }

        for(ShaderMap::size_type i = psMap_.begin();
            i != psMap_.end();
            i = psMap_.next(i))
        {
            lgraphics::Descriptor* desc = psMap_.getValue(i);
            desc->release();
        }

#else
        for(ShaderMap::size_type i = vsMap_.begin();
            i != vsMap_.end();
            i = vsMap_.next(i))
        {
            LIME_DELETE(vsMap_.getValue(i));
        }

        for(ShaderMap::size_type i = psMap_.begin();
            i != psMap_.end();
            i = psMap_.next(i))
        {
            LIME_DELETE(psMap_.getValue(i));
        }
#endif
        LIME_DELETE(creator_);
    }


    void ShaderManager::initialize(u32 size)
    {
        if(instance_ == NULL){
            lgraphics::ShaderKey emptyKey;
#if defined(LIME_GLES2)
            ProgramKey emptyProgramKey;
            instance_ = LIME_NEW ShaderManager(size, emptyKey, emptyProgramKey);
#else
            instance_ = LIME_NEW ShaderManager(size, emptyKey);
#endif
        }
    }

    void ShaderManager::terminate()
    {
        LIME_DELETE(instance_);
    }

#if defined(LIME_GLES2)
    bool ShaderManager::addVS(const lgraphics::ShaderKey& key, lgraphics::Descriptor* shader)
    {
        LASSERT(shader != NULL);

        if(checkVS(key)){
            return false;
        }
        return vsMap_.insert(key, shader);
    }

    bool ShaderManager::addPS(const lgraphics::ShaderKey& key, lgraphics::Descriptor* shader)
    {
        LASSERT(shader != NULL);

        if(checkPS(key)){
            return false;
        }
        return psMap_.insert(key, shader);
    }

    lgraphics::Descriptor* ShaderManager::findVS(const lgraphics::ShaderKey& key)
    {
        ShaderMap::size_type pos = vsMap_.find(key);
        if(vsMap_.isEnd(pos)){
            return NULL;
        }
        return vsMap_.getValue(pos);
    }

    lgraphics::Descriptor* ShaderManager::findPS(const lgraphics::ShaderKey& key)
    {
        ShaderMap::size_type pos = psMap_.find(key);
        if(psMap_.isEnd(pos)){
            return NULL;
        }
        return psMap_.getValue(pos);
    }

    bool ShaderManager::checkVS(const lgraphics::ShaderKey& key) const
    {
        ShaderMap::size_type pos = vsMap_.find(key);
        return (!vsMap_.isEnd(pos));
    }

    bool ShaderManager::checkPS(const lgraphics::ShaderKey& key) const
    {
        ShaderMap::size_type pos = psMap_.find(key);
        return (!psMap_.isEnd(pos));
    }

    bool ShaderManager::addProgram(ShaderVSBase* vs, ShaderPSBase* ps)
    {
        LASSERT(vs != NULL);
        LASSERT(ps != NULL);

        const lgraphics::Descriptor* vsDesc = vs->getShaderRef().getDescriptor();
        const lgraphics::Descriptor* psDesc = ps->getShaderRef().getDescriptor();

        if(checkProgram(vsDesc, psDesc)){
            return false;
        }

        ProgramKey key(vsDesc, psDesc);
        ProgramValue value(vs, ps);
        return programMap_.insert(key, value);
    }

    bool ShaderManager::findProgram(const lgraphics::Descriptor* vsDesc, const lgraphics::Descriptor* psDesc, ShaderVSBase*& vs, ShaderPSBase*& ps)
    {
        ProgramKey key(vsDesc, psDesc);
        ProgramMap::size_type pos = programMap_.find(key);
        if(programMap_.isEnd(pos)){
            return false;
        }
        ProgramValue& value = programMap_.getValue(pos);
        vs = value.vs_;
        ps = value.ps_;
        return true;
    }

    bool ShaderManager::checkProgram(const lgraphics::Descriptor* vs, const lgraphics::Descriptor* ps) const
    {
        ProgramKey key(vs, ps);
        ProgramMap::size_type pos = programMap_.find(key);
        return (!programMap_.isEnd(pos));
    }

#else
    bool ShaderManager::addVS(ShaderVSBase* shader)
    {
        LASSERT(shader != NULL);
        const lgraphics::ShaderKey& key = shader->getKey();

        if(checkVS(key)){
            return false;
        }
        return vsMap_.insert(key, shader);
    }

    bool ShaderManager::addPS(ShaderPSBase* shader)
    {
        LASSERT(shader != NULL);
        const lgraphics::ShaderKey& key = shader->getKey();

        if(checkPS(key)){
            return false;
        }
        return psMap_.insert(key, shader);
    }

    ShaderVSBase* ShaderManager::findVS(const lgraphics::ShaderKey& key)
    {
        ShaderMap::size_type pos = vsMap_.find(key);
        if(vsMap_.isEnd(pos)){
            return NULL;
        }
        return dynamic_cast<lscene::ShaderVSBase*>( vsMap_.getValue(pos) );
    }

    ShaderPSBase* ShaderManager::findPS(const lgraphics::ShaderKey& key)
    {
        ShaderMap::size_type pos = psMap_.find(key);
        if(psMap_.isEnd(pos)){
            return NULL;
        }
        return dynamic_cast<lscene::ShaderPSBase*>( psMap_.getValue(pos) );
    }

    bool ShaderManager::checkVS(const lgraphics::ShaderKey& key) const
    {
        ShaderMap::size_type pos = vsMap_.find(key);
        return (!vsMap_.isEnd(pos));
    }

    bool ShaderManager::checkPS(const lgraphics::ShaderKey& key) const
    {
        ShaderMap::size_type pos = psMap_.find(key);
        return (!vsMap_.isEnd(pos));
    }
#endif


    void ShaderManager::setCreator(ShaderCreator* creator)
    {
        LIME_DELETE(creator_);
        creator_ = creator;
    }

    void ShaderManager::create(Geometry& geometry,
            Material& material,
            ShaderVSBase*& vs,
            ShaderPSBase*& ps)
    {
        LASSERT(creator_ != NULL);
        return creator_->create(geometry, material, *this, vs, ps);
    }

}
