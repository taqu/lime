/**
@file ResourceModel.cpp
@author t-sakai
@date 2016/11/22 create
*/
#include "resource/ResourceModel.h"
#include <lcore/io/FileSystem.h>
#include <lcore/io/VirtualFileSystem.h>
#include "resource/ModelLoader.h"

namespace lfw
{
    ResourceModel* ResourceModel::load(s32 setID, const Char* path, ModelLoader& modelLoader)
    {
        LASSERT(NULL != path);
        Model* model = NULL;
        Skeleton* skeleton = NULL;
        modelLoader.loadModel(model, skeleton, setID, path);
        return LNEW ResourceModel(model, skeleton);
    }

    Model::pointer ResourceModel::clone()
    {
        if(NULL == model_){
            return model_;
        }
        Model* model = LNEW Model();
        if(model_->copyTo(*model)){
            return Model::pointer(model);
        }
        LDELETE(model);
        return Model::pointer();
    }

    Skeleton::pointer ResourceModel::cloneSkeleton()
    {
        if(NULL == skeleton_){
            return skeleton_;
        }
        Skeleton* skeleton = LNEW Skeleton();
        if(skeleton_->copyTo(*skeleton)){
            return Skeleton::pointer(skeleton);
        }
        LDELETE(skeleton);
        return Skeleton::pointer();
    }
}
