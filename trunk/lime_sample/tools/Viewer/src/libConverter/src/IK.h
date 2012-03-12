#ifndef INC_IK_H__
#define INC_IK_H__
/**
@file IK.h
@author t-sakai
@date 2009/11/22 create
*/
#include <lcore/lcore.h>
#include <lgraphics/scene/AnimTree.h>
#include <lmath/Matrix43.h>
namespace lgraphics
{
    class AnimObject;
}

namespace pmd
{
    struct PMDIK;
}

namespace ltools
{
    struct IKNode;

    class IKChain
    {
    public:
        static const lcore::u32 MAX_LENGTH = 255;

        IKChain();
        ~IKChain();

        void set(lcore::u32 length, lcore::u32 effector, lcore::u32 target, lcore::u32 iterations, lcore::f32 weight);

        void create(lgraphics::AnimObject& object);
        void create(lgraphics::AnimObject& object, const pmd::PMDIK& pmdIK);

        void calc(lgraphics::AnimObject& object);
        const lmath::Vector3& getTarget() const{ return target_;}

        void swap(IKChain& rhs);

        inline lcore::u32 getEffectorIndex() const;
        inline lcore::u32 getTargetIndex() const;
        inline lcore::u32 getLength() const;
        inline lcore::u32 getNumIterations() const;
        inline lcore::f32 getWeight() const;

    private:
        void initialize(const lgraphics::AnimObject& object);
        void ccd(const lgraphics::AnimObject& object);
        void update(lgraphics::AnimObject& object);

        void updatePosition(const lgraphics::AnimObject& object);
        void updateLeafChildren(lgraphics::AnimTree& tree, const lmath::Matrix43& transform);

        lcore::u32 length_;
        lcore::u32 numIterations_;
        lcore::f32 weight_;

        // オブジェクト空間のターゲット位置
        lmath::Vector3 target_;

        lmath::Matrix43 rootMatrix_;

        lcore::u32 targetIndex_;
        lcore::u32 effectorIndex_;
        lmath::Matrix43* targetNode_;
        IKNode *nodes_;
    };


    inline lcore::u32 IKChain::getEffectorIndex() const
    {
        return effectorIndex_;
    }

    inline lcore::u32 IKChain::getTargetIndex() const
    {
        return targetIndex_;
    }

    inline lcore::u32 IKChain::getLength() const
    {
        return length_;
    }

    inline lcore::u32 IKChain::getNumIterations() const
    {
        return numIterations_;
    }

    inline lcore::f32 IKChain::getWeight() const
    {
        return weight_;
    }
}

#endif //INC_IK_H__
