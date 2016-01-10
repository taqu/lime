#ifndef INC_LSCENE_OBJECTFACTORYBASE_H__
#define INC_LSCENE_OBJECTFACTORYBASE_H__
/**
@file ObjectFactoryBase.h
@author t-sakai
@date 2014/12/08 create
*/

#include "lscene.h"

namespace lscene
{
namespace lfile
{
    class SharedFile;
}
    class Resource;

    //------------------------------------------------
    //---
    //--- ObjectFactoryBase
    //---
    //------------------------------------------------
    class ObjectFactoryBase : public ObjectFactoryAllocator
    {
    public:
        virtual ~ObjectFactoryBase()
        {}

        virtual Resource* create(s32 type, const Char* path, lfile::SharedFile& file, const SceneParam& param) = 0;

    protected:
        ObjectFactoryBase()
        {}

    private:
        ObjectFactoryBase(const ObjectFactoryBase&);
        ObjectFactoryBase& operator=(const ObjectFactoryBase&);
    };
}

#endif //INC_LSCENE_OBJECTFACTORYBASE_H__
