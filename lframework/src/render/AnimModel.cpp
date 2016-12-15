/**
@file AnimModel.cpp
@author t-sakai
@date 2016/11/26 create
*/
#include "render/AnimModel.h"

namespace lfw
{
    AnimModel::AnimModel()
    {
    }

    AnimModel::~AnimModel()
    {
    }

    //-----------------------------------------------------
    // バッファ作成
    bool AnimModel::create(
        u32 numGeometries,
        u32 numMeshes,
        u32 numMaterials,
        u32 numNodes,
        u32 numTextures,
        s32 numJoints)
    {
        if(!Model::create(
            numGeometries,
            numMeshes,
            numMaterials,
            numNodes,
            numTextures))
        {
            return false;
        }
        Skeleton(numJoints).swap(skeleton_);
        return true;
    }

    //-----------------------------------------------------
    void AnimModel::destroy()
    {
        Model::destroy();
        Skeleton().swap(skeleton_);
    }

    //-----------------------------------------------------
    bool AnimModel::copyTo(AnimModel& dst)
    {
        if(!Model::copyTo(dst)){
            return false;
        }
        skeleton_.copyTo(dst.skeleton_);
        return true;
    }
}
