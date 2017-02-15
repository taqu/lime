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

        data_ = LNEW Data[capacity];
    }

    ComponentSceneElementManager::~ComponentSceneElementManager()
    {
        LDELETE_ARRAY(data_);
        for(s32 i=0; i<ids_.capacity(); ++i){
            LDELETE(components_[i]);
        }
        LDELETE_ARRAY(components_);
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
            expand(data_, capacity, ids_.capacity());
        }
        s32 index = id.index();
        LDELETE(components_[index]);
        components_[index] = component;

        data_[index].entity_ = entity;
        data_[index].clear(type);

        switch(type)
        {
        case ECSType_Camera:
        {
            data_[index].index_ = cameras_.size();
            cameras_.resize(cameras_.size()+1);
            CameraData& c = cameras_[cameras_.size()-1];
            c.componentIndex_ = index;
            c.camera_.setSortLayer(0);
            c.camera_.setLayerMask(Layer_Default);
        }
            break;
        case ECSType_Light:
        {
            data_[index].index_ = lights_.size();
            lights_.resize(lights_.size()+1);
            LightData& l = lights_[lights_.size()-1];
            l.componentIndex_ = index;
            l.light_.setSortLayer(0);
            l.light_.setLayerMask(Layer_Default);
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

    void ComponentSceneElementManager::sortCameras()
    {
        s32 count = 0;
        for(s32 i=0; i<ids_.capacity(); ++i){
            if(NULL == components_[i]){
                continue;
            }
            if(ECSType_Camera != data_[i].type()){
                continue;
            }
            cameras_[count].componentIndex_ = i;
            ++count;
        }
        LASSERT(count == cameras_.size());
        lcore::introsort(count, cameras_.begin(), lessSortData);
        for(s32 i=0; i<count; ++i){
            data_[cameras_[i].componentIndex_].index_ = i;
        }
    }

    void ComponentSceneElementManager::sortLights()
    {

    }

    void ComponentSceneElementManager::postUpdateComponents()
    {
        for(s32 i=0; i<cameras_.size(); ++i){
            Camera& camera = cameras_[i].camera_;
            const Behavior* behavior = components_[cameras_[i].componentIndex_];
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
            LightData& light = lights_[i];
            const Behavior* behavior = components_[light.componentIndex_];
            const ComponentGeometric* geometric = getData(behavior->getID()).entity_.getGeometric();
            light.light_.setPosition(geometric->getPosition());
            geometric->getRotation().getDireciton(light.light_.getDirection());
        }
    }
}
