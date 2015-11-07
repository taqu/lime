#include <loader/LoaderPly.h>
#include <loader/LoaderObj.h>
#include <scene/Scene.h>
#include <scene/Screen.h>
#include <sampler/Sampler.h>
#include <image/Image.h>
#include <camera/PerspectiveCamera.h>
#include <sampler/LowDiscrepancySampler.h>
#include <sampler/RandomSampler.h>
#include <filter/GaussianFilter.h>
#include <filter/BoxFilter.h>
#include <emitter/DistantLight.h>
#include <render/RenderQuery.h>
#include <render/RenderMapQuery.h>
#include <render/RenderMapBruteForceQuery.h>
#include <integrator/PathTracer.h>
#include <integrator/MapPathTracer.h>
#include <integrator/AmbientOcclusion.h>
#include <loader/IOPNG.h>

namespace
{
    lrender::Filter* filter_ = NULL;
    lrender::Screen* screen_ = NULL;
    lrender::Scene* scene_ = NULL;
    lrender::Sampler* sampler_ = NULL;
    lrender::Emitter::pointer emitter_(NULL);
    lrender::PerspectiveCamera::pointer camera_(NULL);

    lrender::s32 width = 640;
    lrender::s32 height = 480;

    void initScene()
    {
        filter_ = LIME_NEW lrender::GaussianFilter(0.5f);
        //filter_ = LIME_NEW lrender::BoxFilter();
        screen_ = LIME_NEW lrender::Screen(width, height, filter_);
        lrender::PerspectiveCamera* camera = LIME_NEW lrender::PerspectiveCamera();
        camera->setResolution(screen_->getWidth(), screen_->getHeight());
        camera->perspective(static_cast<lrender::f32>(screen_->getWidth())/screen_->getHeight(), PI*60.0f/180.0f);

        camera->lookAt(lrender::Vector3(0.0f, 6.0f, -10.0f), lrender::Vector3(0.0f, 2.0f, 0.0f), lrender::Vector3(0.0f, 1.0f, 0.0f));
        //camera->lookAt(lrender::Vector3(10.0f, 2.0f, 0.0f), lrender::Vector3(0.0f, 2.0f, 0.0f), lrender::Vector3(0.0f, 1.0f, 0.0f));
        //camera->lookAt(lrender::Vector3(0.0f, 2.0f, -6.0f), lrender::Vector3(0.0f, 2.0f, 1.0f), lrender::Vector3(0.0f, 1.0f, 0.0f));

        camera_ = camera;

        lrender::u32 seed = lcore::getDefaultSeed();
        lcore::Log("seed %d", seed);
        static const lrender::s32 numSamples = 1024;
        sampler_ = LIME_NEW lrender::LowDiscrepancySampler(numSamples, seed, 4);
        //sampler_ = LIME_NEW lrender::RandomSampler(numSamples, seed, 4);
        //emitter_ = LIME_NEW lrender::DistantLight(lmath::Vector3(0.7071067f, 1.4142134f, 0.0f), lrender::Color3(1.0f));
        emitter_ = LIME_NEW lrender::DistantLight(lmath::Vector3(0.25f, 1.0f, 0.0f), lrender::Color3(1.0f));

        scene_ = LIME_NEW lrender::Scene();
        scene_->setCamera(camera_);
        scene_->add(emitter_);

        //lcore::ofstream file("sample.csv", lcore::ios::binary);
        //for(lrender::s32 i=0; i<sampler_->getNumSamples(); ++i){
        //    lrender::f32 v0, v1;
        //    lcore::sobol02(v0, v1, i, seed, seed);
        //    file.print("%f, %f\n", v0, v1);
        //}
    }

    void termScene()
    {
        camera_ = NULL;
        LIME_DELETE(scene_);
        LIME_DELETE(sampler_);
        LIME_DELETE(screen_);
        LIME_DELETE(filter_);
    }
}

inline __m128 load(float x0, float x1, float x2, float x3)
{
    LIME_ALIGN16 lrender::Vector4 v(x0, x1, x2, x3);
    return _mm_load_ps((float*)&v);
}

int main(int argc, char** argv)
{
    lrender::Shape::ShapeVector shapes;

    {
        lrender::LoaderObj loaderObj(false);
        lrender::s32 numMeshes = loaderObj.loadFromFile(shapes, "data/testscene/TestScene.obj");
        //lrender::s32 numMeshes = loaderObj.loadFromFile(shapes, "data/sponza/sponza.obj");
        //lrender::s32 numMeshes = loaderObj.loadFromFile(shapes, "data/sponza/sponza_uv_maya.obj.obj");
        if(numMeshes<=0){
            return 0;
        }
    }

    initScene();
    scene_->add(shapes);
    scene_->initRender();

#if 1
    lrender::PathTracer pathtracer(5);

    lrender::RenderQuery renderQuery(scene_, sampler_, screen_, &pathtracer);
    renderQuery.initialize();

    lcore::Timer<true> timer;
    timer.begin();
    renderQuery.render();
    timer.end();
    lcore::Log("time: render: %f", timer.getAverage());

    screen_->linearToStandardRGB();
    screen_->savePPM("out.ppm");

#else

    //lrender::Vector2 p0(0.920812011f, 0.817484021f);
    //lrender::Vector2 p1(0.903581023f, 0.817310989f);
    //lrender::Vector2 p2(0.920984983f, 0.817310989f);
    //lrender::Image image(1024, 1024);
    //lrender::Mapper mapper;
    //lrender::Mapper::ConservativeSampler sampler;
    //mapper.generateSampler(sampler, image.getWidth(), image.getHeight(), p0, p1, p2);
    //lrender::MapperSample mapperSample;
    //while(sampler.next(mapperSample)){
    //    image.set(mapperSample.b0_, mapperSample.b1_, mapperSample.b2_, mapperSample.texcoordu_, mapperSample.texcoordv_);
    //}
    //image.savePPM("map.ppm");
    //return 0;

    shapes.clear();
    {
        lrender::LoaderObj loaderObj(false);
        //lrender::s32 numRefs = loaderObj.loadFromFile(shapes, "data/sponza/sponza_uv_maya.obj");
        lrender::s32 numRefs = loaderObj.loadFromFile(shapes, "data/testscene/TestScene_uv.obj");
        if(0<numRefs){
            lrender::u32 seed = lcore::getDefaultSeed();
            lcore::Log("seed %d", seed);
            lrender::Sampler* mapSampler = LIME_NEW lrender::LowDiscrepancySampler(128, seed, 4);

            //lrender::AmbientOcclusion ambientOcclution(2, 1.0f);
            lrender::MapPathTracer mapPathTracer;

            lrender::Image image(1024, 1024);
            image.clear(lrender::Color4::clear());
            lrender::RenderMapBruteForceQuery renderMapQuery(scene_, mapSampler, &image, &mapPathTracer);
            renderMapQuery.initialize();

            lcore::Timer<true> timer;
            timer.begin();
            renderMapQuery.render(shapes);
            timer.end();
            lcore::Log("time: render map: %f", timer.getAverage());

            image.linearToStandardRGB();
            image.savePPM("map.ppm");
            LIME_DELETE(mapSampler);
        }
    }

    //lrender::RenderQuery renderQuery(scene_, sampler_, screen_, &pathtracer);
    //renderQuery.initialize();

    //lcore::Timer<true> timer;
    //timer.begin();
    //renderQuery.render();
    //timer.end();
    //lcore::Log("time: render: %f", timer.getAverage());

    //screen_->linearToStandardRGB();
    //screen_->savePPM("out.ppm");
#endif

    termScene();
    return 0;
}
