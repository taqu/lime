#ifndef INC_LFRAMEWORK_ENTITY_H__
#define INC_LFRAMEWORK_ENTITY_H__
/**
@file Entity.h
@author t-sakai
@date 2016/05/09 create
*/
#include "lecs.h"

namespace lfw
{
    class ComponentLogical;
    class ComponentGeometric;
    class Behavior;

    class Entity
    {
    public:
        //-------------------------------------
        class Components
        {
        public:
            Components();
            ~Components();

            inline s16 size() const;
            inline s32 bufferSize() const;
            inline s32 offset() const;
            ID get(s16 index) const;
            const ID* get() const;
            void add(ID id);
            void remove(ID id);
            void removeAt(s16 index);
        private:
            friend class ECSManager;

            void forceAdd(ID id);
            void forceRemove(ID id);
            ID* resize();

            s16 capacity_;
            s16 size_;
            s32 offset_;
        };

        Entity()
        {}

        explicit Entity(Handle handle)
            :handle_(handle)
        {}

        ~Entity()
        {}

        inline void clear();
        inline bool isNull() const;
        inline Handle getHandle() const;
        inline void setHandle(Handle handle);

        bool checkFlag(u8 flag) const;
        void setFlag(u8 flag);
        void resetFlag(u8 flag);

        void addComponent(u8 category, u32 type, Behavior* behavior);
        template<class T>
        inline T* addComponent();

        const NameString& getName() const;
        NameString& getName();

        bool isLogical() const;
        const ComponentLogical* getLogical() const;
        ComponentLogical* getLogical();
        bool isGeometric() const;
        const ComponentGeometric* getGeometric() const;
        ComponentGeometric* getGeometric();

        IDConstAccess getComponents() const;

        inline const Behavior* getComponent(u8 category, u32 type) const;
        Behavior* getComponent(u8 category, u32 type);

        template<class T>
        inline const T* getComponent() const;
        template<class T>
        inline T* getComponent();

        inline const Behavior* getComponent(ID id) const;
        Behavior* getComponent(ID id);

        template<class T>
        inline const T* getComponent(ID id) const;
        template<class T>
        inline T* getComponent(ID id);

        void removeComponent(ID id);

        inline bool operator==(const Entity& entity) const;
        inline bool operator!=(const Entity& entity) const;
    private:
        Handle handle_;
    };

    inline void Entity::clear()
    {
        handle_.clear();
    }

    inline bool Entity::isNull() const
    {
        return handle_.isNull();
    }

    inline Handle Entity::getHandle() const
    {
        return handle_;
    }

    inline void Entity::setHandle(Handle handle)
    {
        handle_ = handle;
    }

    template<class T>
    inline T* Entity::addComponent()
    {
        T* component = LNEW T;
        addComponent(T::category(), T::type(), component);
        return component;
    }

    inline const Behavior* Entity::getComponent(u8 category, u32 type) const
    {
        return const_cast<Entity*>(this)->getComponent(category, type);
    }

    template<class T>
    inline const T* Entity::getComponent() const
    {
        return static_cast<T*>(getComponent(T::category(), T::type()));
    }

    template<class T>
    inline T* Entity::getComponent()
    {
        return static_cast<T*>(getComponent(T::category(), T::type()));
    }

    inline const Behavior* Entity::getComponent(ID id) const
    {
        return const_cast<Entity*>(this)->getComponent(id);
    }

    template<class T>
    inline const T* Entity::getComponent(ID id) const
    {
        return static_cast<T*>(getComponent(id));
    }

    template<class T>
    inline T* Entity::getComponent(ID id)
    {
        return static_cast<T*>(getComponent(id));
    }

    inline bool Entity::operator==(const Entity& entity) const
    {
        return handle_ == entity.handle_;
    }

    inline bool Entity::operator!=(const Entity& entity) const
    {
        return handle_ != entity.handle_;
    }

    //----------------------------------------------------------
    //---
    //--- Entity::Components
    //---
    //----------------------------------------------------------
    inline s16 Entity::Components::size() const
    {
        return size_;
    }

    inline s32 Entity::Components::bufferSize() const
    {
        return capacity_<<2;
    }

    inline s32 Entity::Components::offset() const
    {
        return offset_;
    }
}
#endif //INC_LFRAMEWORK_ENTITY_H__
