#include <gtest/gtest.h>
#include <lcore/io/VirtualFileSystem.h>
#include "resource/ResourceSet.h"
#include "resource/ResourceBytes.h"
#include "resource/Resources.h"
#include <stdio.h>

namespace lfw
{
    class TestResources : public ::testing::Test
    {
    protected:
        static const s32 NumPaths = 127;
        static const s32 PathLength = 64;

        virtual void SetUp()
        {
            for(s32 i=0; i<NumPaths; ++i){
                _snprintf_s(paths_[i], PathLength, "/root%d", i);
            }
        }

        virtual void TearDown()
        {
        }

        lcore::Char paths_[NumPaths][PathLength];
    };

    TEST_F(TestResources, CreateResourceSet)
    {
        ResourceSet resourceSet0;
        EXPECT_EQ(resourceSet0.findIndex(0), -1);
        for(s32 i=0; i<NumPaths; ++i){
            Resource::pointer resource(ResourceBytes::load(8, LNEW u8[8]));
            resource->setID(calcHash(paths_[i]));
            EXPECT_TRUE(resourceSet0.add(resource));
        }

        for(s32 i=0; i<NumPaths; ++i){
            u64 id = calcHash(paths_[i]);
            s32 index = resourceSet0.findIndex(id);
            EXPECT_GE(index, 0);
            const Resource::pointer& resource = resourceSet0.get(index);
            EXPECT_EQ(resource->getID(), id);
            EXPECT_EQ(resource->getReferenceCount(), 1);
        }
        s32 Half = NumPaths/2;
        for(s32 i=0; i<Half; ++i){
            u64 id = calcHash(paths_[i]);
            EXPECT_TRUE(resourceSet0.remove(id));
        }
        for(s32 i=0; i<Half; ++i){
            u64 id = calcHash(paths_[i]);
            EXPECT_LT(resourceSet0.findIndex(id), 0);
        }
        for(s32 i=Half; i<NumPaths; ++i){
            u64 id = calcHash(paths_[i]);
            EXPECT_GE(resourceSet0.findIndex(id), 0);
        }

        ResourceSet resourceSet1;
        resourceSet1 = resourceSet0;

        for(s32 i=Half; i<NumPaths; ++i){
            u64 id = calcHash(paths_[i]);
            s32 index0 = resourceSet0.findIndex(id);
            s32 index1 = resourceSet1.findIndex(id);
            const Resource::pointer& resource0 = resourceSet0.get(index0);
            const Resource::pointer& resource1 = resourceSet1.get(index1);
            EXPECT_EQ(resource0, resource1);
            EXPECT_EQ(resource0->getReferenceCount(), 2);
            EXPECT_EQ(resource1->getReferenceCount(), 2);

            resourceSet0.remove(id);
            EXPECT_EQ(resource1->getReferenceCount(), 1);
        }
    }
}
