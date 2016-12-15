#ifndef INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H__
#define INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H__
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

    class ComponentSceneElementManager : public ComponentManager
    {
    public:
        static const u8 Category = ECSCategory_SceneElement;

        struct CameraData
        {
            Camera camera_;
            s32 componentIndex_;
        };

        struct LightData
        {
            Light light_;
            s32 componentIndex_;
        };

        typedef lcore::Array<CameraData> CameraArray;
        typedef lcore::Array<LightData> LightArray;


        struct Data: public ComponentDataCommon<Data>
        {
            s32 index_;
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

        inline const CameraArray& getCameras() const;
        inline CameraArray& getCameras();
        inline const LightArray& getLights() const;
        inline LightArray& getLights();

        void sortCameras();
        void sortLights();

        void postUpdateComponents();
    protected:
        ComponentSceneElementManager(const ComponentSceneElementManager&);
        ComponentSceneElementManager& operator=(const ComponentSceneElementManager&);

        friend bool lessSortData(const CameraData& d0, const CameraData& d1);

        IDTable ids_;
        Behavior** components_;
        Data* data_;
        CameraArray cameras_;
        LightArray lights_;
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
        const Data& d = data_[id.index()];
        return cameras_[d.index_].camera_;
    }

    inline Camera& ComponentSceneElementManager::getCamera(ID id)
    {
        const Data& d = data_[id.index()];
        return cameras_[d.index_].camera_;
    }

    inline const Light& ComponentSceneElementManager::geLight(ID id) const
    {
        const Data& d = data_[id.index()];
        return lights_[d.index_].light_;
    }

    inline Light& ComponentSceneElementManager::geLight(ID id)
    {
        const Data& d = data_[id.index()];
        return lights_[d.index_].light_;
    }

    inline const ComponentSceneElementManager::CameraArray& ComponentSceneElementManager::getCameras() const
    {
        return cameras_;
    }

    inline ComponentSceneElementManager::CameraArray& ComponentSceneElementManager::getCameras()
    {
        return cameras_;
    }

    inline const ComponentSceneElementManager::LightArray& ComponentSceneElementManager::getLights() const
    {
        return lights_;
    }

    inline ComponentSceneElementManager::LightArray& ComponentSceneElementManager::getLights()
    {
        return lights_;
    }

    inline bool lessSortData(const ComponentSceneElementManager::CameraData& d0, const ComponentSceneElementManager::CameraData& d1)
    {
        return d0.camera_.getSortLayer()<d1.camera_.getSortLayer();
    }
}
#endif //INC_LFRAMEWORK_COMPONENTSCENEELEMENTMANAGER_H__
