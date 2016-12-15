#include <gtest/gtest.h>
#include <string.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentBehavior.h"

namespace lfw
{
    class TestComponentBehavior : public ::testing::Test
    {
    public:
        class ComponentBehavior00 : public lfw::ComponentBehavior
        {
        public:
            static u8 category(){ return ECSCategory_Behavior;}
            static u8 type(){ return ECSType_Behavior;}

            ComponentBehavior00()
            {}

        };

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

    TEST_F(TestComponentBehavior, CreateComponent)
    {
        static const s32 Size = 256;
        Entity entities[Size];
        ECSManager& ecsManager = ECSManager::getInstance();
        for(s32 i=0; i<Size; ++i){
            entities[i] = ecsManager.requestCreateGeometric();
            ASSERT_FALSE(entities[i].isNull());

            ComponentGeometric* componentGeometric = entities[i].getGeometric();
            ASSERT_TRUE(NULL != componentGeometric);

            entities[i].addComponent<ComponentBehavior00>();
        }

        ecsManager.update();
        ecsManager.update();
    }
}
