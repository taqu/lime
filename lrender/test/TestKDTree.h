#include <gtest/gtest.h>
#include "accelerator/KDTree.h"

namespace lrender
{
    class TestKDTree : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };


    TEST_F(TestKDTree, TestGather)
    {
        static const s32 numSamples = 16;
        typedef lrender::KDTree<lrender::Point, 1> kdtree_type;
        kdtree_type kdtree;
        lrender::Point points[numSamples] =
        {
            {Vector3(-0.5f, -0.5f, -0.5f)},
            {Vector3( 0.5f, -0.5f, -0.5f)},
            {Vector3(-0.5f,  0.5f, -0.5f)},
            {Vector3( 0.5f,  0.5f, -0.5f)},
            {Vector3(-0.5f, -0.5f,  0.5f)},
            {Vector3( 0.5f, -0.5f,  0.5f)},
            {Vector3(-0.5f,  0.5f,  0.5f)},
            {Vector3( 0.5f,  0.5f,  0.5f)},

            {Vector3(-0.75f, -0.75f, -0.75f)},
            {Vector3( 0.75f, -0.75f, -0.75f)},
            {Vector3(-0.75f,  0.75f, -0.75f)},
            {Vector3( 0.75f,  0.75f, -0.75f)},
            {Vector3(-0.75f, -0.75f,  0.75f)},
            {Vector3( 0.75f, -0.75f,  0.75f)},
            {Vector3(-0.75f,  0.75f,  0.75f)},
            {Vector3( 0.75f,  0.75f,  0.75f)},
        };

        kdtree.build(numSamples, points);

        const lrender::Point* ppoints[8];
        f32 sqrDistances[8];
        kdtree_type::Nearests nearests(8, Vector3(0.0f), 10.0f, sqrDistances, ppoints);
        kdtree.gather(nearests);
    }
}
