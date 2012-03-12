/**
@file ResourceDB.cpp
@author t-sakai
@date 2011/01/09
*/
#include "ResourceDB.h"

namespace lresource
{
    ResourceDB::ResourceDB()
        :nameToTexture_(DefaultSize)
    {
    }

    ResourceDB::~ResourceDB()
    {
    }


    bool ResourceDB::addTexture(lgraphics::TextureRef& texture)
    {
        return nameToTexture_.insert(texture.getName().c_str(), texture.getName().size(), texture);
    }

    void ResourceDB::eraseTexture(lgraphics::TextureRef& texture)
    {
        nameToTexture_.erase(texture.getName().c_str(), texture.getName().size());
    }

    lgraphics::TextureRef* ResourceDB::findTexture(const Char* name, u32 length)
    {
        NameToTextureMap::size_type pos = nameToTexture_.find(name, length);
        if(pos == nameToTexture_.end()){
            return NULL;
        }
        return &(nameToTexture_.getValue(pos));
    }
}
