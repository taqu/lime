/**
@file ECSManager.cpp
@author t-sakai
@date 2016/05/15 create
*/
#include "ecs/ECSManager.h"
#include "ecs/ComponentManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentRendererManager.h"
#include "ecs/ComponentRenderer2DManager.h"
#include "ecs/ComponentSceneElementManager.h"
#include "ecs/ComponentBehaviorManager.h"

#include "ecs/ComponentBehavior.h"
#include "ecs/ComponentRenderer.h"
#include "ecs/ComponentRenderer2D.h"
#include "ecs/ComponentCamera.h"
#include "ecs/ComponentLight.h"

namespace lfw
{
    ECSManager* ECSManager::instance_ = NULL;

    ECSManager::ECSManager()
        :numComponentManagers_(0)
        ,componentManagers_(NULL)
        ,entityFlags_(NULL)
        ,entityLayers_(NULL)
    {
        componentOperations_.reserve(64);
    }

    ECSManager::~ECSManager()
    {
        LDELETE_ARRAY(entityLayers_);
        LDELETE_ARRAY(entityFlags_);
        for(s32 i=0; i<numComponentManagers_; ++i){
            LDELETE(componentManagers_[i]);
        }
        LDELETE_ARRAY(componentManagers_);
    }

    bool ECSManager::create(const ECSInitParam& initParam)
    {
        destroy();
        instance_ = LNEW ECSManager;

        instance_->numComponentManagers_ = initParam.maxTypes_;
        instance_->componentManagers_ = LNEW ComponentManager*[instance_->numComponentManagers_];
        for(s32 i=0; i<instance_->numComponentManagers_; ++i){
            instance_->componentManagers_[i] = NULL;
        }

        if(0 != (initParam.flags_ & Flag_CreateRequiredComponentManagers)){
            instance_->addComponentManager(LNEW ComponentLogicalManager());
            instance_->addComponentManager(LNEW ComponentGeometricManager());
            instance_->addComponentManager(LNEW ComponentRendererManager());
            instance_->addComponentManager(LNEW ComponentRenderer2DManager());
            instance_->addComponentManager(LNEW ComponentSceneElementManager());
            instance_->addComponentManager(LNEW ComponentBehaviorManager());
        }
        instance_->initialize();
        return true;
    }

    void ECSManager::destroy()
    {
        if(NULL != instance_){
            instance_->terminate();
            LDELETE(instance_);
        }
    }

    Entity ECSManager::requestCreateLogical(const Char* name)
    {
        LASSERT(NULL != componentManagers_[ECSCategory_Logical]);
        LASSERT(ECSCategory_Logical == componentManagers_[ECSCategory_Logical]->category());

        Handle handle = entityHandles_.create();
        if(entityComponents_.size()<entityHandles_.capacity()){
            expand(entityFlags_, entityComponents_.size(), entityHandles_.capacity());
            expand(entityLayers_, entityComponents_.size(), entityHandles_.capacity());
            entityComponents_.resize(entityHandles_.capacity());
        }
        entityComponents_.construct(handle.index());
        entityFlags_[handle.index()] = EntityFlag_Default;
        entityLayers_[handle.index()] = Layer_Default;

        Entity entity = Entity(handle);

        ComponentLogicalManager* componentLogicalManager = getComponentManager<ComponentLogicalManager>();
        ID id = componentLogicalManager->create(entity);
        Entity::Components& components = entityComponents_[entity.getHandle().index()];
        components.forceAdd(id);
        componentLogicalManager->add(id, Operation::Type_Create);
        if(NULL != name){
            componentLogicalManager->getTreeComponent(id)->getName().assign(name);
        }
        return entity;
    }

