#ifndef INC_LRENDER_RENDERSURFACEPOINTQUERY_H__
#define INC_LRENDER_RENDERSURFACEPOINTQUERY_H__
/**
@file RenderSurfacePointQuery.h
@author t-sakai
@date 2015/12/01 create
*/
#include "../lrender.h"
#include <lcore/Array.h>

namespace lrender
{
    class Scene;
    class Screen;
    class Sampler;
    class Integrator;

    struct SurfacePoint
    {
        SurfacePoint()
        {}

        SurfacePoint(const Vector3& p, const Vector3& n, f32 area)
            :point_(p)
            ,normal_(n)
            ,area_(area)
        {}

        const Vector3& getPosition() const
        {
            return point_;
        }

        Vector3 point_;
        Vector3 normal_;
        f32 area_;
    };

    class RenderSurfacePointQuery
    {
    public:
        typedef lcore::Array<SurfacePoint> SurfacePointVector;

        RenderSurfacePointQuery(const Scene* scene, const Vector3& origin, f32 minimumSampleDistance=0.25f, s32 maxSamples=10000000);
        ~RenderSurfacePointQuery();

        void initialize();

        void render();

        inline const SurfacePointVector& getSurfacePoints() const;
    private:
        RenderSurfacePointQuery(const RenderSurfacePointQuery&);
        RenderSurfacePointQuery& operator=(const RenderSurfacePointQuery&);

        const Scene* scene_;
        Vector3 origin_;
        f32 minimumSampleDistance_;
        s32 maxSamples_;

        SurfacePointVector surfacePoints_;
    };

    inline const RenderSurfacePointQuery::SurfacePointVector& RenderSurfacePointQuery::getSurfacePoints() const
    {
        return surfacePoints_;
    }
}
#endif //INC_LRENDER_RENDERSURFACEPOINTQUERY_H__
