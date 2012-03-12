#ifndef INC_LSCENE_ANIMOBJECT_H__
#define INC_LSCENE_ANIMOBJECT_H__
/**
@file AnimObject.h
@author t-sakai
@date 2010/04/02 create
*/
#include <lframework/scene/lscene.h>
#include "../render/Drawable.h"
#include <lcore/Buffer.h>

#include <lframework/anim/Skeleton.h>
#include <lframework/anim/IKPack.h>

namespace lrender
{
    class Batch;
}

namespace lanim
{
    class SkeletonPose;
}

namespace lscene
{
    class Geometry;
    class Material;

    class AnimObject : public lrender::Drawable
    {
    public:
        AnimObject();

        AnimObject(
            u32 numGeometries,
            u32 numMaterials);

        ~AnimObject();

        void swap(AnimObject& rhs);

        inline void setSkeleton(lanim::Skeleton::pointer& skeleton);
        inline lanim::Skeleton::pointer& getSkeleton();
        inline const lanim::Skeleton::pointer& getSkeleton() const;

        inline void setIKPack(lanim::IKPack::pointer& ikPack);
        inline lanim::IKPack::pointer& getIKPack();

        inline void setSkeletonPose(const lanim::SkeletonPose* pose);
        inline const lanim::SkeletonPose* getSkeletonPose() const;

        virtual u32 getNumJointPoses() const;
        virtual const lmath::Matrix34* getGlobalJointPoses() const;
    private:
        AnimObject(const AnimObject&);
        AnimObject& operator=(const AnimObject&);

        lanim::Skeleton::pointer skeleton_;
        const lanim::SkeletonPose* skeletonPose_;

        lanim::IKPack::pointer ikPack_;
    };

    inline void AnimObject::setSkeleton(lanim::Skeleton::pointer& skeleton)
    {
        skeleton_ = skeleton;
    }

    inline lanim::Skeleton::pointer& AnimObject::getSkeleton()
    {
        return skeleton_;
    }

    inline const lanim::Skeleton::pointer& AnimObject::getSkeleton() const
    {
        return skeleton_;
    }

    inline void AnimObject::setIKPack(lanim::IKPack::pointer& ikPack)
    {
        ikPack_ = ikPack;
    }

    inline lanim::IKPack::pointer& AnimObject::getIKPack()
    {
        return ikPack_;
    }

    inline void AnimObject::setSkeletonPose(const lanim::SkeletonPose* pose)
    {
        skeletonPose_ = pose;
    }

    inline const lanim::SkeletonPose* AnimObject::getSkeletonPose() const
    {
        return skeletonPose_;
    }
}

#endif //INC_LSCENE_ANIMOBJECT_H__
