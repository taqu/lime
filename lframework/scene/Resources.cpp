/**
@file Resources.cpp
@author t-sakai
@date 2014/12/08 create
*/
#include "Resources.h"
#include "Resource.h"

namespace lscene
{
    Resources::Resources()
        :entryAllocator_(sizeof(Entry))
    {
        for(s32 i=0; i<NumResourceCategory; ++i){
            idToEntry_[i].initialize(InitialResourceMapSize);
        }
    }

    Resources::~Resources()
    {
        clearAll();
    }

    //------------------------------------------
    Resource* Resources::find(s32 category, u64 id)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        IDToEntryMap::size_type pos = idToEntry_[category].find(id);
        return (idToEntry_[category].end() == pos)? NULL : idToEntry_[category].getValue(pos)->resource_;
    }

    //------------------------------------------
    bool Resources::add(s32 category, u64 id, Resource* resource)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        LASSERT(0<id);
        LASSERT(NULL != resource);
        LASSERT(NULL == find(category, id));

        Entry* entry = (Entry*)entryAllocator_.allocate();
        entry->id_ = id;
        entry->resource_ = resource;
        if(idToEntry_[category].insert(id, entry)){
            resource->addRef();
            return true;
        }
        entryAllocator_.deallocate(entry);
        return false;
    }

    //------------------------------------------
    void Resources::remove(s32 category, Resource* resource)
    {
        LASSERT(0<=category && category<NumResourceCategory);
        IDToEntryMap& idToEntry = idToEntry_[category];
        for(IDToEntryMap::size_type itr = idToEntry.begin();
            itr != idToEntry.end();
            itr = idToEntry.next(itr))
        {
            Entry* entry = idToEntry.getValue(itr);
            if(resource == entry->resource_){
                destroy(entry);
                idToEntry.erase(itr);
                return;
            }
        }
    }

    //------------------------------------------
    void Resources::clear(s32 category)
    {
        LASSERT(0<=category && category<NumResourceCategory);

        IDToEntryMap& idToEntry = idToEntry_[category];
        for(IDToEntryMap::size_type itr = idToEntry.begin();
            itr != idToEntry.end();
            itr = idToEntry.next(itr))
        {
            Entry* entry = idToEntry.getValue(itr);
            destroy(entry);
        }
        idToEntry.clear();
    }

    //------------------------------------------
    void Resources::clearAll()
    {
        for(s32 i=0; i<NumResourceCategory; ++i){
            clear(i);
        }
    }

    //------------------------------------------
    void Resources::destroy(Entry* entry)
    {
        LASSERT(NULL != entry);
        LASSERT(NULL != entry->resource_);

        entry->resource_->release();
        entry->resource_ = NULL;
        entryAllocator_.deallocate(entry);
    }
}
