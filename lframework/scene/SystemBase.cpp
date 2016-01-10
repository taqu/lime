/**
@file SystemBase.cpp
@author t-sakai
@date 2014/10/08 create
*/
#include "SystemBase.h"
#include "ObjectFactory.h"
#include "Resource.h"
#include "Resources.h"
#include "SceneManager.h"
#include "file/FileSystem.h"
#include "./render/InputLayoutManager.h"
#include "./render/ShaderManager.h"

namespace lscene
{
    //--------------------------------------------------
    //---
    //--- SystemBase
    //---
    //--------------------------------------------------
    SystemBase::SystemBase()
        :fileSystem_(NULL)
        ,objectFactory_(NULL)
        ,resources_(NULL)
        ,wcsSize_(0)
        ,wcsSharedBuffer_(NULL)
    {
    }

    SystemBase::~SystemBase()
    {
    }

    bool SystemBase::initialize(const InitParam& param)
    {
        switch(param.locale_)
        {
        case Locale_SystemDefault:
            lcore::setLocale("");
            break;
        }

        SceneAllocator::malloc_ = param.allocFunc_;
        SceneAllocator::mallocDebug_ = param.allocFuncDebug_;
        SceneAllocator::free_ = param.freeFunc_;
        SceneAllocator::alignedMalloc_ = param.alignedAllocFunc_;
        SceneAllocator::alignedMallocDebug_ = param.alignedAllocFuncDebug_;
        SceneAllocator::alignedFree_ = param.alignedFreeFunc_;

        SystemInstance::set(this);

        lrender::InputLayoutManager::initialize();

        if(NULL != param.objectFactory_){
            objectFactory_ = param.objectFactory_;
        }else{
            objectFactory_ = LSCENE_NEW ObjectFactory();
        }

        resources_ = LSCENE_NEW Resources();

        frameSync_.initialize();
        constantBuffer_.initialize(param.numConstantTables_);
        transientVertexBuffer_.initialize();
        transientIndexBuffer_.initialize();

        //Shader Manager
        lrender::ShaderManager::initialize(param.numVS_, param.numGS_, param.numPS_, param.numCS_, param.numDS_, param.numHS_);

        return true;
    }

    void SystemBase::terminate()
    {
        //Shader Manager
        lrender::ShaderManager::terminate();

        transientIndexBuffer_.terminate();
        transientVertexBuffer_.terminate();
        constantBuffer_.terminate();
        frameSync_.terminate();

        LSCENE_DELETE(resources_);
        LSCENE_DELETE(objectFactory_);
        LSCENE_DELETE(fileSystem_);

        lrender::InputLayoutManager::terminate();

        wcsSize_ = 0;
        LSCENE_FREE(wcsSharedBuffer_);
        SystemInstance::set(NULL);

        SceneAllocator::malloc_ = NULL;
        SceneAllocator::free_ = NULL;
    }

    bool SystemBase::openFileSystem(const Char* path, bool isVirtual)
    {
        lfile::FileSystemBase* fileSystem = NULL;
        if(isVirtual){
            fileSystem = LSCENE_NEW lfile::VirtualFileSystem();
        }else{
            fileSystem = LSCENE_NEW lfile::FileSystem();
        }

        if(!fileSystem->mount(path)){
            LSCENE_DELETE(fileSystem);
            return false;
        }

        LSCENE_DELETE(fileSystem_);
        fileSystem_ = fileSystem;
        return true;
    }

    s32 SystemBase::MBSToWCSSharedBuffer(const WChar*& dst, const Char* src)
    {
        s32 required = lcore::MBSToWCS(NULL, 0, src);
        if(required<0){
            return required;
        }

        if(wcsSize_<=required){
            wcsSize_ = required+1;
            LSCENE_FREE(wcsSharedBuffer_);
            wcsSharedBuffer_ = (WChar*)LSCENE_MALLOC(sizeof(WChar)*wcsSize_);
        }

        dst = wcsSharedBuffer_;
        return lcore::MBSToWCS(wcsSharedBuffer_, wcsSize_, src);
    }

    void SystemBase::clearResource(s32 category)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        LASSERT(NULL != resources_);
        resources_->clear(category);
    }

    void SystemBase::clearResources()
    {
        LASSERT(NULL != resources_);
        resources_->clearAll();
    }

    Resource* SystemBase::getResource(s32 category, s32 type, const Char* path)
    {
        return getResource(category, type, path, defaultParameter_);
    }

    Resource* SystemBase::getResource(s32 category, s32 type, const Char* path, const SceneParam& param)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        LASSERT(NULL != path);
        LASSERT(NULL != objectFactory_);
        LASSERT(NULL != resources_);

        lfile::SharedFile file = fileSystem_->open(path);
        if(!file.is_open()){
            return NULL;
        }
        Resource* resource = resources_->find(category, file.getID());
        if(NULL != resource){
            return resource;
        }
        resource = objectFactory_->create(type, path, file, param);
        if(NULL == resource){
            return NULL;
        }

        resource->addRef();
        if(resources_->add(category, file.getID(), resource)){
            resource->release();
            return resource;
        }
        resource->release();
        return NULL;
    }

    void SystemBase::removeResource(s32 category, Resource* resource)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        LASSERT(NULL != resources_);
        if(NULL == resource){
            return;
        }
        resources_->remove(category, resource);
    }

    void SystemBase::initializeDefaultRenderPath(RenderQueue& queue)
    {
        queue.setPathToIndex(Path_Shadow, 0);
        queue.setPathToIndex(Path_Opaque, 1);
        queue.setPathToIndex(Path_Transparent, 2);
        queue.setPathToIndex(Path_MotionVelocity, 3);
        queue.setPathToIndex(Path_Effect, 4);
        queue.setPathToIndex(Path_2D, 5);
    }

    void SystemBase::initializeSceneManager()
    {
        SceneManager::getInstance().initialize();
    }

    void SystemBase::terminateSceneManager()
    {
        SceneManager::getInstance().terminate();
    }
}
