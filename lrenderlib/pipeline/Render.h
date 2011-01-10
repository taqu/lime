#ifndef INC_LRENDER_RENDER_H__
#define INC_LRENDER_RENDER_H__
/**
@file Render.h
@author t-sakai
@date 2010/01/05 create

*/
#include <list>

namespace lrender
{
    class GeometryPreprocessBase
    {
    public:
        virtual ~GeometryPreprocessBase(){}

    protected:
        GeometryPreprocessBase(){}
    };

    class RayTraceBase
    {
    public:
        virtual ~RayTraceBase(){}

    protected:
        RayTraceBase(){}
    };

    class FilterSamplesBase
    {
    public:
        virtual ~FilterSamplesBase(){}

    protected:
        FilterSamplesBase(){}
    };
}

#endif //INC_LRENDER_RENDER_H__
