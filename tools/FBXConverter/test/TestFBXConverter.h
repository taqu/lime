#include <gtest/gtest.h>
#include "converter.h"

namespace lfw
{
    class TestFBXConverter : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F(TestFBXConverter, Convert)
    {
        std::string inFile = "data/sphere_bin.fbx";
        std::string outFile = "data/sphere_bin.lm";
        bool forceDDS = true;

        //Fbx
        lfw::Manager manager;

        lfw::Scene scene;
        {
            lfw::Importer importer;
            EXPECT_TRUE(manager.initialize(importer, inFile.c_str()));
            EXPECT_TRUE(manager.import(scene, importer));
        }

        lfw::Converter converter;
        converter.forceDDS_ = forceDDS;
        EXPECT_TRUE(converter.process(scene.scene_));
        EXPECT_TRUE(converter.out(outFile.c_str()));
    }
}
