#include <catch_wrap.hpp>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentGeometricManager.h"

namespace lfw
{
    TEST_CASE("TestEntity")
    {

        ECSManager* ecsManager = NULL;
        SECTION("SetUp")
        {
            ecsManager = ECSManager::create(ECSManager::defaultInitParam());
            System::setECSManager(ecsManager);
        }

        SECTION("CreateLogicalEntity")
        {
            static const s32 numSamples = 256;
            Entity entities[numSamples];

            for(s32 i=0; i<numSamples; ++i){
                entities[i] = ecsManager->requestCreateLogical("");
            }
            for(s32 i=0; i<numSamples; ++i){
                EXPECT_TRUE(ecsManager->isValidEntity(entities[i]));
                for(s32 j=i+1; j<numSamples; ++j){
                    EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
                }
            }
            for(s32 i=0; i<numSamples; ++i){
                ecsManager->destroyEntity(entities[i]);
                EXPECT_TRUE(entities[i].getHandle().isNull());
            }

            for(s32 i=0; i<numSamples; ++i){
                entities[i] = ecsManager->requestCreateLogical("");
            }
            for(s32 i=0; i<numSamples; ++i){
                EXPECT_TRUE(ecsManager->isValidEntity(entities[i]));
                for(s32 j=i+1; j<numSamples; ++j){
                    EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
                }
            }
        }

        SECTION("CreateGeometricEntity")
        {
            static const s32 numSamples = 256;
            Entity entities[numSamples];

            for(s32 i=0; i<numSamples; ++i){
                entities[i] = ecsManager->requestCreateGeometric("");
            }
            ecsManager->update();
            ecsManager->postUpdate();

            for(s32 i=0; i<numSamples; ++i){
                EXPECT_TRUE(ecsManager->isValidEntity(entities[i]));
                for(s32 j=i+1; j<numSamples; ++j){
                    EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
                }
                entities[i].getGeometric()->getPosition();
            }
            for(s32 i=0; i<numSamples; ++i){
                ecsManager->destroyEntity(entities[i]);
                EXPECT_TRUE(entities[i].getHandle().isNull());
            }

            for(s32 i=0; i<numSamples; ++i){
                entities[i] = ecsManager->requestCreateGeometric("");
            }
            for(s32 i=0; i<numSamples; ++i){
                EXPECT_TRUE(ecsManager->isValidEntity(entities[i]));
                for(s32 j=i+1; j<numSamples; ++j){
                    EXPECT_NE(entities[i].getHandle(), entities[j].getHandle());
                }
                entities[i].getGeometric()->getPosition();
            }
        }

        SECTION("TearDown")
        {
            ECSManager::destroy(ecsManager);
            ecsManager = NULL;
            System::clear();
        }
    }
}
