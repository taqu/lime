#ifndef INC_LSCENE_OBJECTFACTORY_H__
#define INC_LSCENE_OBJECTFACTORY_H__
/**
@file ObjectFactory.h
@author t-sakai
@date 2014/12/08 create
*/

#include "ObjectFactoryBase.h"
#include "load/ModelLoader.h"

namespace lscene
{
    //------------------------------------------------
    //---
    //--- ObjectFactory
    //---
    //------------------------------------------------
    class ObjectFactory : public ObjectFactoryBase
    {
    public:
        ObjectFactory();
        virtual ~ObjectFactory();

        virtual Resource* create(s32 type, const Char* path, lfile::SharedFile& file);

    private:
        ObjectFactory(const ObjectFactory&);
        ObjectFactory& operator=(const ObjectFactory&);

        lload::ModelLoader modelLoader_;
    };
}

#endif //INC_LSCENE_OBJECTFACTORY_H__
