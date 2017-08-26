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
            ecsManager_ = ECSManager::create(ECSManager::defaultInitParam());
            System::setECSManager(ecsManager_);
        }

        virtual void TearDown()
        {
            ECSManager::destroy(ecsManager_);
            ecsManager_ = NULL;
            System::clear();
        }

        ECSManager* ecsManager_;

    };


    TEST_F(TestEntity, CreateLogicalEntity)
    {
        static const s32 numSamples = 256;
        Entity entities[numSamples];
        ECSManager& ecsManager = *ecsManager_;

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical("");
        }
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
        }
        for(s32 i=0; i<numSamples; ++i){
            ecsManager.destroyEntity(entities[i]);
            EXPECT_TRUE(entities[i].getHandle().isNull());
        }

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical("");
        }
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
        }
    }

    TEST_F(TestEntity, CreateGeometricEntity)
    {
        static const s32 numSamples = 256;
        Entity entities[numSamples];
        ECSManager& ecsManager = *ecsManager_;

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateGeometric("");
        }
        ecsManager.update();
        ecsManager.postUpdate();

        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
            entities[i].getGeometric()->getPosition();
        }
        for(s32 i=0; i<numSamples; ++i){
            ecsManager.destroyEntity(entities[i]);
            EXPECT_TRUE(entities[i].getHandle().isNull());
        }

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateGeometric("");
        }
        for(s32 i=0; i<numSamples; ++i){
            EXPECT_TRUE(ecsManager.isValidEntity(entities[i]));
            for(s32 j=i+1; j<numSamples; ++j){
                EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
            }
            entities[i].getGeometric()->getPosition();
        }
    }
}
