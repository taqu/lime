/**
@file ResourceSet.cpp
@author t-sakai
@date 2016/11/20 create
*/
#include "resource/ResourceSet.h"
#include <lcore/Sort.h>
#include <lcore/Search.h>

namespace lfw
{
namespace
{
    s32 comp(const Resource::pointer& resource, const u64& id)
    {
        u64 lid = resource->getID();
        return (id<=lid)? (lid==id)? 0 : 1 : -1;
    }

    bool lessEqual(const Resource::pointer& x0, const Resource::pointer& x1)
    {
        return x0->getID()<=x1->getID();
    }
}

    ResourceSet::ResourceSet()
    {
    }

    ResourceSet::ResourceSet(const ResourceSet& resourceSet)
        :resources_(resourceSet.resources_.capacity())
    {
        resources_.resize(resourceSet.resources_.size());
        for(s32 i=0; i<resourceSet.resources_.size(); ++i){
            resources_[i] = resourceSet.resources_[i];
        }
    }

    ResourceSet::ResourceSet(s32 numResources)
        :resources_(numResources)
    {
        LASSERT(0<=numResources);
    }

    ResourceSet::~ResourceSet()
    {
    }

    // ŒŸõ
    s32 ResourceSet::findIndex(u64 id) const
    {
        LASSERT(0<=id);
        return lcore::binarySearchIndex(resources_.size(), resources_.begin(), id, comp);
    }

    // ’Ç‰Á
    bool ResourceSet::add(Resource::pointer& resource)
    {
        LASSERT(resource != NULL);
        if(0<=findIndex(resource->getID())){
            return false;
        }
        const Resource::pointer* p = lcore::upper_bound(begin(), end(), resource, lessEqual);
        s32 index = lcore::indexof(begin(), p);
        resources_.push_back(Resource::pointer());
        for(s32 i=resources_.size()-1; index<i; --i){
            resources_[i] = resources_[i-1];
        }
        resources_[index] = resource;
        return true;
    }

    // íœ
    bool ResourceSet::remove(u64 id)
    {
        LASSERT(0<=id);
        s32 index = findIndex(id);
        if(0<=index){
            resources_.removeAt(index);
            return true;
        }
        return false;
    }

    // ƒNƒŠƒA
    void ResourceSet::clear()
    {
        resources_.clear();
    }

    void ResourceSet::swap(ResourceSet& rhs)
    {
        resources_.swap(rhs.resources_);
    }

    ResourceSet& ResourceSet::operator=(const ResourceSet& rhs)
    {
        ResourceSet(rhs).swap(*this);
        return *this;
    }
}
