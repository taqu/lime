/**
@file Scene.cpp
@author t-sakai
@date 2013/05/23 create
*/
#include "scene/Scene.h"

#include <lmath/Quaternion.h>

//#include <stdio.h>

namespace lrender
{
    Scene::Scene()
        :worldMaxSize_(0.0f)
        ,camera_(NULL)
        ,globalRefractiveIndex_(1.0f)
    {
        worldBound_.setInvalid();

        for(s32 i=0; i<NumSHCoefficients; ++i){
            shRCoefficients_[i] = 0.0f;
            shGCoefficients_[i] = 0.0f;
            shBCoefficients_[i] = 0.0f;
        }

        shRMatrix_.zero();
        shGMatrix_.zero();
        shBMatrix_.zero();
    }

    Scene::~Scene()
    {
        shapes_.clear();
    }

    void Scene::add(Shape::ShapeVector& shapes)
    {
        s32 id = shapes_.size();
        for(s32 i=0; i<shapes.size(); ++i){
            shapes[i]->setID(id);
            shapes_.push_back(shapes[i]);

            AABB b;
            shapes_[i]->getBBox(b);
            worldBound_.extend(b);

            ++id;
        }
    }

    void Scene::clearShapes()
    {
        shapes_.clear();
        worldBound_.setInvalid();
    }

    void Scene::add(Emitter::pointer& emitter)
    {
        emitters_.push_back(emitter);
    }

    void Scene::clearEmitters()
    {
        emitters_.clear();
    }

    void Scene::calcSphericalHarmonicCoefficient()
    {
        f64 shRCoefficients[NumSHCoefficients];
        f64 shGCoefficients[NumSHCoefficients];
        f64 shBCoefficients[NumSHCoefficients];

        for(s32 i=0; i<NumSHCoefficients; ++i){
            shRCoefficients[i] = 0.0f;
            shGCoefficients[i] = 0.0f;
            shBCoefficients[i] = 0.0f;
        }

        f64 delta = PI_64/720.0;

        lmath::Vector4 direction;

        s32 nSamples = 0;
        //integral 0 Pi
        for(f64 theta=0.0; theta<=PI_64; theta += delta){

            //integral 0 2*Pi
            for(f64 phi=0.0f; phi<=PI2_64; phi += delta){

                ++nSamples;

                direction.set(
                    lmath::sin(theta)*lmath::cos(phi),
                    lmath::sin(theta)*lmath::sin(phi),
                    -lmath::cos(theta),
                    0.0f);
                direction = normalize(direction);

                f32 u, v;
                lrender::directionToLightProbeCoord(u, v, direction.x_, direction.y_, direction.z_);
                f32 r, g, b;
                sphereImage_.sampleLinearRGB(r, g, b, u, 1.0f-v);


                for(s32 l=0; l<SHBands; ++l){
                    for(s32 m=-l; m<=l; ++m){
                        s32 index = l*(l+1)+m;
                        f64 coefficient = sh::SH(l, m, theta, phi);

                        shRCoefficients[index] += coefficient * r;
                        shGCoefficients[index] += coefficient * g;
                        shBCoefficients[index] += coefficient * b;
                    }
                }

            } //for(f64 phi=0.0f;
        } //for(f64 theta=0.0;

        //‹…‚Ì–ÊÏ=4*Pi*r*r
        f64 scaleFactor = 4.0*PI_64/nSamples;

        for(s32 i=0; i<NumSHCoefficients; ++i){
            shRCoefficients[i] *= scaleFactor * sh::SHCartesianCoefficient(i);
            shGCoefficients[i] *= scaleFactor * sh::SHCartesianCoefficient(i);
            shBCoefficients[i] *= scaleFactor * sh::SHCartesianCoefficient(i);

            shRCoefficients_[i] = static_cast<f32>(shRCoefficients[i]);
            shGCoefficients_[i] = static_cast<f32>(shGCoefficients[i]);
            shBCoefficients_[i] = static_cast<f32>(shBCoefficients[i]);
        }

        sh::calcSHMatrix(shRMatrix_, shRCoefficients_);
        sh::calcSHMatrix(shGMatrix_, shGCoefficients_);
        sh::calcSHMatrix(shBMatrix_, shBCoefficients_);

        //for(s32 l=0; l<SHBands; ++l){
        //    printf("[%d] ", l);

        //    for(s32 m=-l; m<=l; ++m){
        //        s32 index = l*(l+1)+m;

        //        printf("%f  ", shRCoefficients_[index]);
        //    }

        //    printf("\n");
        //}
    }

