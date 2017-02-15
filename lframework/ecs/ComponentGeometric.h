#ifndef INC_LFRAMEWORK_COMPONENTGEOMETRIC_H__
#define INC_LFRAMEWORK_COMPONENTGEOMETRIC_H__
/**
@file ComponentGeometric.h
@author t-sakai
@date 2016/06/27 create
*/
#include "Component.h"
#include "ECSTree.h"

namespace lmath
{
    class Vector3;
    class Vector4;
    class Quaternion;
    class Matrix44;
}

namespace lfw
{
    class Entity;

    class ComponentGeometric : public Component
    {
    public:
        typedef ECSNodeIterator<ComponentGeometric> Iterator;
        typedef ECSNodeConstIterator<ComponentGeometric> ConstIterator;

        static ComponentGeometric* find(const Char* path);

        static u8 category(){ return ECSCategory_Geometric;}
        static u32 type(){ return ECSType_Geometric;}

        ComponentGeometric();
        explicit ComponentGeometric(ID id);
        ~ComponentGeometric();

        const Entity& getEntity() const;
        Entity getEntity();

        const NameString& getName() const;
        NameString& getName();

        u16 getNumChildren() const;
        ConstIterator beginChild() const;
        Iterator beginChild();

        ComponentGeometric* getParent();
        void setParent(ComponentGeometric* parent);

        void setFirstSibling();
        void setLastSibling();

        ECSNode getECSNode() const;

        //---------------------------------------------------
        const lmath::Vector4& getPosition() const;
        lmath::Vector4& getPosition();
        void setPosition(const lmath::Vector4& position);

        const lmath::Quaternion& getRotation() const;
        lmath::Quaternion& getRotation();
        void setRotation(const lmath::Quaternion& rotation);

        const lmath::Vector3& getScale() const;
        lmath::Vector3& getScale();
        void setScale(const lmath::Vector3& scale);

        const lmath::Matrix44& getMatrix() const;
        lmath::Matrix44& getMatrix();
        void updateMatrix();
    };
}
#endif //INC_LFRAMEWORK_COMPONENTGEOMETRIC_H__
