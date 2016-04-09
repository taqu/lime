#ifndef INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
#define INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
/**
@file TranslucencyMagnitudeIntegrator.h
@author t-sakai
@date 2015/12/18 create
*/
#include "Integrator.h"
#include "DipoleIntegrator.h"

namespace lrender
{
    class TranslucencyMagnitudeIntegrator : public Integrator
    {
    public:
        struct IrradiancePoint
        {
            IrradiancePoint()
            {}

            IrradiancePoint(const Vector3& p, const Vector3& n, f32 occlusion, f32 area)
                :point_(p)
                ,normal_(n)
                ,E_(occlusion, occlusion, occlusion)
                ,area_(area)
            {}

            const Vector3& getPosition() const
            {
                return point_;
            }

            Vector3 point_;
            Vector3 normal_;
            Color3 E_;
            f32 area_;
        };

        typedef lcore::vector_arena<IrradiancePoint> IrradiancePointVector;

        struct ClacDiffuseReflectance
        {
            ClacDiffuseReflectance(f32 A)
                :A_(A)
            {}

            f32 operator()(f32 alpha_prime) const
            {
                f32 power = -lmath::sqrt(3.0f*(1.0f - alpha_prime));

                f32 t0 = 1.0f + lmath::exp((4.0f / 3.0f)*A_*power);
                f32 t1 = lmath::exp(power);
                return (0.5f*alpha_prime*t0*t1);
            }

            f32 A_;
        };

        /**
        @brief ポイントクラウドの八分木
        */
        typedef PointOctreeNode<IrradiancePoint, DipoleIntegrator::DiffusionReflectance> PointOctree;


        TranslucencyMagnitudeIntegrator(
            const Vector3& sigmaS,
            const Vector3& sigmaA,
            const Vector3& g,
            f32 eta,
            s32 russianRouletteDepth=4,
            s32 maxDepth=5,
            f32 maxSolidAngle=0.1f,
            f32 minSampleDistance=0.01f,
            f32 maxSampleDistance=1.0f,
            s32 maxSamples=10000000);

        void set(
            const Vector3& sigmaS,
            const Vector3& sigmaA,
            const Vector3& g,
            f32 eta,
            s32 russianRouletteDepth=4,
            s32 maxDepth=5,
            f32 maxSolidAngle=0.1f,
            f32 minSampleDistance=0.01f,
            f32 maxSampleDistance=1.0f,
            s32 maxSamples=10000000);

        virtual ~TranslucencyMagnitudeIntegrator();

        virtual void requestSamples(Sampler* sampler);

        virtual Color3 Li(const Ray& ray, IntegratorQuery& query);
        virtual Color4 E(IntegratorQuery& query);

        void preprocess(
            const Scene& scene,
            s32 numOcclusionSamples = 128,
            f32 occlusionGamma = 1.0f,
            f32 occlusionRayLength = lcore::numeric_limits<f32>::maximum());

    protected:
        TranslucencyMagnitudeIntegrator(const TranslucencyMagnitudeIntegrator&);
        TranslucencyMagnitudeIntegrator& operator=(const TranslucencyMagnitudeIntegrator&);

        s32 russianRouletteDepth_;
        s32 maxDepth_;
        f32 maxSolidAngle_;
        f32 minSampleDistance_;
        f32 maxSampleDistance_;
        s32 maxSamples_;
        IrradiancePointVector irradiancePoints_;
        PointOctree::NodeAllocator octreeNodeAllocator_;
        AABB octreeBound_;
        PointOctree* octree_;
        DipoleIntegrator::DiffusionReflectance diffusion_;
    };
}
#endif //INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