    void Scene::sampleSphericalHarmonic(f32& r, f32& g, f32& b, f32 x, f32 y, f32 z) const
    {
        f32 v[7];

        r = shRCoefficients_[0];
        g = shGCoefficients_[0];
        b = shBCoefficients_[0];

        s32 index = 1*(1+1);
        r += shRCoefficients_[index-1] * y;
        r += shRCoefficients_[index] * z;
        r += shRCoefficients_[index+1] * x;

        g += shGCoefficients_[index-1] * y;
        g += shGCoefficients_[index] * z;
        g += shGCoefficients_[index+1] * x;

        b += shBCoefficients_[index-1] * y;
        b += shBCoefficients_[index] * z;
        b += shBCoefficients_[index+1] * x;


        v[0] = x*y;
        v[1] = y*z;
        v[2] = 3.0f*z*z - 1.0f;
        v[3] = z*x;
        v[4] = x*x - y*y;
        index = 2*(2+1) - 2;
        for(s32 i=0; i<5; ++i){
            r += shRCoefficients_[index] * v[i];
            g += shGCoefficients_[index] * v[i];
            b += shBCoefficients_[index] * v[i];
            ++index;
        }


        v[0] = 3.0f*x*x*y - y*y*y;
        v[1] = x*y*z;
        v[2] = y*(5.0f*z*z - 1.0f);
        v[3] = 5.0f*z*z*z - 3.0f*z;
        v[4] = x*(5.0f*z*z - 1.0f);
        v[5] = z*(x*x - y*y);
        v[6] = (x*x*x - 3.0f*x*y*y);
        index = 3*(3+1) - 3;

        for(s32 i=0; i<7; ++i){
            r += shRCoefficients_[index] * v[i];
            g += shGCoefficients_[index] * v[i];
            b += shBCoefficients_[index] * v[i];
            ++index;
        }

        if(r<0.0f){
            r = 0.0f;
        }

        if(g<0.0f){
            g = 0.0f;
        }

        if(b<0.0f){
            b = 0.0f;
        }
    }

    void Scene::sampleSphericalHarmonicMatrix(f32& r, f32& g, f32& b, f32 x, f32 y, f32 z) const
    {
        lmath::Vector4 n(x, y, z, 1.0f);
        lmath::Vector4 vr = mul(shRMatrix_, n);
        lmath::Vector4 vg = mul(shGMatrix_, n);
        lmath::Vector4 vb = mul(shBMatrix_, n);

        r = dot(vr, n);
        g = dot(vg, n);
        b = dot(vb, n);

        if(r<0.0f){
            r = 0.0f;
        }

        if(g<0.0f){
            g = 0.0f;
        }

        if(b<0.0f){
            b = 0.0f;
        }
    }

    void Scene::initRender()
    {
        bvh_.clearShapes();
        for(s32 i=0; i<shapes_.size(); ++i){
            bvh_.addShape(shapes_[i]);
        }
        bvh_.build();
        AABB worldBound = getWorldBound();
        worldBound.extend(camera_->getPosition());

        Vector3 extent = worldBound.extent();
        worldMaxSize_ = extent.length() * RAY_MAX_RATIO_OF_WORLD_SIZE;
    }
}