    Entity ECSManager::requestCreateGeometric(const Char* name)
    {
        LASSERT(NULL != componentManagers_[ECSCategory_Geometric]);
        LASSERT(ECSCategory_Geometric == componentManagers_[ECSCategory_Geometric]->category());

        Handle handle = entityHandles_.create();
        if(entityComponents_.size()<entityHandles_.capacity()){
            expand(entityFlags_, entityComponents_.size(), entityHandles_.capacity());
            expand(entityLayers_, entityComponents_.size(), entityHandles_.capacity());
            entityComponents_.resize(entityHandles_.capacity());
        }
        entityComponents_.construct(handle.index());
        entityFlags_[handle.index()] = EntityFlag_Default;
        entityLayers_[handle.index()] = Layer_Default;

        Entity entity = Entity(handle);

        ComponentGeometricManager* componentGeometricManager = getComponentManager<ComponentGeometricManager>();
        ID id = componentGeometricManager->create(entity);
        Entity::Components& components = entityComponents_[entity.getHandle().index()];
        components.forceAdd(id);
        componentGeometricManager->add(id, Operation::Type_Create);
        if(NULL != name){
            componentGeometricManager->getTreeComponent(id)->getName().assign(name);
        }
        return entity;
    }

    bool ECSManager::isValidEntity(Entity entity) const
    {
        return entityHandles_.isValid(entity.getHandle());
    }

    void ECSManager::destroyEntity(Entity& entity)
    {
        Handle handle = entity.getHandle();
        entityHandles_.destroy(handle);

        Entity::Components& components = entityComponents_[handle.index()];
        const ID* ids = components.get();
        for(s32 i=0; i<components.size(); ++i){
            u8 category = ids[i].category();
            LASSERT(NULL != componentManagers_[category]);
            componentManagers_[category]->destroy(ids[i]);
        }

        entityComponents_.destruct(handle.index());
        entityFlags_[handle.index()] = 0;
        entityLayers_[handle.index()] = Layer_Default;
        entity.setHandle(Handle::construct(Handle::Invalid));
    }

    void ECSManager::requestDestroy(Entity entity)
    {
        Handle handle = entity.getHandle();
        Entity::Components& components = entityComponents_[handle.index()];
        const ID* ids = components.get();
        for(s32 i=0; i<components.size(); ++i){
            switch(ids[i].category())
            {
            case ECSCategory_Logical:
            {
                ComponentLogicalManager* manager = getComponentManager<ComponentLogicalManager>();
                manager->add(ids[i], Operation::Type_Destroy);
            }
                break;
            case ECSCategory_Geometric:
            {
                ComponentGeometricManager* manager = getComponentManager<ComponentGeometricManager>();
                manager->add(ids[i], Operation::Type_Destroy);
            }
                break;
            }
        }
    }

    void ECSManager::addComponentManager(ComponentManager* componentManager)
    {
        u8 category = componentManager->category();
        LASSERT(category<numComponentManagers_);
        LDELETE(componentManagers_[category]);
        componentManagers_[category] = componentManager;
    }

    void ECSManager::addComponent(Entity entity, u8 category, u32 type, Behavior* behavior)
    {
        LASSERT(NULL != behavior);

        ID id;
        switch(category)
        {
        case ECSCategory::ECSCategory_Renderer:
        {
            ComponentRendererManager* componentRendererManager = getComponentManager<ComponentRendererManager>();
            id = componentRendererManager->create(entity, type, static_cast<ComponentRenderer*>(behavior));
        }
            break;

        case ECSCategory::ECSCategory_Renderer2D:
        {
            ComponentRenderer2DManager* componentRenderer2DManager = getComponentManager<ComponentRenderer2DManager>();
            id = componentRenderer2DManager->create(entity, type, static_cast<ComponentRenderer2D*>(behavior));
        }
            break;

        case ECSCategory::ECSCategory_Behavior:
        {
            ComponentBehaviorManager* componentBehaviorManager = getComponentManager<ComponentBehaviorManager>();
            id = componentBehaviorManager->create(entity, type, static_cast<ComponentBehavior*>(behavior));
        }
            break;

        case ECSCategory::ECSCategory_SceneElement:
        {
            ComponentSceneElementManager* componentSceneElementManager = getComponentManager<ComponentSceneElementManager>();
            id = componentSceneElementManager->create(entity, type, behavior);
        }
            break;
        default:
            return;
        }

        LASSERT(category == behavior->getID().category());

#ifdef _DEBUG
        IDConstAccess access = getComponents(entity);
        for(s32 i=1; i<access.size(); ++i){
            LASSERT(access[i].valid());
            LASSERT(id != getComponent(entity, access[i])->getID());
        }
#endif
        //Entity::Components& components = entityComponents_[entity.getHandle().index()];
        //components.add(id);
        ComponentOperation componentOp = {ComponentOperation::Type_Add, entity.getHandle(), id};
        componentOperations_.push_back(componentOp);
    }

