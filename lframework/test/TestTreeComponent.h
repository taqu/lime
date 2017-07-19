#include <gtest/gtest.h>
#include "ecs/lecs.h"
#include "ecs/ECSManager.h"
#include "ecs/Component.h"
#include "ecs/ComponentManager.h"
#include "ecs/ComponentLogicalManager.h"
#include "ecs/ComponentLogical.h"
#include "ecs/ComponentGeometricManager.h"
#include "ecs/ComponentGeometric.h"

namespace lfw
{
    class TestTreeComponent : public ::testing::Test
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

    TEST_F(TestTreeComponent, Create)
    {
        //Initialize
        static const s32 Size = 64;
        Char name[Size][NameString::Size];
        Entity entities[Size];
        ECSManager& ecsManager = *ecsManager_;

        for(s32 i=0; i<Size; ++i){
            entities[i] = ecsManager.requestCreateLogical("");
            ASSERT_FALSE(entities[i].isNull());

            ComponentLogical* componentLogical = entities[i].getLogical();
            ASSERT_TRUE(NULL != componentLogical);
            componentLogical->getName().print("name%d", i);
            sprintf_s(name[i], "name%d", i);
        }

        ecsManager.update();

        for(s32 i=0; i<Size; ++i){
            ComponentLogical* componentLogical = entities[i].getLogical();
            ASSERT_TRUE(NULL != componentLogical);
            EXPECT_TRUE(NULL == componentLogical->getParent());
            EXPECT_EQ(0, componentLogical->getNumChildren());
            EXPECT_STREQ(name[i], componentLogical->getName().c_str());

            const ECSNode node = componentLogical->getECSNode();
            EXPECT_EQ(i, node.index());
            EXPECT_EQ(ECSNode::Root, node.parent());
            EXPECT_EQ(0, node.numChildren());
            EXPECT_EQ(ECSNode::Invalid, node.child());
            s32 prev = (0<i)? i-1 : Size-1;
            s32 next = (i<(Size-1))? i+1 : 0;
            EXPECT_EQ(prev, node.prev());
            EXPECT_EQ(next, node.next());
        }

        //Create children of node 0
        static const s32 ChildSize = 64;
        Char childName[ChildSize][NameString::Size];
        Entity children[ChildSize];

        for(s32 i=0; i<ChildSize; ++i){
            children[i] = ecsManager.requestCreateLogical("");
            ASSERT_FALSE(entities[i].isNull());

            ComponentLogical* componentLogical = children[i].getLogical();
            ASSERT_TRUE(NULL != componentLogical);
            componentLogical->setParent(entities[0].getLogical());
            componentLogical->getName().print("child%d", i);
            lcore::snprintf(childName[i], NameString::Size, "child%d", i);
        }

        EXPECT_TRUE(NULL == ComponentLogical::find("///"));
        Char path[NameString::Size*3];
        for(s32 i=0; i<ChildSize; ++i){
            lcore::snprintf(path, NameString::Size*3, "/name0/child%d", i);
            ComponentLogical* componentLogical = ComponentLogical::find(path);
            EXPECT_TRUE(NULL != componentLogical);
            EXPECT_STREQ(childName[i], componentLogical->getName().c_str());
        }

        ComponentLogical* firstComponent = entities[0].getLogical();
        EXPECT_EQ(ChildSize, firstComponent->getNumChildren());
        s32 count = 0;
        for(ComponentLogical::Iterator itr = firstComponent->beginChild();
            !itr.end();
            ++itr)
        {
            ComponentLogical* componentLogical = itr.current();
            EXPECT_TRUE(componentLogical->getEntity() == children[count]);
            EXPECT_EQ(0, componentLogical->getNumChildren());
            EXPECT_STREQ(childName[count], componentLogical->getName().c_str());
            ++count;
        }

        //Destroy children of root
        for(s32 i=0; i<Size; ++i){
            ecsManager.requestDestroy(entities[i]);
        }
        ecsManager.update();
        ecsManager.postUpdate();

        for(s32 i=0; i<Size; ++i){
            EXPECT_FALSE(ecsManager.isValidEntity(entities[i]));
        }
        for(s32 i=0; i<ChildSize; ++i){
            EXPECT_FALSE(ecsManager.isValidEntity(children[i]));
        }
    }
}
