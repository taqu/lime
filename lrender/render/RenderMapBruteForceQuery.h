#ifndef INC_LRENDER_RENDERMAPBRUTEFORCEQUERY_H__
#define INC_LRENDER_RENDERMAPBRUTEFORCEQUERY_H__
/**
@file RenderMapBruteForceQuery.h
@author t-sakai
@date 2015/10/07 create
*/
#include "../lrender.h"
#include "../shape/Shape.h"
#include "../accelerator/MidBVH2D.h"

namespace lrender
{
    class Scene;
    class Image;
    class Sampler;
    class Integrator;

    class RenderMapBruteForceQuery
    {
    public:
        RenderMapBruteForceQuery(const Scene* scene, Sampler* sampler, Image* image, Integrator* integrator);
        ~RenderMapBruteForceQuery();

        void initialize();
        void render(Shape::ShapeVector& shapes);
    private:
        RenderMapBruteForceQuery(const RenderMapBruteForceQuery&);
        RenderMapBruteForceQuery& operator=(const RenderMapBruteForceQuery&);

        bool testTexcoordInPrimitive(Intersection& intersection, Shape::ShapeVector& shapes, const Vector2& p);

        const Scene* scene_;
        Sampler* sampler_;
        Image* image_;
        Integrator* integrator_;

        MidBVH2D accelerator_;
    };
}
#endif //INC_LRENDER_RENDERMAPBRUTEFORCEQUERY_H__
