/**
@file Resources.cpp
@author t-sakai
@date 2016/11/20 create
*/
#include "resource/Resources.h"
#include <lcore/io/VirtualFileSystem.h>
#include "resource/ResourceBytes.h"
#include "resource/ResourceTexture2D.h"
#include "resource/ResourceModel.h"
#include "resource/ResourceAnimation.h"

namespace lfw
{
    Resources* Resources::instance_ = NULL;

    bool Resources::initialize(s32 numSets, lcore::FileSystem* fileSystem)
    {
        if(NULL != instance_){
            return true;
        }
        numSets = lcore::maximum(1, numSets);
        instance_ = LNEW Resources(numSets, fileSystem);
        return true;
    }

    void Resources::terminate()
    {
        LDELETE(instance_);
    }

    Resources::Resources(s32 numSets, lcore::FileSystem* fileSystem)
        :resourceSets_(numSets)
        ,modelLoader_(fileSystem)
        ,fileSystem_(fileSystem)
        ,bufferSize_(0)
        ,buffer_(NULL)
    {
        LASSERT(0<=numSets);
        LASSERT(NULL != fileSystem_);
        addSet(DefaultSet);
    }

    Resources::~Resources()
    {
        LDELETE_ARRAY(buffer_);
    }

    // 取得
    const ResourceSet& Resources::getSet(s32 setID)
    {
        LASSERT(0<=setID && setID<resourceSets_.size());
        return resourceSets_[setID];
    }

    // 追加
    const ResourceSet& Resources::addSet(s32 setID)
    {
        LASSERT(0<=setID);
        if(resourceSets_.size()<=setID){
            resourceSets_.resize(setID+1);
        }
        return resourceSets_[setID];
    }

    // 検索
    bool Resources::existsInSet(s32 setID, u64 resID) const
    {
        LASSERT(0<=setID);
        LASSERT(0<resID);
        return 0 <= resourceSets_[setID].findIndex(resID);
    }

    // 追加
    bool Resources::addInSet(s32 setID, Resource::pointer& resource)
    {
        LASSERT(0<=setID && setID<resourceSets_.size());
        LASSERT(NULL != resource);
        if(resourceSets_[setID].add(resource)){
            resources_.insert(resource->getID(), resource);
            return true;
        }
        return false;
    }

    // 削除
    bool Resources::removeInSet(s32 setID, u64 resID)
    {
        LASSERT(0<=setID && setID<resourceSets_.size());
        LASSERT(0<resID);
        if(resourceSets_[setID].remove(resID)){
            u32 pos = resources_.find(resID);
            if(resources_.end() == pos){
                return true;
            }
            //参照が1以下なら削除
            Resource::pointer& resource = resources_.getValue(pos);
            if(resource->getReferenceCount()<=1){
                resources_.erase(resID);
            }
            return true;
        }
        return false;
    }

    // クリア
    void Resources::removeAllInSet(s32 setID)
    {
        LASSERT(0<=setID && setID<resourceSets_.size());
        for(const Resource::pointer* itr = resourceSets_[setID].begin();
            itr != resourceSets_[setID].end();
            ++itr)
        {
            resources_.erase((*itr)->getID());
        }
        resourceSets_[setID].clear();
    }

    // 追加
    bool Resources::addUnique(s32 setID, Resource::pointer& resource)
    {
        LASSERT(0<=setID && setID<resourceSets_.size());
        LASSERT(NULL != resource);
        u64 id = resource->getID();
        for(s32 i=0; i<resourceSets_.size(); ++i){
            if(0<=resourceSets_[i].findIndex(id)){
                return false;
            }
        }

        if(resourceSets_[setID].add(resource)){
            resources_.insert(id, resource);
            return true;
        }
        return false;
    }

    // クリア
    void Resources::removeAll()
    {
        for(s32 i=0; i<resourceSets_.size(); ++i){
            removeAllInSet(i);
        }
    }

    // 検索
    bool Resources::exists(u64 resID) const
    {
        return (resources_.end() != resources_.find(resID));
    }

    Resource::pointer Resources::load(const Char* path, ResourceType type)
    {
        LASSERT(NULL != path);

        u64 id = calcHash(path);

        //すでにあるか検索
        u32 pos = resources_.find(id);
        if(resources_.end() != pos){
            for(s32 i=0; i<resourceSets_.size(); ++i){
                if(0 <= resourceSets_[i].findIndex(id)){
                    return resources_.getValue(pos);
                }
            }
            addInSet(0, resources_.getValue(pos));
            return resources_.getValue(pos);
        }

        Resource* resource = loadInternal(DefaultSet, path, type);
        if(NULL == resource){
            return Resource::pointer();
        }
        resource->setID(id);
        return Resource::pointer(resource);
    }

