#ifndef INC_LFRAMEWORK_COMPONENTLOGICAL_H__
#define INC_LFRAMEWORK_COMPONENTLOGICAL_H__
/**
@file ComponentLogical.h
@author t-sakai
@date 2016/06/27 create
*/
#include "Component.h"
#include "ECSTree.h"

namespace lfw
{
    class Entity;

    class ComponentLogical : public Component
    {
    public:
        typedef ECSNodeIterator<ComponentLogical> Iterator;
        typedef ECSNodeConstIterator<ComponentLogical> ConstIterator;

        static ComponentLogical* find(const Char* path);

        static u8 category(){ return ECSCategory_Logical;}
        static u32 type(){ return ECSType_Logical;}

        ComponentLogical();
        explicit ComponentLogical(ID id);
        ~ComponentLogical();

        const Entity& getEntity() const;
        Entity getEntity();

        const NameString& getName() const;
        NameString& getName();

        u16 getNumChildren() const;
        ConstIterator beginChild() const;
        Iterator beginChild();

        ComponentLogical* getParent();
        void setParent(ComponentLogical* parent);

        void setFirstSibling();
        void setLastSibling();

        ECSNode getECSNode() const;
    };
}
#endif //INC_LFRAMEWORK_COMPONENTLOGICAL_H__
