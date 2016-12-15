#include <gtest/gtest.h>
#include "LOUDS.h"

namespace lcore
{
    class TestLOUDS : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

        static bool procNode(s32 index, Tree<Char>::node_type& node)
        {
            printf("[%d] %c\n", index, node.getLabel());
            return true;
        }
    };

    TEST_F(TestLOUDS, LOUDS)
    {
        FullyIndexableDictionary fid(256);

        s32 offset = 0;
        for(s32 i=0; i<offset; ++i){
            fid.add(i, false);
        }
        //01101100100
        fid.add(offset+0, false);
        fid.add(offset+1, true);
        fid.add(offset+2, true);
        fid.add(offset+3, false);
        fid.add(offset+4, true);
        fid.add(offset+5, true);
        fid.add(offset+6, false);
        fid.add(offset+7, false);
        fid.add(offset+8, true);
        fid.add(offset+9, false);
        fid.add(offset+10, false);

        for(s32 i=0; i<offset; ++i){
            fid.add(i+offset+11, false);
        }

        fid.build();

        //assert(4 == fid.rank_one_brute_force(6));
        //assert(5 == fid.select_one_brute_force(3));

        //assert(5 == fid.rank_one_brute_force(10));
        //assert(8 == fid.select_one_brute_force(4));

        //assert(2 == fid.rank_zero_brute_force(6));
        //assert(7 == fid.select_zero_brute_force(3));

        //assert(5 == fid.rank_zero_brute_force(10));
        //assert(10 == fid.select_zero_brute_force(5));


        EXPECT_EQ(fid.rank_one_brute_force(offset+6), fid.rank_one(offset+6));
        EXPECT_EQ(fid.select_one_brute_force(3), fid.select_one(3));

        EXPECT_EQ(fid.rank_one_brute_force(offset+10), fid.rank_one(offset+10));
        EXPECT_EQ(fid.select_one_brute_force(5), fid.select_one(5));

        EXPECT_EQ(fid.rank_zero_brute_force(offset+6), fid.rank_zero(offset+6));
        EXPECT_EQ(fid.select_zero_brute_force(3), fid.select_zero(3));

        EXPECT_EQ(fid.rank_zero_brute_force(offset+10), fid.rank_zero(offset+10));
        EXPECT_EQ(fid.select_zero_brute_force(5), fid.select_zero(5));

        typedef Tree<Char>::node_type node_type;
        Tree<Char> tree;
        node_type* root = tree.getRoot();
        root->setLabel('a');
        node_type* node = tree.add(root);
        node->setLabel('b');
        node = tree.add(root);
        node->setLabel('c');
        node = tree.add(root);
        node->setLabel('d');

        root = node;
        node = tree.add(root);
        node->setLabel('e');
        node = tree.add(root);
        node->setLabel('f');

        //traverse(tree, procNode);

        ByteTrie trie;
        trie.add("key0", "value0");
        trie.add("key1", "value1");
        trie.add("key2", "value2");
        trie.add("key3", "value3");

        EXPECT_STREQ("value0", trie.find("key0"));
        EXPECT_STREQ("value1", trie.find("key1"));
        EXPECT_STREQ("value2", trie.find("key2"));
        EXPECT_STREQ("value3", trie.find("key3"));

        LOUDS louds;
        louds.build(trie);
        EXPECT_STREQ("value0", louds.find("key0"));
        EXPECT_EQ(NULL, louds.find("key0te"));
        EXPECT_EQ(NULL, louds.find("key"));
    }
}
