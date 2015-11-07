#ifndef INC_LRENDER_RENDERMAPQUERY_H__
#define INC_LRENDER_RENDERMAPQUERY_H__
/**
@file RenderMapQuery.h
@author t-sakai
@date 2015/10/06 create
*/
#include "../lrender.h"
#include "mapper/Mapper.h"
#include "shape/Shape.h"

namespace lrender
{
    class Scene;
    class Image;
    class Sampler;
    class Integrator;

    class RenderMapQuery
    {
    public:
        RenderMapQuery(const Scene* scene, Sampler* sampler, Image* image, Integrator* integrator);
        ~RenderMapQuery();

        void initialize();
        void render(Shape::ShapeVector& shapes);
    private:
        RenderMapQuery(const RenderMapQuery&);
        RenderMapQuery& operator=(const RenderMapQuery&);

        const Scene* scene_;
        Sampler* sampler_;
        Image* image_;
        Integrator* integrator_;
        Mapper mapper_;
    };
}
#endif //INC_LRENDER_RENDERMAPQUERY_H__
