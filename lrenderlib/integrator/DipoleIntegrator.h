#ifndef INC_LRENDER_DIPOLEINTEGRATOR_H__
#define INC_LRENDER_DIPOLEINTEGRATOR_H__
/**
@file DipoleIntegrator.h
@author t-sakai
@date 2015/12/14 create
*/
#include "Integrator.h"
#include "core/AABB.h"
#include "core/SampleTable.h"
#include "core/Buffer.h"
#include "PointOctree.h"

namespace lrender
{
    class DipoleIntegrator : public Integrator
    {
    public:
        struct MeasuredMaterial
        {
            const Char* name_;
            f32 sigmaS_[3];
            f32 sigmaA_[3];
            f32 g_[3];
            f32 eta_;
            f32 albedo_[3];
            f32 diffuseMeanFreePath_[3];
        };

        static const MeasuredMaterial MeasuredMaterialData[];

        struct IrradiancePoint
        {
            IrradiancePoint()
            {}

            IrradiancePoint(const Vector3& p, const Vector3& n, const Color3& E, f32 area)
                :point_(p)
                ,normal_(n)
                ,E_(E)
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

        struct DiffusionReflectance
        {
            Color3 operator()(f32 distance) const;

            void set(
                const Vector3& sigmaS,
                const Vector3& sigmaA,
                const Vector3& g,
                f32 eta);

            f32 computeBSSDF(
                f32 distance,
                f32 sigmaTr,
                f32 alphaPrime,
                f32 meanFreePath,
                f32 zr,
                f32 zv) const;

            //Vector3 diffuseMeanFreePath_;
            Vector3 sigmaA_;
            Vector3 sigmaT_;
            f32 eta_;

            f32 Fdr_; //fresnel diffuse reflectance
            //f32 Fdt_; //fresnel diffuse transmittance
            f32 A_; //(1.0f+Fdr_)/(1.0f-Fdr_);

            Vector3 meanFreePath_;
            //SampleTable alphaPrimeTable_;
            Vector3 alphaPrime_;

            Vector3 sigmaTr_;
            Vector3 zr_;
            Vector3 zv_;
            f32 radius_;
        };

        /**
        @brief ポイントクラウドの八分木
        */
        typedef PointOctreeNode<IrradiancePoint, DiffusionReflectance> PointOctree;

        DipoleIntegrator(
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
        virtual ~DipoleIntegrator();

        virtual void requestSamples(Sampler* sampler);

        virtual Color3 Li(const Ray& ray, IntegratorQuery& query);
        virtual Color4 E(IntegratorQuery& query);

        void preprocess(const Scene& scene);

    protected:
        DipoleIntegrator(const DipoleIntegrator&);
        DipoleIntegrator& operator=(const DipoleIntegrator&);

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
        DiffusionReflectance diffusion_;
    };
}
#endif //INC_LRENDER_DIPOLEINTEGRATOR_H__
