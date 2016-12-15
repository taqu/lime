#include <gtest/gtest.h>
#include <string.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    class TestComponent : public ::testing::Test
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

    TEST_F(TestComponent, CreateComponent)
    {
        static const s32 numSamples = 256;
        Entity entities[numSamples];
        Char str[numSamples][NameString::Size];
        ECSManager& ecsManager = ECSManager::getInstance();

        for(s32 i=0; i<numSamples; ++i){
            entities[i] = ecsManager.requestCreateLogical();
            EXPECT_FALSE(entities[i].isNull());

            ComponentLogical* componentLogical = entities[i].getLogical();
            EXPECT_FALSE(NULL == componentLogical);
            componentLogical->getName().print("name%d", i);
            sprintf(str[i], "name%d", i);
        }

        for(s32 i=0; i<numSamples; ++i){
            ComponentLogical* componentLogical = entities[i].getLogical();
            EXPECT_FALSE(NULL == componentLogical);
            EXPECT_STREQ(str[i], componentLogical->getName().c_str());

            EXPECT_TRUE(!entities[i].isGeometric());

            ecsManager.destroyEntity(entities[i]);
        }
    }
}
