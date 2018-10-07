#ifndef INC_LFRAMEWORK_RESOURCEMODEL_H_
#define INC_LFRAMEWORK_RESOURCEMODEL_H_
/**
@file ResourceModel.h
@author t-sakai
@date 2016/11/23 create
*/
#include "Resource.h"
#include "../render/Model.h"
#include "../animation/Skeleton.h"

namespace lfw
{
    class ModelLoader;

    class ResourceModel : public Resource
    {
    public:
        typedef lcore::intrusive_ptr<ResourceModel> pointer;
        static const s32 Type = ResourceType_Model;

        static ResourceModel* load(s32 setID, const Char* path, ModelLoader& modelLoader);

        ResourceModel(Model* model, Skeleton* skeleton)
            :model_(model)
            ,skeleton_(skeleton)
        {}

        virtual s32 getType() const
        {
            return ResourceType_Model;
        }

        inline Model::pointer& get();
        Model::pointer clone();
        inline Skeleton::pointer& getSkeleton();
        Skeleton::pointer cloneSkeleton();
    protected:
        ResourceModel(const ResourceModel&);
        ResourceModel& operator=(const ResourceModel&);

        virtual ~ResourceModel()
        {}

        Model::pointer model_;
        Skeleton::pointer skeleton_;
    };

    inline Model::pointer& ResourceModel::get()
    {
        return model_;
    }

    inline Skeleton::pointer& ResourceModel::getSkeleton()
    {
        return skeleton_;
    }
}
#endif //INC_LFRAMEWORK_RESOURCEMODEL_H_