    Resource::pointer Resources::load(s32& setID, const Char* path, ResourceType type)
    {
        LASSERT(NULL != path);
        LASSERT(0<=setID && setID<resourceSets_.size());

        u64 id = calcHash(path);

        //すでにあるか検索
        u32 pos = resources_.find(id);
        if(resources_.end() != pos){
            for(s32 i=0; i<resourceSets_.size(); ++i){
                if(0 <= resourceSets_[i].findIndex(id)){
                    setID = i;
                    return resources_.getValue(pos);
                }
            }
            setID = 0;
            addInSet(0, resources_.getValue(pos));
            return resources_.getValue(pos);
        }

        Resource* resource = loadInternal(setID, path, type);
        if(NULL == resource){
            return Resource::pointer();
        }
        resource->setID(id);
        Resource::pointer p = Resource::pointer(resource);
        addInSet(setID, p);
        return p;
    }

    Resource::pointer Resources::load(const Char* path, ResourceType type, const TextureParameter& param)
    {
        LASSERT(NULL != path);

        u64 id = calcHash(path);

        //すでにあるか検索
        u32 pos = resources_.find(id);
        if(resources_.end() != pos){
            for(s32 i=0; i<resourceSets_.size(); ++i){
                if(0 <= resourceSets_[i].findIndex(id)){
                    return resources_.getValue(pos);
                }
            }
            addInSet(0, resources_.getValue(pos));
            return resources_.getValue(pos);
        }

        Resource* resource = loadInternal(path, type, param);
        if(NULL == resource){
            return Resource::pointer();
        }
        resource->setID(id);
        return Resource::pointer(resource);
    }

    Resource::pointer Resources::load(s32& setID, const Char* path, ResourceType type, const TextureParameter& param)
    {
        LASSERT(NULL != path);
        LASSERT(0<=setID && setID<resourceSets_.size());

        u64 id = calcHash(path);

        //すでにあるか検索
        u32 pos = resources_.find(id);
        if(resources_.end() != pos){
            for(s32 i=0; i<resourceSets_.size(); ++i){
                if(0 <= resourceSets_[i].findIndex(id)){
                    setID = i;
                    return resources_.getValue(pos);
                }
            }
            setID = 0;
            addInSet(0, resources_.getValue(pos));
            return resources_.getValue(pos);
        }

        Resource* resource = loadInternal(path, type, param);
        if(NULL == resource){
            return Resource::pointer();
        }
        resource->setID(id);
        Resource::pointer p = Resource::pointer(resource);
        addInSet(setID, p);
        return p;
    }

    void Resources::expand(s64 size)
    {
        if(size<=bufferSize_){
            return;
        }
        LDELETE_ARRAY(buffer_);
        buffer_ = LNEW u8[size];
        bufferSize_ = size;
    }

    Resource* Resources::loadInternal(s32 setID, const Char* path, ResourceType type)
    {
        Resource* resource = NULL;
        switch(type)
        {
        case ResourceType_Bytes:
        {
            lcore::FileProxy* file = fileSystem_->openFile(path);
            if(NULL != file){
                s64 size = file->getUncompressedSize();
                u8* bytes = LNEW u8[size];
                file->read(0, size, bytes);
                fileSystem_->closeFile(file);
                resource = ResourceBytes::load(size, bytes);
            }
        }
        break;
        case ResourceType_Texture2D:
            LASSERT(false);
            return NULL;

        case ResourceType_Model:
        {
            resource = ResourceModel::load(setID, path, modelLoader_);
            if(0<=setID && NULL != resource){
                for(s32 i=0; i<modelLoader_.getNumTexture(); ++i){
                    addInSet(setID, modelLoader_.getTexture(i));
                }
            }
            modelLoader_.clearTextures();
        }
        break;
        case ResourceType_Animation:
            break;
        default:
            return NULL;
        }
        return resource;
    }

    Resource* Resources::loadInternal(const Char* path, ResourceType type, const TextureParameter& param)
    {
        Resource* resource = NULL;
        switch(type)
        {
        case ResourceType_Texture2D:
        {
            lcore::FileProxy* file = fileSystem_->openFile(path);
            if(NULL != file){
                s64 size = file->getUncompressedSize();
                expand(size);
                file->read(0, size, buffer_);
                fileSystem_->closeFile(file);
                resource = ResourceTexture2D::load(path, size, buffer_, param);
            }
        }
            break;
        default:
            LASSERT(false);
            return NULL;
        }
        return resource;
    }
}
