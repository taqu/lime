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
        Model* model = modelLoader.loadModel(setID, path);
        return LNEW ResourceModel(model);
    }
}
