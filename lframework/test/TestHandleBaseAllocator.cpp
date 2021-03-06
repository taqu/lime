#include <catch_wrap.hpp>
#include "lcore/Random.h"
#include "HandleBasedAllocator.h"

namespace lfw
{
    TEST_CASE("TestHandleBasedAllocator")
    {
        SECTION("HandleBasedAllocator")
        {
            static const s32 NumSamples = 1024;
            lcore::RandXorshift random(lcore::getDefaultSeed());

            HandleBasedAllocator allocator;
            HandleBasedAllocator::Handle handles[NumSamples];

            for(s32 i=0; i<NumSamples; ++i){
                u32 size = lcore::range_rclose(random, 1, 129);
                handles[i] = allocator.allocate(size);
                if(HandleBasedAllocator::MaxSize<size){
                    EXPECT_EQ(size, handles[i].size());
                } else{
                    size = (size+HandleBasedAllocator::MinMask) & ~HandleBasedAllocator::MinMask;
                    EXPECT_EQ(size, handles[i].size());
                }
                for(s32 j=0; j<i; ++j){
                    if(handles[i].size()<=HandleBasedAllocator::MaxSize && handles[j].size()<=HandleBasedAllocator::MaxSize){
                        EXPECT_NE(handles[i].offset(), handles[j].offset());
                    }
                }
            }

            for(s32 i=0; i<NumSamples; ++i){
                allocator.deallocate(handles[i]);
            }

            for(s32 i=0; i<NumSamples; ++i){
                u32 size = lcore::range_rclose(random, 1, 129);
                handles[i] = allocator.allocate(size);
                if(HandleBasedAllocator::MaxSize<size){
                    EXPECT_EQ(size, handles[i].size());
                } else{
                    size = (size+HandleBasedAllocator::MinMask) & ~HandleBasedAllocator::MinMask;
                    EXPECT_EQ(size, handles[i].size());
                }
                for(s32 j=0; j<i; ++j){
                    if(handles[i].size()<=HandleBasedAllocator::MaxSize && handles[j].size()<=HandleBasedAllocator::MaxSize){
                        EXPECT_NE(handles[i].offset(), handles[j].offset());
                    }
                }
            }

            for(s32 i=0; i<NumSamples; ++i){
                allocator.deallocate(handles[i]);
            }


            for(s32 i=0; i<NumSamples; ++i){
                u32 size = HandleBasedAllocator::MaxSize;
                handles[i] = allocator.allocate(size);
                if(HandleBasedAllocator::MaxSize<size){
                    EXPECT_EQ(size, handles[i].size());
                } else{
                    size = (size+HandleBasedAllocator::MinMask) & ~HandleBasedAllocator::MinMask;
                    EXPECT_EQ(size, handles[i].size());
                }
                for(s32 j=0; j<i; ++j){
                    if(handles[i].size()<=HandleBasedAllocator::MaxSize && handles[j].size()<=HandleBasedAllocator::MaxSize){
                        EXPECT_NE(handles[i].offset(), handles[j].offset());
                    }
                }
            }

            for(s32 i=0; i<NumSamples; ++i){
                allocator.deallocate(handles[i]);
            }

            allocator.gatherFragments();
            //EXPECT_EQ(128*(NumSamples/256), allocator.countTable(HandleBasedAllocator::MaxSize));
        }
    }
}
