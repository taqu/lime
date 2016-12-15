#ifndef INC_LFRAMEWORK_ECSMANAGER_H__
#define INC_LFRAMEWORK_ECSMANAGER_H__
/**
@file ECSManager.h
@author t-sakai
@date 2016/05/09 create
*/
#include <lcore/Buffer.h>
#include "ECSInitParam.h"
#include "../HandleBasedAllocator.h"
#include "Entity.h"
#include "ECSTree.h"

namespace lfw
{
    class ComponentManager;
    class ComponentID;
    class ComponentRenderer;

    //----------------------------------------------------------
    //---
    //--- ECSManager
    //---
    //----------------------------------------------------------
    class ECSManager
    {
    public:
        inline static ECSInitParam defaultInitParam();

        static inline ECSManager& getInstance(){return *instance_;}
        static bool create(const ECSInitParam& initParam);
        static void destroy();

        inline HandleBasedAllocator& getComponentHandleAllocator();

        Entity requestCreateLogical(const Char* name);
        Entity requestCreateGeometric(const Char* name);

        bool isValidEntity(Entity entity) const;
        void destroyEntity(Entity& entity);
        void requestDestroy(Entity entity);

        inline bool checkFlag(Entity entity, u8 flag) const;
        inline void setFlag(Entity entity, u8 flag);
        inline void resetFlag(Entity entity, u8 flag);

        inline u32 getLayer(Entity entity) const;
        inline void setLayer(Entity entity, u32 layer);

        void addComponentManager(ComponentManager* componentManager);

        inline const ComponentManager* getComponentManager(u8 category) const;
        inline ComponentManager* getComponentManager(u8 category);

        template<class T>
        inline const T* getComponentManager() const;
        template<class T>
        inline T* getComponentManager();

        void addComponent(Entity entity, u8 category, u32 type, Behavior* behavior);

        IDConstAccess getComponents(Entity entity) const;

        const ComponentLogical* getLogical(Entity entity, ID component) const;
        ComponentLogical* getLogical(Entity entity, ID component);

        const ComponentGeometric* getGeometric(Entity entity, ID component) const;
        ComponentGeometric* getGeometric(Entity entity, ID component);

        const Behavior* getComponent(Entity entity, ID component) const;
        Behavior* getComponent(Entity entity, ID component);

        template<class T>
        inline const T* getComponent(Entity entity, ID component) const;
        template<class T>
        inline T* getComponent(Entity entity, ID component);


        inline const Behavior* getComponent(Entity entity, u8 category, u32 type) const;
        Behavior* getComponent(Entity entity, u8 category, u32 type);

        void initialize();
        void update();
        void terminate();
    private:
        ECSManager();
        ~ECSManager();

        ECSManager(const ECSManager&);
        ECSManager& operator=(const ECSManager&);

        static ECSManager* instance_;

        HandleBasedAllocator componentHandleAllocator_;

        HandleTable entityHandles_;

        typedef lcore::Buffer<Entity::Components> EntityComponentsBuffer;
        EntityComponentsBuffer entityComponents_;

        s32 numComponentManagers_;
        ComponentManager** componentManagers_;

        u8* entityFlags_;
        u32* entityLayers_;

        typedef lcore::Array<ComponentRenderer*> ComponentRendererArray;
        ComponentRendererArray componentRenderers_;
    };

    inline ECSInitParam ECSManager::defaultInitParam()
    {
        return ECSInitParam();
    }

    inline HandleBasedAllocator& ECSManager::getComponentHandleAllocator()
    {
        return componentHandleAllocator_;
    }

    inline bool ECSManager::checkFlag(Entity entity, u8 flag) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        return 0 != (entityFlags_[entity.getHandle().index()] & flag);
    }

    inline void ECSManager::setFlag(Entity entity, u8 flag)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        entityFlags_[entity.getHandle().index()] |= flag;
    }

    inline void ECSManager::resetFlag(Entity entity, u8 flag)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        entityFlags_[entity.getHandle().index()] &= ~flag;
    }

    inline u32 ECSManager::getLayer(Entity entity) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        return entityLayers_[entity.getHandle().index()];
    }

    inline void ECSManager::setLayer(Entity entity, u32 layer)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        entityLayers_[entity.getHandle().index()] = layer;
    }

    inline const ComponentManager* ECSManager::getComponentManager(u8 category) const
    {
        LASSERT(category<numComponentManagers_);
        return componentManagers_[category];
    }

    inline ComponentManager* ECSManager::getComponentManager(u8 category)
    {
        LASSERT(category<numComponentManagers_);
        return componentManagers_[category];
    }

    template<class T>
    inline const T* ECSManager::getComponentManager() const
    {
        return static_cast<T*>(getComponentManager(T::Category));
    }

    template<class T>
    inline T* ECSManager::getComponentManager()
    {
        return static_cast<T*>(getComponentManager(T::Category));
    }

    template<class T>
    inline const T* ECSManager::getComponent(Entity entity, ID component) const
    {
        return static_cast<T*>(getComponent(entity, component));
    }
    template<class T>
    inline T* ECSManager::getComponent(Entity entity, ID component)
    {
        return static_cast<T*>(getComponent(entity, component));
    }


    inline const Behavior* ECSManager::getComponent(Entity entity, u8 category, u32 type) const
    {
        return const_cast<ECSManager*>(this)->getComponent(entity, category, type);
    }
}
#endif //INC_LFRAMEWORK_ECSMANAGER_H__
