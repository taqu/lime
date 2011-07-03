/**
@file Filter.cpp
@author t-sakai
@date 2010/01/18 create

*/
#include "Filter.h"
#include "../pipeline/TextureBuffer.h"

namespace lrender
{
    class GaussianFilter : public Filter
    {
    public:
        GaussianFilter(
            u32 dstW,
            u32 dstH,
            u32 srcW,
            u32 srcH,
            f32 variance);

        virtual ~GaussianFilter();

        virtual void run(
            u32 dstX,
            u32 dstY,
            TextureBuffer& dst,
            TextureBuffer& src);
    private:
        inline f32 calcCoefficient(f32 x, f32 y) const;

        f32 variance_;
        u32 width_;
        u32 height_;
        f32 *coeff_;
    };

    GaussianFilter::GaussianFilter(
        u32 dstW,
        u32 dstH,
        u32 srcW,
        u32 srcH,
        f32 variance)
        :variance_(variance)
    {
        LASSERT(variance_ > F32_EPSILON);
        LASSERT(width_ > 0);
        LASSERT(height_ > 0);
        LASSERT(dstW > 0);
        LASSERT(dstH > 0);

        width_ = (srcW/dstW);
        height_ = (srcH/dstH);
        coeff_ = LIME_NEW f32[width_*height_];


        //確率密度係数計算
        f32 sum = 0.0f;
        for(u32 j=0; j<width_; ++j){
            for(u32 i=0; i<height_; ++i){
                f32 cx = 0.5f*width_ + i;
                f32 cy = 0.5f*height_ + j;

                f32 fx = -cx + i;
                f32 fy = -cy + j;
                fx += (fx>0.0f)? -0.5f : 0.5f;
                fy += (fy>0.0f)? -0.5f : 0.5f;

                f32 tmp = calcCoefficient(fx, fy);

                coeff_[j*width_ + i] = tmp;
                sum += tmp;
            }
        }

        //正規化
        f32 invSum = 1.0f/sum;
        for(u32 j=0; j<width_; ++j){
            for(u32 i=0; i<height_; ++i){
                coeff_[j*width_ + i] *= invSum;
            }
        }
    }

    GaussianFilter::~GaussianFilter()
    {
        LIME_DELETE_ARRAY(coeff_);
    }

    void GaussianFilter::run(
        u32 dstX,
        u32 dstY,
        TextureBuffer& dst,
        TextureBuffer& src)
    {
        u32 dstW = dst.getWidth();
        u32 dstH = dst.getHeight();

        dst.lock();
        src.lock();

        for(u32 j=0; j<dstH; ++j){
            for(u32 i=0; i<dstW; ++i){
                u32 sx = i*width_;
                u32 sy = j*height_;
                u32 ex = sx + width_;
                u32 ey = sy + height_;

                Vector4 dc(0.0f, 0.0f, 0.0f, 0.0f);
                for(u32 l=sy; l<ey; ++l){
                    for(u32 k=sx; k<ex; ++k){
                        u32 fx = k - sx;
                        u32 fy = l - sy;

                        f32 coeff = coeff_[fy*width_ + fx];

                        Vector4 sc = src.get<Vector4>(k, l);
                        sc *= coeff;
                        dc += sc;
                    }
                }
                u32 dx = i + dstX;
                u32 dy = j + dstY;

                u32 R = static_cast<u32>(255 * dc._x + 0.5f);
                u32 G = static_cast<u32>(255 * dc._y + 0.5f);
                u32 B = static_cast<u32>(255 * dc._z + 0.5f);
                u32 A = static_cast<u32>(255 * dc._w + 0.5f);

                if(A>0xFFU){ A = 0xFFU;}
                if(B>0xFFU){ B = 0xFFU;}
                if(G>0xFFU){ G = 0xFFU;}
                if(R>0xFFU){ R = 0xFFU;}

                u32 value = A<<24 | R << 16 | G << 8 | B;
                dst.set(dx, dy, value);
            }
        }

        src.unlock();
        dst.unlock();
    }

    inline f32 GaussianFilter::calcCoefficient(f32 x, f32 y) const
    {
        f32 t = 1.0f/lmath::sqrt(2.0f*PI)/variance_;
        f32 u = -0.5f / (variance_*variance_);
        f32 cy = t * lmath::exp(u*y*y);
        f32 cx = t * lmath::exp(u*x*x);
        return cy*cx;
    }


    FilterFactory::filter_type* FilterFactory::create(
        Filter::Type type,
        u32 dstW,
        u32 dstH,
        u32 srcW,
        u32 srcH,
        const Param& param)
    {
        switch(type)
        {
        case Filter::Type_Gaussian:
            return LIME_NEW GaussianFilter(dstW, dstH, srcW, srcH, param.variance_);
            break;

        default:
            break;
        }
        return NULL;
    }
}