    IDConstAccess ECSManager::getComponents(Entity entity) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));

        const Entity::Components& components = entityComponents_[entity.getHandle().index()];
        return {components.size(), components.get()};
    }

    const ComponentLogical* ECSManager::getLogical(Entity entity, ID component) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Logical == component.category());

        return static_cast<ComponentLogicalManager*>(componentManagers_[component.category()])->getTreeComponent(component);
    }

    ComponentLogical* ECSManager::getLogical(Entity entity, ID component)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Logical == component.category());

        return static_cast<ComponentLogicalManager*>(componentManagers_[component.category()])->getTreeComponent(component);
    }

    const ComponentGeometric* ECSManager::getGeometric(Entity entity, ID component) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Geometric == component.category());

        return static_cast<ComponentGeometricManager*>(componentManagers_[component.category()])->getTreeComponent(component);
    }

    ComponentGeometric* ECSManager::getGeometric(Entity entity, ID component)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Geometric == component.category());

        return static_cast<ComponentGeometricManager*>(componentManagers_[component.category()])->getTreeComponent(component);
    }

    const Behavior* ECSManager::getComponent(Entity entity, ID component) const
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Logical != component.category() && ECSCategory_Geometric != component.category());

        return componentManagers_[component.category()]->getBehavior(component);
    }

    Behavior* ECSManager::getComponent(Entity entity, ID component)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Logical != component.category() && ECSCategory_Geometric != component.category());

        return componentManagers_[component.category()]->getBehavior(component);
    }

    Behavior* ECSManager::getComponent(Entity entity, u8 category, u32 type)
    {
        LASSERT(entityHandles_.isValid(entity.getHandle()));
        LASSERT(ECSCategory_Logical != category && ECSCategory_Geometric != category);
        LASSERT(0<=category && category<numComponentManagers_);

        Entity::Components& components = entityComponents_[entity.getHandle().index()];
        ComponentManager* manager = componentManagers_[category];
        const ID* ids = components.get();
        for(s32 i=0; i<components.size(); ++i){
            Behavior* behavior = manager->getBehavior(ids[i]);
            u32 t;
            switch(category)
            {
            case ECSCategory_Renderer:
                t = static_cast<ComponentRenderer*>(behavior)->getType();
                break;
            case ECSCategory_SceneElement:
                switch(type)
                {
                case ECSType_Camera:
                    t = static_cast<ComponentCamera*>(behavior)->getType();
                    break;
                case ECSType_Light:
                    t = static_cast<ComponentLight*>(behavior)->getType();
                    break;
                default:
                    LASSERT(false);
                    t = ECSType_None;
                    break;
                }
                break;
            case ECSCategory_Behavior:
                t = static_cast<ComponentBehavior*>(behavior)->getType();
                break;
            default:
                LASSERT(false);
                t = ECSType_None;
                break;
            }
            if(type == t){
                return behavior;
            }
        }
        return NULL;
    }

    void ECSManager::removeComponent(Entity entity, ID component)
    {
        LASSERT(NULL != componentManagers_[component.category()]);
        //componentManagers_[component.category()]->destroy(component);
        //Entity::Components& components = entityComponents_[entity.getHandle().index()];
        //components.remove(component);

        ComponentOperation componentOp = {ComponentOperation::Type_Destroy, entity.getHandle(), component};
        componentOperations_.push_back(componentOp);
    }

    void ECSManager::initialize()
    {
        for(s32 i=0; i<numComponentManagers_; ++i){
            if(NULL != componentManagers_[i]){
                componentManagers_[i]->initialize();
            }
        }
    }

    void ECSManager::update()
    {
        LASSERT(NULL != componentManagers_[ECSType_Logical]);
        LASSERT(NULL != componentManagers_[ECSType_Geometric]);

        //
        getComponentManager<ComponentRendererManager>()->preUpdateComponents();
        getComponentManager<ComponentRenderer2DManager>()->preUpdateComponents();

        ComponentLogicalManager* logicalManager = getComponentManager<ComponentLogicalManager>();
        ComponentGeometricManager* geometricManager = getComponentManager<ComponentGeometricManager>();

        const Entity* entityEnd;

        //Logical更新
        entityEnd = logicalManager->end();
        for(const Entity* entity = logicalManager->begin(); entity != entityEnd; ++entity){
            const Entity::Components& components = entityComponents_[entity->getHandle().index()];
            LASSERT(componentHandleAllocator_.get<ID>(components.bufferSize(), components.offset())[0].category() == ECSCategory_Logical);

            ID* ids = componentHandleAllocator_.get<ID>(components.bufferSize(), components.offset());
            for(s32 i=1; i<components.size(); ++i){
                ID componentID = ids[i];
                componentManagers_[componentID.category()]->updateComponent(componentID);
            }
        }

        //Geometric更新
        entityEnd = geometricManager->end();
        for(const Entity* entity = geometricManager->begin(); entity != entityEnd; ++entity){
            const Entity::Components& components = entityComponents_[entity->getHandle().index()];
            LASSERT(componentHandleAllocator_.get<ID>(components.bufferSize(), components.offset())[0].category() == ECSCategory_Geometric);

            ID* ids = componentHandleAllocator_.get<ID>(components.bufferSize(), components.offset());
            for(s32 i=0; i<components.size(); ++i){
                ID componentID = ids[i];
#ifdef _DEBUG
                u8 category = componentID.category();
                componentManagers_[category]->updateComponent(componentID);
#else
                componentManagers_[componentID.category()]->updateComponent(componentID);
#endif
            }
        }
    }

    void ECSManager::postUpdate()
    {
        ComponentLogicalManager* logicalManager = getComponentManager<ComponentLogicalManager>();
        ComponentGeometricManager* geometricManager = getComponentManager<ComponentGeometricManager>();

        if(0<componentOperations_.size()){
            for(s32 i=0; i<componentOperations_.size(); ++i){
                switch(componentOperations_[i].type_)
                {
                case ComponentOperation::Type_Add:
                {
                    Entity::Components& components = entityComponents_[componentOperations_[i].entityHandle_.index()];
                    components.add(componentOperations_[i].component_);
                }
                    break;
                case ComponentOperation::Type_Destroy:
                {
                    ID component = componentOperations_[i].component_;
                    componentManagers_[component.category()]->destroy(component);
                    Entity::Components& components = entityComponents_[componentOperations_[i].entityHandle_.index()];
                    components.remove(component);
                }
                    break;
                }
            }
            componentOperations_.clear();
        }

        logicalManager->update();
        geometricManager->update();

        geometricManager->updateMatrices();

        getComponentManager<ComponentSceneElementManager>()->postUpdateComponents();
        getComponentManager<ComponentRendererManager>()->postUpdateComponents();
        getComponentManager<ComponentRenderer2DManager>()->postUpdateComponents();
    }

    void ECSManager::terminate()
    {
        if(NULL != componentManagers_[ECSType_Logical]){
            ComponentLogicalManager* logicalManager = getComponentManager<ComponentLogicalManager>();
            logicalManager->destroyAllEntries();
        }

        if(NULL != componentManagers_[ECSType_Geometric]){
            ComponentGeometricManager* geometricManager = getComponentManager<ComponentGeometricManager>();
            geometricManager->destroyAllEntries();
        }

        for(s32 i=0; i<numComponentManagers_; ++i){
            if(NULL != componentManagers_[i]){
                componentManagers_[i]->terminate();
            }
        }
        for(s32 i=0; i<numComponentManagers_; ++i){
            LDELETE(componentManagers_[i]);
        }
    }
}
