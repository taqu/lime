#include <gtest/gtest.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentGeometricManager.h"

namespace lfw
{
    class TestEntity : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            ECSManager::create(ECSManager::defaultInitParam());
        }

        virtual void TearDown()
        {
            ECSManager::destroy();
        }

    };


    TEST_F(TestEntity, CreateEntity)
    {
        static const s32 numSamples = 256;
        Entity entities[numSamples];
        ECSManager& ecsManager = ECSManager::getInstance();

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical();
        }
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
        }
        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical();
            ecsManager.destroyEntity(entities[i]);
            EXPECT_TRUE(entities[i].getHandle().isNull());
        }

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical();
        }
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
        }
    }
}
