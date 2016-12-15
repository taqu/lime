#include <gtest/gtest.h>
#include "Sort.h"

namespace lcore
{
    class TestSort : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestSort, SortRandom)
    {

    }
}

#if 0
namespace
{
    bool cmp(int lhs, int rhs)
    {
        return (lhs<rhs);
    }

    bool cmpp(int* lhs, int* rhs)
    {
        return (*lhs<*rhs);
    }

    bool ceckAscend(int num, int* v)
    {
        for(int i=1; i<num; ++i){
            if(v[i]<v[i-1]){
                return false;
            }
        }
        return true;
    }

    enum Mode
    {
        Mode_Random,
        Mode_Ascend,
        Mode_Descend,
    };

    void run(int num, Mode mode)
    {
        std::cout << "sort " << num << std::endl;
        std::cout << "---------------" << std::endl;

        lcore::RandXorshift random;
        random.srand(lcore::getDefaultSeed());

        int* v = LIME_NEW int[num];
        int* tmp = LIME_NEW int[num];

        switch(mode)
        {
        case Mode_Random:
            std::cout << "mode random" << std::endl;
            for(int i=0; i<num; ++i){
                v[i] = static_cast<int>( random.rand() % num );
            }
            break;

        case Mode_Ascend:
            std::cout << "mode ascend" << std::endl;
            for(int i=0; i<num; ++i){
                v[i] = i;
            }
            break;

        case Mode_Descend:
            std::cout << "mode descend" << std::endl;
            for(int i=0; i<num; ++i){
                v[i] = num-i;
            }
            break;
        }

        lcore::ClockType start;
        lcore::f32 time;

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            std::sort(tmp, tmp+num, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());

            std::cout << "std::sort: " << time << " msec" << std::endl;
        }

        if(num<=10000){
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::insertionsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            std::cout << "lcore::insertionsort: " << time << " msec" << std::endl;
            if(!ceckAscend(num, tmp)){
                std::cout << "error" << std::endl;
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::quicksort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            std::cout << "lcore::quicksort: " << time << " msec" << std::endl;
            if(!ceckAscend(num, tmp)){
                std::cout << "error" << std::endl;
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::heapsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            std::cout << "lcore::heapsort: " << time << " msec" << std::endl;
            if(!ceckAscend(num, tmp)){
                std::cout << "error" << std::endl;
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::introsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            std::cout << "lcore::introsort: " << time << " msec" << std::endl;
            if(!ceckAscend(num, tmp)){
                std::cout << "error" << std::endl;
            }
        }

        std::cout << std::endl;
        std::cout << std::endl;

        LIME_DELETE_ARRAY(tmp);
        LIME_DELETE_ARRAY(v);
    }

    typedef bool (*CmpFunc)(int, int);
    typedef bool (*CmpPFunc)(int*, int*);

    typedef void (*SortFunc)(lcore::s32, int*, CmpFunc);
    typedef void (*SortPFunc)(lcore::s32, int**, CmpPFunc);

    void outArray(int num, int* v)
    {
        std::cout << "[ ";
        for(int i=0; i<num; ++i){
            std::cout << v[i] << " ";
        }
        std::cout << "]";
    }

    void checkArray(int num, int* v, SortFunc func)
    {
        std::cout << "check ";
        outArray(num, v);
        std::cout << std::endl;

        func(num, v, cmp);
        std::cout << "      ";
        outArray(num, v);
        std::cout << std::endl;
        std::cout << std::endl;
    }

    void check(SortFunc func)
    {
        //------------------------------
        std::cout << "check 0" << std::endl;
        int* v0 = NULL;
        func(0, v0, cmp);
        std::cout << std::endl;

        //------------------------------
        int v1[] = {0};
        checkArray(1, v1, func);

        //------------------------------
        int v2[] = {1, 0};
        checkArray(2, v2, func);

        //------------------------------
        int v3_0[] = {2, 1, 0};
        checkArray(3, v3_0, func);

        //------------------------------
        int v3_1[] = {1, 2, 0};
        checkArray(3, v3_1, func);

        //------------------------------
        int v3_2[] = {0, 1, 2};
        checkArray(3, v3_2, func);

        //------------------------------
        int v5_0[] = {2, 1, 0, 0, 0};
        checkArray(5, v5_0, func);

        //------------------------------
        int v5_1[] = {0, 1, 0, 1, 0};
        checkArray(5, v5_1, func);


        //------------------------------
        int v20[] = {11, 1, 8, 16, 0, 13, 14, 12, 19, 2, 9, 17, 7, 6, 3, 4, 5, 10, 18, 15,};
        checkArray(20, v20, func);

        //------------------------------
        int v21_0[] = {11, 1, 8, 16, 0, 13, 14, 12, 19, 2, 9, 17, 7, 6, 3, 4, 5, 10, 18, 15, 15, };
        checkArray(21, v21_0, func);

        //------------------------------
        int v21_1[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        checkArray(21, v21_1, func);
    }

    void checkp(int num, SortPFunc func)
    {
        std::cout << "check sort pointer " << num << std::endl;
        std::cout << "---------------" << std::endl;

        lcore::RandXorshift random;
        random.srand(lcore::getDefaultSeed());

        int** v = LIME_NEW int*[num];
        for(int i=0; i<num; ++i){
            int* p = LIME_NEW int;
            *p = static_cast<int>( random.rand() % 10 );
            v[i] = p;
        }

        lcore::ClockType start;
        lcore::f32 time;

        {
            start = lcore::getPerformanceCounter();

            func(num, v, cmpp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());

            std::cout << time << " msec" << std::endl;
        }

        for(int i=0; i<num; ++i){
            LIME_DELETE(v[i]);
        }
        LIME_DELETE_ARRAY(v);
    }
}

int main(int argc, char** argv)
{
    {
        lcore::System system;

        std::cout << "check insertionsort" << std::endl;
        std::cout << "-------------------" << std::endl;
        check(lcore::insertionsort);
        checkp(10000, lcore::insertionsort);

        std::cout << "check quicksort" << std::endl;
        std::cout << "-------------------" << std::endl;
        check(lcore::quicksort);
        checkp(10000, lcore::quicksort);

        std::cout << "check heapsort" << std::endl;
        std::cout << "-------------------" << std::endl;
        check(lcore::heapsort);
        checkp(10000, lcore::heapsort);

        std::cout << "check introsort" << std::endl;
        std::cout << "-------------------" << std::endl;
        check(lcore::introsort);
        checkp(10000, lcore::introsort);

#ifndef _DEBUG
        run(10, Mode_Random);
        run(100, Mode_Random);
        run(10000, Mode_Random);
        run(1000000, Mode_Random);

        run(100000, Mode_Ascend);
        run(100000, Mode_Descend);
        //run(2147483647);
#endif
    }
    return 0;
}
#endif
