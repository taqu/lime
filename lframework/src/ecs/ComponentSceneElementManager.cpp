/**
@file ComponentSceneElementManager.cpp
@author t-sakai
@date 2016/11/29 create
*/
#include "ecs/ComponentSceneElementManager.h"
#include <lcore/Sort.h>
#include <lmath/Quaternion.h>
#include "ecs/ECSManager.h"
#include "ecs/Component.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    ComponentSceneElementManager::ComponentSceneElementManager(s32 capacity, s32 expandSize)
        :ids_(capacity, expandSize)
        ,components_(NULL)
        ,data_(NULL)
    {
        LASSERT(0<=capacity);
        components_ = LNEW Behavior*[capacity];
        lcore::memset(components_, 0, sizeof(Behavior*)*capacity);

        data_ = (Data*)LMALLOC(sizeof(Data)*capacity);
        for(s32 i=0; i<capacity; ++i){
            data_[i].setType(0);
        }
    }

    ComponentSceneElementManager::~ComponentSceneElementManager()
    {
        for(s32 i=0; i<ids_.capacity(); ++i){
            LDELETE(components_[i]);
            destroyData(i);
        }
        LFREE(data_);
        LDELETE_ARRAY(components_);
        cameras_.clear();
        lights_.clear();
        ids_.clear();
    }

    u8 ComponentSceneElementManager::category() const
    {
        return Category;
    }

    ID ComponentSceneElementManager::create(Entity entity, u32 type, Behavior* component)
    {
        LASSERT(NULL != component);
        s32 capacity = ids_.capacity();
        ID id = ids_.create(Category);
        component->setID(id);
        if(capacity<ids_.capacity()){
            expand(components_, capacity, ids_.capacity());
            lcore::memset(components_+capacity, 0, sizeof(ComponentRenderer*)*(ids_.capacity()-capacity));
            expandData(ids_.capacity(), capacity);
        }
        s32 index = id.index();
        LDELETE(components_[index]);
        components_[index] = component;

        LASSERT(data_[index].type()<=0);

        data_[index].entity_ = entity;
        data_[index].clear(type);

        switch(type)
        {
        case ECSType_Camera:
        {
            LPLACEMENT_NEW(&data_[index].camera_) Camera();

            cameras_.push_back(index);
        }
            break;
        case ECSType_Light:
        {
            LPLACEMENT_NEW(&data_[index].light_) Light();

            lights_.push_back(index);
        }
            break;
        default:
            LASSERT(false);
            break;
        }
        if(NULL != component){
            component->onCreate();
        }

        return id;
    }

    void ComponentSceneElementManager::destroy(ID id)
    {
        LASSERT(id.valid());
        u16 index = id.index();
        if(NULL != components_[index]){
            components_[index]->onDestroy();
            LDELETE(components_[index]);
        }
        destroyData(index);
        ids_.destroy(ID::construct(0, index));
    }

    Behavior* ComponentSceneElementManager::getBehavior(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        return components_[id.index()];
    }

    void ComponentSceneElementManager::initialize()
    {
    }

    void ComponentSceneElementManager::terminate()
    {
    }

    void ComponentSceneElementManager::updateComponent(ID id)
    {
        LASSERT(0<=id.index() && id.index()<ids_.capacity());
        LASSERT(id.valid());
        LASSERT(NULL != components_[id.index()]);

        s32 index = id.index();
        u32 type = data_[id.index()].type();
        Data& data = data_[id.index()];
        switch(type)
        {
        case ECSType_Camera:
        {
            if(!data.checkFlag(CommonFlag_Start)){
                components_[index]->onStart();
                data.setFlag(CommonFlag_Start);
            }
        }
        break;
        case ECSType_Light:
        {
            if(!data.checkFlag(CommonFlag_Start)){
                components_[index]->onStart();
                data.setFlag(CommonFlag_Start);
            }
        }
        break;
        default:
            LASSERT(false);
            break;
        }
    }

    CameraArray ComponentSceneElementManager::getCameras()
    {
        return {cameras_.size(), &cameras_[0], data_};
    }

    LightArray ComponentSceneElementManager::getLights()
    {
        return {lights_.size(), &lights_[0], data_};
    }

    void ComponentSceneElementManager::sortCameras()
    {
        lcore::introsort(cameras_.size(), cameras_.begin(), LessSortData(data_));
    }

    void ComponentSceneElementManager::sortLights()
    {

    }

    void ComponentSceneElementManager::postUpdateComponents()
    {
        for(s32 i=0; i<cameras_.size(); ++i){
            s32 index = cameras_[i];
            Camera& camera = data_[index].camera_;
            const Behavior* behavior = components_[index];
            Entity entity = getData(behavior->getID()).entity_;
            const ComponentGeometric* geometric = entity.getGeometric();
            lmath::Matrix44& view = camera.getViewMatrix();
            lmath::Matrix44& invview = camera.getInvViewMatrix();
            lookAt(view, invview, geometric->getPosition(), geometric->getRotation());
            lmath::Vector4& eyePosition = camera.getEyePosition();
            eyePosition = geometric->getPosition();
            camera.updateMatrix();
        }

        for(s32 i=0; i<lights_.size(); ++i){
            s32 index = lights_[i];
            Light& light = data_[index].light_;
            const Behavior* behavior = components_[index];
            const ComponentGeometric* geometric = getData(behavior->getID()).entity_.getGeometric();
            light.setPosition(geometric->getPosition());
            geometric->getRotation().getDireciton(light.getDirection());
        }
    }

    void ComponentSceneElementManager::destroyData(s32 index)
    {
        switch(data_[index].type()){
        case ECSType_Camera:
            data_[index].camera_.~Camera();
            break;
        case ECSType_Light:
            data_[index].light_.~Light();
            break;
        }
        data_[index].setType(0);
    }

    void ComponentSceneElementManager::expandData(s32 newCapacity, s32 oldCapacity)
    {
        Data* data = (Data*)LMALLOC(sizeof(Data)*newCapacity);
        for(s32 i=0; i<oldCapacity; ++i){
            data[i].entity_ = data_[i].entity_;
            data[i].typeFlags_ = data_[i].typeFlags_;
            switch(data_[i].type()){
            case ECSType_Camera:
                data[i].camera_ = lcore::move(data_[i].camera_);
                data_[i].camera_.~Camera();
                break;
            case ECSType_Light:
                data[i].light_ = lcore::move(data_[i].light_);
                data_[i].light_.~Light();
                break;
            }
        }
        LFREE(data_);
        data_ = data;
    }
}
