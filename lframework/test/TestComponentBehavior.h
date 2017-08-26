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
            virtual void onStart()
            {
                geometric_ = getEntity().getGeometric();
            }
            virtual void update()
            {
                geometric_->getPosition();
            }
            lfw::ComponentGeometric* geometric_;
        };

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

    TEST_F(TestComponentBehavior, CreateComponent)
    {
        static const s32 Size = 256;
        static const s32 HalfSize = Size/2;
        Entity entities[Size];
        ECSManager& ecsManager = *ecsManager_;
        for(s32 i=0; i<HalfSize; ++i){
            entities[i] = ecsManager.requestCreateGeometric("");
            ASSERT_FALSE(entities[i].isNull());

            ComponentGeometric* componentGeometric = entities[i].getGeometric();
            ASSERT_TRUE(NULL != componentGeometric);

            entities[i].addComponent<ComponentBehavior00>();
        }

        ecsManager.update();
        ecsManager.postUpdate();

        for(s32 i=HalfSize; i<Size; ++i){
            entities[i] = ecsManager.requestCreateGeometric("");
            ASSERT_FALSE(entities[i].isNull());

            ComponentGeometric* componentGeometric = entities[i].getGeometric();
            ASSERT_TRUE(NULL != componentGeometric);

            entities[i].addComponent<ComponentBehavior00>();
        }

        ecsManager.update();
        ecsManager.postUpdate();
        ecsManager.update();
        ecsManager.postUpdate();
    }
}
