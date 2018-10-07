#include <catch_wrap.hpp>
#include <string.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    TEST_CASE("TestComponent")
    {

        ECSManager* ecsManager = NULL;
        SECTION("SetUp")
        {
            ecsManager = ECSManager::create(ECSManager::defaultInitParam());
            System::setECSManager(ecsManager);
        }

        SECTION("CreateComponent")
        {
            static const s32 numSamples = 256;
            Entity entities[numSamples];
            Char str[numSamples][NameString::Size];

            for(s32 i=0; i<numSamples; ++i){
                entities[i] = ecsManager->requestCreateLogical("");
                EXPECT_FALSE(entities[i].isNull());

                ComponentLogical* componentLogical = entities[i].getLogical();
                EXPECT_FALSE(NULL == componentLogical);
                componentLogical->getName().print("name%d", i);
                sprintf_s(str[i], "name%d", i);
            }

            for(s32 i=0; i<numSamples; ++i){
                ComponentLogical* componentLogical = entities[i].getLogical();
                EXPECT_FALSE(NULL == componentLogical);
                EXPECT_STR_EQ(str[i], componentLogical->getName().c_str());

                EXPECT_TRUE(!entities[i].isGeometric());

                ecsManager->destroyEntity(entities[i]);
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
