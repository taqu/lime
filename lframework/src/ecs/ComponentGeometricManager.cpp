/**
@file ComponentGeometricManager.cpp
@author t-sakai
@date 2016/08/11 create
*/
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentGeometric.h"
#include "ecs/ECSManager.h"

namespace lfw
{
    ComponentGeometricManager::ComponentGeometricManager(s32 capacity, s32 expandSize)
        :ComponentTreeManager(capacity, expandSize)
        ,positions_(NULL)
        ,rotations_(NULL)
        ,matrices_(NULL)
    {
        positions_ = LNEW lmath::Vector4[capacity];
        rotations_ = LNEW lmath::Quaternion[capacity];
        matrices_ = LNEW lmath::Matrix44[capacity];
    }

    ComponentGeometricManager::~ComponentGeometricManager()
    {
        LDELETE_ARRAY(matrices_);
        LDELETE_ARRAY(rotations_);
        LDELETE_ARRAY(positions_);
    }

    u8 ComponentGeometricManager::category() const
    {
        return Category;
    }

    ID ComponentGeometricManager::create(Entity entity)
    {
        s32 capacity = ids_.capacity();
        ID id = parent_type::create(entity);
        if(capacity<ids_.capacity()){
            s32 newCapacity = ids_.capacity();
            expand(positions_, capacity, newCapacity);
            expand(rotations_, capacity, newCapacity);
            expand(matrices_, capacity, newCapacity);
        }
        u16 index = id.index();
        positions_[index].zero();
        rotations_[index].identity();
        matrices_[index].identity();
        return id;
    }

    void ComponentGeometricManager::initialize()
    {
    }

    void ComponentGeometricManager::terminate()
    {
    }

    void ComponentGeometricManager::updateMatrices()
    {
        for(const u16* id = entityTree_.beginID(); id != entityTree_.endID(); ++id){
            u16 index = *id;
            rotations_[index].getMatrix(matrices_[index]);
            matrices_[index].translate(positions_[index]);

            u16 parent = entityTree_.get(index).parent();
            if(ECSNode::Root != parent){
                matrices_[index].mul(matrices_[parent], matrices_[index]);
            }
        }
    }
}
