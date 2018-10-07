#include <catch_wrap.hpp>
#include "Sort.h"
#include "Random.h"

namespace lcore
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
        LOG_INFO("sort " << num);
        LOG_INFO("---------------");

        RandXorshift random;
        random.srand(getDefaultSeed());

        int* v = LNEW int[num];
        int* tmp = LNEW int[num];

        LOG_INFO("mode " << mode);
        switch(mode)
        {
        case Mode_Random:
            for(int i=0; i<num; ++i){
                v[i] = static_cast<int>( random.rand() % num );
            }
            break;

        case Mode_Ascend:
            for(int i=0; i<num; ++i){
                v[i] = i;
            }
            break;

        case Mode_Descend:
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

            LOG_INFO("std::sort: " << time << " msec");
        }

        if(num<=10000){
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::insertionsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            LOG_INFO("lcore::insertionsort: " << time << " msec");
            if(!ceckAscend(num, tmp)){
                LOG_INFO("error");
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::quicksort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            LOG_INFO("lcore::quicksort: " << time << " msec");
            if(!ceckAscend(num, tmp)){
                LOG_INFO("error");
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::heapsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            LOG_INFO("lcore::heapsort: " << time << " msec");
            if(!ceckAscend(num, tmp)){
                LOG_INFO("error");
            }
        }

        {
            lcore::memcpy(tmp, v, sizeof(int)*num);
            start = lcore::getPerformanceCounter();

            lcore::introsort(num, tmp, cmp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());
            LOG_INFO("lcore::introsort: " << time << " msec");
            if(!ceckAscend(num, tmp)){
                LOG_INFO("error");
            }
        }

        LDELETE_ARRAY(tmp);
        LDELETE_ARRAY(v);
    }

    typedef bool (*CmpFunc)(int, int);
    typedef bool (*CmpPFunc)(int*, int*);

    typedef void (*SortFunc)(lcore::s32, int*, CmpFunc);
    typedef void (*SortPFunc)(lcore::s32, int**, CmpPFunc);

    void outArray(int num, int* v)
    {
        LOG_INFO_PUT("[ ");
        for(int i=0; i<num; ++i){
            LOG_INFO_PUT(v[i] << " ");
        }
        LOG_INFO_PUT("]");
    }

    void checkArray(int num, int* v, SortFunc func)
    {
        LOG_INFO_PUT("check ");
        outArray(num, v);
        LOG_INFO("");

        func(num, v, cmp);
        LOG_INFO_PUT("      ");
        outArray(num, v);
        LOG_INFO("");
    }

    void check(SortFunc func)
    {
        //------------------------------
        LOG_INFO("check 0");
        int* v0 = NULL;
        func(0, v0, cmp);

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
        LOG_INFO("check sort pointer " << num);
        LOG_INFO("---------------");

        lcore::RandXorshift random;
        random.srand(lcore::getDefaultSeed());

        int** v = LNEW int*[num];
        for(int i=0; i<num; ++i){
            int* p = LNEW int;
            *p = static_cast<int>( random.rand() % 10 );
            v[i] = p;
        }

        lcore::ClockType start;
        lcore::f32 time;

        {
            start = lcore::getPerformanceCounter();

            func(num, v, cmpp);

            time = lcore::calcTime(start, lcore::getPerformanceCounter());

            LOG_INFO(time << " msec");
        }

        for(int i=0; i<num; ++i){
            LDELETE(v[i]);
        }
        LDELETE_ARRAY(v);
    }
}

TEST_CASE("TestSort::TestSort")
{
    lcore::System system;

    LOG_INFO("check insertionsort");
    LOG_INFO("-------------------");
    lcore::check(lcore::insertionsort);
    lcore::checkp(10000, lcore::insertionsort);

    LOG_INFO("check quicksort");
    LOG_INFO("-------------------");
    lcore::check(lcore::quicksort);
    lcore::checkp(10000, lcore::quicksort);

    LOG_INFO("check heapsort");
    LOG_INFO("-------------------");
    lcore::check(lcore::heapsort);
    lcore::checkp(10000, lcore::heapsort);

    LOG_INFO("check introsort");
    LOG_INFO("-------------------");
    lcore::check(lcore::introsort);
    lcore::checkp(10000, lcore::introsort);
}
