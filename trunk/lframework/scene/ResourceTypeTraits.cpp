/**
@file ResourceTypeTraits.cpp
@author t-sakai
@date 2014/12/10 create
*/
#include "ResourceTypeTraits.h"

namespace lscene
{
    void ResourceTypeTraitsUnknown::destroy(void* resource)
    {
        ResourceUnknown* unknown = reinterpret_cast<ResourceUnknown*>(resource);
        if(0 == unknown->decRefCount()){
            LSCENE_DELETE(unknown);
        }
    }
}
