/**
@file ComponentLogicalManager.cpp
@author t-sakai
@date 2016/08/11 create
*/
#include "ecs/ComponentLogicalManager.h"

namespace lfw
{
    ComponentLogicalManager::ComponentLogicalManager(s32 capacity, s32 expandSize)
        :ComponentTreeManager(capacity, expandSize)
    {
    }

    ComponentLogicalManager::~ComponentLogicalManager()
    {
    }

    u8 ComponentLogicalManager::category() const
    {
        return Category;
    }

    ID ComponentLogicalManager::create(Entity entity)
    {
        return parent_type::create(entity);
    }

    void ComponentLogicalManager::initialize()
    {
    }

    void ComponentLogicalManager::terminate()
    {
    }
}
