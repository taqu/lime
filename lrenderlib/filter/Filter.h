#ifndef INC_LRENDER_FILTER_H__
#define INC_LRENDER_FILTER_H__
/**
@file Filter.h
@author t-sakai
@date 2010/01/18 create

*/
#include "../lrendercore.h"

#include "../sampler/Sampler.h"

namespace lrender
{
    class TextureBuffer;

    class Filter
    {
    public:
        enum Type
        {
            Type_Gaussian,
            Type_Num,
        };

        virtual ~Filter(){}

        virtual void run(
            u32 dstX,
            u32 dstY,
            TextureBuffer& dst,
            TextureBuffer& src) =0;
    protected:
        Filter(){}
    };


    class FilterFactory
    {
    public:
        typedef Filter filter_type;

        struct Param
        {
            Param(f32 variance)
                :variance_(variance)
            {
            }

            f32 variance_;
        };

        static filter_type* create(Filter::Type type, u32 dstW, u32 dstH, u32 srcW, u32 srcH, const Param& param);
    };
}

#endif //INC_LRENDER_FILTER_H__
