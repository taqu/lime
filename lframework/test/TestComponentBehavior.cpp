#include <catch_wrap.hpp>
#include <string.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/ComponentBehavior.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    class ComponentBehavior00 : public lfw::ComponentBehavior
    {
    public:
        static u8 category(){ return ECSCategory_Behavior; }
        static u8 type(){ return ECSType_Behavior; }

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

    TEST_CASE("TestComponentBehavior")
    {
        ECSManager* ecsManager = NULL;
        SECTION("SetUp")
        {
            ecsManager = ECSManager::create(ECSManager::defaultInitParam());
            System::setECSManager(ecsManager);
        }

        SECTION("CreateComponent")
        {
            static const s32 Size = 256;
            static const s32 HalfSize = Size/2;
            Entity entities[Size];
            for(s32 i=0; i<HalfSize; ++i){
                entities[i] = ecsManager->requestCreateGeometric("");
                CHECK_FALSE(entities[i].isNull());

                ComponentGeometric* componentGeometric = entities[i].getGeometric();
                CHECK(NULL != componentGeometric);

                entities[i].addComponent<ComponentBehavior00>();
            }

            ecsManager->update();
            ecsManager->postUpdate();

            for(s32 i=HalfSize; i<Size; ++i){
                entities[i] = ecsManager->requestCreateGeometric("");
                CHECK_FALSE(entities[i].isNull());

                ComponentGeometric* componentGeometric = entities[i].getGeometric();
                CHECK(NULL != componentGeometric);

                entities[i].addComponent<ComponentBehavior00>();
            }

            ecsManager->update();
            ecsManager->postUpdate();
            ecsManager->update();
            ecsManager->postUpdate();
        }

        SECTION("TearDown")
        {
            ECSManager::destroy(ecsManager);
            ecsManager = NULL;
            System::clear();
        }
    }
}
