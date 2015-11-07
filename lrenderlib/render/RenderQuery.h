#ifndef INC_LRENDER_RENDERQUERY_H__
#define INC_LRENDER_RENDERQUERY_H__
/**
@file RenderQuery.h
@author t-sakai
@date 2015/09/16 create
*/
#include "../lrender.h"

namespace lrender
{
    class Scene;
    class Screen;
    class Sampler;
    class Integrator;

    class RenderQuery
    {
    public:
        RenderQuery(const Scene* scene, Sampler* sampler, Screen* screen, Integrator* integrator);
        ~RenderQuery();

        void initialize();

        void render();
    private:
        RenderQuery(const RenderQuery&);
        RenderQuery& operator=(const RenderQuery&);

        const Scene* scene_;
        Sampler* sampler_;
        Screen* screen_;
        Integrator* integrator_;
    };
}
#endif //INC_LRENDER_RENDERQUERY_H__
