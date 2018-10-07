#ifndef INC_LFRAMEWORK_COMPONENTGEOMETRICMANAGER_H_
#define INC_LFRAMEWORK_COMPONENTGEOMETRICMANAGER_H_
/**
@file ComponentGeometricManager.h
@author t-sakai
@date 2016/08/11 create
*/
#include "ComponentTreeManager.h"
#include "ComponentGeometric.h"

#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lmath/Matrix44.h>

namespace lfw
{
    class ComponentGeometricManager : public ComponentTreeManager<ComponentGeometric>
    {
    public:
        typedef ComponentTreeManager<ComponentGeometric> parent_type;
        using parent_type::iterator;
        using parent_type::const_iterator;

        using parent_type::EntityArray;

        static const u8 Category = ECSCategory_Geometric;

        ComponentGeometricManager(s32 capacity=DefaultComponentManagerCapacity, s32 expandSize=DefaultComponentManagerExpandSize);
        virtual ~ComponentGeometricManager();

        virtual u8 category() const;

        virtual ID create(Entity entity);

        virtual void initialize();
        virtual void terminate();

        void updateMatrices();

        inline lmath::Vector4& getPosition(u16 id);
        inline lmath::Quaternion& getRotation(u16 id);
        inline lmath::Vector3& getScale(u16 id);
        inline lmath::Matrix44& getMatrix(u16 id);
    protected:
        ComponentGeometricManager(const ComponentGeometricManager&);
        ComponentGeometricManager& operator=(const ComponentGeometricManager&);

        lmath::Vector4* positions_;
        lmath::Quaternion* rotations_;
        lmath::Vector3* scales_;
        lmath::Matrix44* matrices_;
    };

    inline lmath::Vector4& ComponentGeometricManager::getPosition(u16 id)
    {
        LASSERT(0<=id && id<ids_.capacity());
        return positions_[id];
    }

    inline lmath::Quaternion& ComponentGeometricManager::getRotation(u16 id)
    {
        LASSERT(0<=id && id<ids_.capacity());
        return rotations_[id];
    }

    inline lmath::Vector3& ComponentGeometricManager::getScale(u16 id)
    {
        LASSERT(0<=id && id<ids_.capacity());
        return scales_[id];
    }

    inline lmath::Matrix44& ComponentGeometricManager::getMatrix(u16 id)
    {
        LASSERT(0<=id && id<ids_.capacity());
        return matrices_[id];
    }
}
#endif //INC_LFRAMEWORK_COMPONENTGEOMETRICMANAGER_H_
