#ifndef INC_LRENDER_VISIBILITY_H__
#define INC_LRENDER_VISIBILITY_H__
/**
@file Visibility.h
@author t-sakai
@date 2015/09/24 create
*/
#include "../lrender.h"
#include "core/Ray.h"

namespace lrender
{
    class Scene;

    class Visibility
    {
    public:
        Visibility()
        {}

        explicit Visibility(const Ray& ray)
            :ray_(ray)
        {}

        void setSegment(
            const Vector3& p0, f32 eps0,
            const Vector3& p1, f32 eps1);

        void setRay(
            const Vector3& p, f32 eps, f32 tmax,
            const Vector3& w);

        bool unoccluded(const Scene& scene) const;

        Ray ray_;
    };
}
#endif //INC_LRENDER_VISIBILITY_H__
