#ifndef INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H_
/**
@file ComponentSceneElementManager.h
@author t-sakai
@date 2016/11/29 create
*/
#include "ComponentManager.h"
#include <lcore/Array.h>
#include "../render/Camera.h"
#include "../render/Light.h"

namespace lfw
{
    class Camera;
    class ComponentCamera;
    class ComponentLight;

    class CameraArray;
    class LightArray;

    //-----------------------------------------------------------
    //---
    //--- ComponentSceneElementManager
    //---
    //-----------------------------------------------------------
    class ComponentSceneElementManager : public ComponentManager
    {
    public:
        typedef lcore::Array<s32> IndexArray;

        static const u8 Category = ECSCategory_SceneElement;

        struct Data : public ComponentDataCommon<Data>
        {
            ~Data() = delete;

            union
            {
                Camera camera_;
                Light light_;
            };
        };

        ComponentSceneElementManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);

        virtual ~ComponentSceneElementManager();

        virtual u8 category() const;

        ID create(Entity entity, u32 type, Behavior* component);
        virtual void destroy(ID id);

        virtual Behavior* getBehavior(ID id);
        virtual void initialize();
        virtual void terminate();

        virtual void updateComponent(ID id);

        inline const Data& getData(ID id) const;
        inline Data& getData(ID id);
        inline const Camera& getCamera(ID id) const;
        inline Camera& getCamera(ID id);
        inline const Light& geLight(ID id) const;
        inline Light& geLight(ID id);

        CameraArray getCameras();
        LightArray getLights();

        void sortCameras();
        void sortLights();

        void postUpdateComponents();

        template<class T>
        inline T* getComponent(s32 index);
    protected:
        ComponentSceneElementManager(const ComponentSceneElementManager&) = delete;
        ComponentSceneElementManager& operator=(const ComponentSceneElementManager&) = delete;

        void destroyData(s32 index);
        void expandData(s32 newCapacity, s32 oldCapacity);

        struct LessSortData
        {
            LessSortData(const Data* data)
                :data_(data)
            {
            }

            bool operator()(s32 i0, s32 i1) const
            {
                return data_[i0].camera_.getSortLayer()<data_[i1].camera_.getSortLayer();
            }
            const Data* data_;
        };

        IDTable ids_;
        Behavior** components_;
        Data* data_;
        IndexArray cameras_;
        IndexArray lights_;
    };

    inline const ComponentSceneElementManager::Data& ComponentSceneElementManager::getData(ID id) const
    {
        return data_[id.index()];
    }

    inline ComponentSceneElementManager::Data& ComponentSceneElementManager::getData(ID id)
    {
        return data_[id.index()];
    }

    inline const Camera& ComponentSceneElementManager::getCamera(ID id) const
    {
        return data_[id.index()].camera_;
    }

    inline Camera& ComponentSceneElementManager::getCamera(ID id)
    {
        return data_[id.index()].camera_;
    }

    inline const Light& ComponentSceneElementManager::geLight(ID id) const
    {
        return data_[id.index()].light_;
    }

    inline Light& ComponentSceneElementManager::geLight(ID id)
    {
        return data_[id.index()].light_;
    }

    template<class T>
    inline T* ComponentSceneElementManager::getComponent(s32 index)
    {
        LASSERT(0<=index && index<ids_.capacity());
        LASSERT(NULL != components_[index]);
        return reinterpret_cast<T*>(components_[index]);
    }

    //-----------------------------------------------------------
    //---
    //--- CameraArray
    //---
    //-----------------------------------------------------------
    class CameraArray
    {
    public:
        CameraArray()
            :size_(0)
            ,indices_(NULL)
            ,data_(NULL)
        {}

        s32 size() const{return size_;}
        const Camera& operator[](s32 index)const {return data_[indices_[index]].camera_;}
        Camera& operator[](s32 index){return data_[indices_[index]].camera_;}
        s32 componentIndex(s32 index)const {return indices_[index];}
    private:
        friend class ComponentSceneElementManager;

        CameraArray(s32 size, s32* indices, ComponentSceneElementManager::Data* data)
            :size_(size)
            ,indices_(indices)
            ,data_(data)
        {}

        s32 size_;
        s32* indices_;
        ComponentSceneElementManager::Data* data_;
    };

    //-----------------------------------------------------------
    //---
    //--- LightArray
    //---
    //-----------------------------------------------------------
    class LightArray
    {
    public:
        LightArray()
            :size_(0)
            ,indices_(NULL)
            ,data_(NULL)
        {}

        s32 size() const{return size_;}
        const Light& operator[](s32 index) const{return data_[indices_[index]].light_;}
        Light& operator[](s32 index){return data_[indices_[index]].light_;}
        s32 componentIndex(s32 index)const {return indices_[index];}
    private:
        friend class ComponentSceneElementManager;
        LightArray(s32 size, s32* indices, ComponentSceneElementManager::Data* data)
            :size_(size)
            ,indices_(indices)
            ,data_(data)
        {}

        s32 size_;
        s32* indices_;
        ComponentSceneElementManager::Data* data_;
    };
}
#endif //INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H_
