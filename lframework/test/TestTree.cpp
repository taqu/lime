#include <catch_wrap.hpp>
#include "ecs/lecs.h"
#include "ecs/ECSTree.h"

namespace lfw
{
    TEST_CASE("TestTree::Create")
    {
        static const s32 Size = 64;
        ECSEntityTree tree;

        //Initialize
        tree.resize(0, Size);
        ID ids[Size];
        Entity entities[Size];
        for(s32 i=0; i<Size; ++i){
            ids[i] = ID::construct(i);
            entities[i].setHandle(Handle::construct(i));
        }

        //Create children of root
        for(s32 i=0; i<10; ++i){
            tree.create(ids[i].index(), entities[i]);
        }
        for(s32 i=0; i<10; ++i){
            const ECSNode& node = tree.get(ids[i].index());
            EXPECT_EQ(i, node.index());
            EXPECT_EQ(ECSNode::Root, node.parent());
            EXPECT_EQ(0, node.numChildren());
            EXPECT_EQ(ECSNode::Invalid, node.child());
            s32 prev = (0<i)? i-1 : 9;
            s32 next = (i<9)? i+1 : 0;
            EXPECT_EQ(prev, node.prev());
            EXPECT_EQ(next, node.next());
        }

        //Create children of node 0
        for(s32 i=0; i<10; ++i){
            tree.create(ids[10+i].index(), entities[10+i]);
            tree.setParent(tree.get(ids[10+i].index()), tree.get(ids[0].index()));
        }
        const ECSNode& firstNode = tree.get(ids[0].index());
        EXPECT_EQ(10, firstNode.numChildren());
        u16 child = firstNode.child();
        for(s32 i=0; i<firstNode.numChildren(); ++i){
            const ECSNode& node = tree.get(child);
            EXPECT_EQ(i+10, node.index());
            EXPECT_EQ(firstNode.index(), node.parent());
            EXPECT_EQ(0, node.numChildren());
            EXPECT_EQ(ECSNode::Invalid, node.child());
            s32 prev = (0<i)? i-1+10 : 9+10;
            s32 next = (i<9)? i+1+10 : 0+10;
            EXPECT_EQ(prev, node.prev());
            EXPECT_EQ(next, node.next());
            child = node.next();
        }

        //Destroy children of root
        for(s32 i=0; i<10; ++i){
            const ECSNode& node = tree.get(ids[i].index());
            s32 prev = 9;
            s32 next = (i<9)? i+1 : 9;
            EXPECT_EQ(prev, node.prev());
            EXPECT_EQ(next, node.next());

            tree.destroyRecurse(node.index());
            EXPECT_EQ((9-i), tree.getRoot().numChildren());

            EXPECT_EQ(ECSNode::Invalid, node.index());
            EXPECT_EQ(ECSNode::Invalid, node.parent());
            EXPECT_EQ(0, node.numChildren());
            EXPECT_EQ(ECSNode::Invalid, node.child());
            EXPECT_EQ(node.index(), node.prev());
            EXPECT_EQ(node.index(), node.next());
        }
    }
}
