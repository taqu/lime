#ifndef INC_LRENDER_SCENE_H__
#define INC_LRENDER_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2013/05/09 create
*/
#include "../lrender.h"
#include <lcore/smart_ptr/intrusive_ptr.h>
#include <lcore/Vector.h>
#include "../camera/Camera.h"
#include "../image/Image.h"
#include "../shape/Shape.h"
#include "emitter/Emitter.h"
#include "accelerator/BinQBVH.h"

namespace lrender
{
    class Scene
    {
    public:
        static const u32 NumMaxLights = 64;

        static const s32 SHBands = 4;
        static const s32 NumSHCoefficients = 16;

        Scene();
        ~Scene();

        inline const Camera& getCamera() const;
        inline Camera& getCamera();
        inline void setCamera(Camera::pointer& camera);

        void add(Shape::ShapeVector& shapes);
        void clearShapes();

        void add(Emitter::pointer& emitter);
        void clearEmitters();
        const Emitter::EmitterVector& getEmitters() const{ return emitters_;}

        inline f32 getWorldMaxSize() const;
        inline const AABB& getWorldBound() const;

        inline const Image& getSkySphereImage() const;
        inline void setSkySphereImage(Image& image);

        inline f32 getGlobalRefractiveIndex() const;
        inline void setGlobalRefractiveIndex(f32 refractiveIndex);

        void calcSphericalHarmonicCoefficient();

        void sampleSphericalHarmonic(f32& r, f32& g, f32& b, f32 x, f32 y, f32 z) const;
        void sampleSphericalHarmonicMatrix(f32& r, f32& g, f32& b, f32 x, f32 y, f32 z) const;

        void initRender();
        inline bool intersect(Intersection& intersection, const Ray& ray)  const;
        inline bool intersect(const Ray& ray)  const;
    private:
        Scene(const Scene&);
        Scene& operator=(const Scene&);

        AABB worldBound_;
        f32 worldMaxSize_;
        Camera::pointer camera_;
        Shape::ShapeVector shapes_;
        Emitter::EmitterVector emitters_;

        BinQBVH bvh_;
        Image sphereImage_;

        f32 globalRefractiveIndex_;

        f32 shRCoefficients_[NumSHCoefficients];
        f32 shGCoefficients_[NumSHCoefficients];
        f32 shBCoefficients_[NumSHCoefficients];

        lmath::Matrix44 shRMatrix_;
        lmath::Matrix44 shGMatrix_;
        lmath::Matrix44 shBMatrix_;
    };

    inline const Camera& Scene::getCamera() const
    {
        return *camera_;
    }

    inline Camera& Scene::getCamera()
    {
        return *camera_;
    }

    inline void Scene::setCamera(Camera::pointer& camera)
    {
        camera_ = camera;
    }

    inline f32 Scene::getWorldMaxSize() const
    {
        return worldMaxSize_;
    }

    inline const AABB& Scene::getWorldBound() const
    {
        return worldBound_;
    }

    inline const Image& Scene::getSkySphereImage() const
    {
        return sphereImage_;
    }

    inline void Scene::setSkySphereImage(Image& image)
    {
        sphereImage_.swap(image);
    }

    inline f32 Scene::getGlobalRefractiveIndex() const
    {
        return globalRefractiveIndex_;
    }

    inline void Scene::setGlobalRefractiveIndex(f32 refractiveIndex)
    {
        globalRefractiveIndex_ = refractiveIndex;
    }

    inline bool Scene::intersect(Intersection& intersection, const Ray& ray)  const
    {
        return bvh_.intersect(intersection, ray);
    }

    inline bool Scene::intersect(const Ray& ray)  const
    {
        return bvh_.intersect(ray);
    }
}
#endif //INC_LRENDER_SCENE_H__
