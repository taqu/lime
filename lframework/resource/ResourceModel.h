#ifndef INC_LFRAMEWORK_RESOURCEMODEL_H__
#define INC_LFRAMEWORK_RESOURCEMODEL_H__
/**
@file ResourceModel.h
@author t-sakai
@date 2016/11/23 create
*/
#include "Resource.h"
#include "render/Model.h"

namespace lfw
{
    class ModelLoader;

    class ResourceModel : public Resource
    {
    public:
        typedef lcore::intrusive_ptr<ResourceModel> pointer;
        static const s32 Type = ResourceType_Model;

        static ResourceModel* load(s32 setID, const Char* path, ModelLoader& modelLoader);

        virtual s32 getType() const
        {
            return ResourceType_Model;
        }

        inline Model::pointer& get();
    protected:
        ResourceModel(const ResourceModel&);
        ResourceModel& operator=(const ResourceModel&);

        ResourceModel(Model* model)
            :model_(model)
        {}

        virtual ~ResourceModel()
        {}

        Model::pointer model_;
    };

    inline Model::pointer& ResourceModel::get()
    {
        return model_;
    }
}
#endif //INC_LFRAMEWORK_RESOURCEMODEL_H__
