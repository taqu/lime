#include <catch_wrap.hpp>
#include "Queue.h"

namespace lcore
{
    typedef QueuePOD<s32, lcore::ArrayStaticCapacityIncrement<4> > QueuePODType;
    void print(const QueuePODType& queue)
    {
        for(QueuePODType::const_iterator itr = queue.begin(); !queue.end(itr); itr = queue.next(itr)){
            printf("%d, ", queue.get(itr));
        }
        printf("\n");

    }

    void compare(const s32* expects, const QueuePODType& queue)
    {
        s32 count = 0;
        for(QueuePODType::const_iterator itr = queue.begin(); !queue.end(itr); itr = queue.next(itr)){
            EXPECT_EQ(expects[count], queue.get(itr));
            ++count;
        }
    }

    TEST_CASE("TestQueue::QueuePOD")
    {
        QueuePODType queue;
        {
            for(s32 i=0; i<8; ++i){
                queue.push_back(i);
            }
            s32 expects[] = {0,1,2,3,4,5,6,7,};
            compare(expects, queue);
        }
        {
            for(s32 i=7; 0<=i; --i){
                queue.push_front(i);
            }
            s32 expects[] ={0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, };
            compare(expects, queue);
        }
        {
            for(s32 i=0; i<4; ++i){
                s32 x;
                queue.try_pop_back(x);
            }
            s32 expects[] ={0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, };
            compare(expects, queue);
        }
        {
            for(s32 i=0; i<4; ++i){
                s32 x;
                queue.try_pop_front(x);
            }
            s32 expects[] ={4, 5, 6, 7, 0, 1, 2, 3, };
            compare(expects, queue);
        }
        {
            for(s32 i=0; i<4; ++i){
                queue.push_back(i);
                queue.push_back(i);
                queue.push_front(i);
            }
            s32 expects[] ={ 3, 2, 1, 0, 4, 5, 6, 7, 0, 1, 2, 3, 0, 0, 1, 1, 2, 2, 3, 3,};
            compare(expects, queue);
        }
        {
            for(s32 i=4; i<8; ++i){
                s32 x;
                queue.try_pop_back(x);
                queue.try_pop_front(x);
                queue.push_front(i);
                queue.push_front(i);
                queue.push_back(i);
                //print(queue);
            }
            s32 expects[] ={ 7, 7, 6, 5, 4, 2, 1, 0, 4, 5, 6, 7, 0, 1, 2, 3, 0, 0, 1, 1, 2, 2, 3, 7,};
            compare(expects, queue);
        }

        {
            s32 expects[] ={ 7, 7, 6, 5, 4, 2, 1, 0, 4, 5, 6, 7, 0, 1, 2, 3, 0, 0, 1, 1, 2, 2, 3, 7,};
            while(0<queue.size()){
                s32 x = queue.pop_back();
                EXPECT_EQ(expects[queue.size()], x);
            }
            EXPECT_EQ(0, queue.size());
            for(s32 i=0; i<7; ++i){
                queue.push_front(i);
            }
            for(s32 i=0; i<7; ++i){
                s32 x = queue.pop_back();
                EXPECT_EQ(i, x);
            }
            EXPECT_EQ(0, queue.size());
        }
    }
}
