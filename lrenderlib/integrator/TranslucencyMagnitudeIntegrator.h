#ifndef INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
#define INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
/**
@file TranslucencyMagnitudeIntegrator.h
@author t-sakai
@date 2015/12/18 create
*/
#include "Integrator.h"
#include "core/AABB.h"
#include "core/SampleTable.h"
#include "core/Buffer.h"

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
                ,occlusion_(occlusion)
                ,area_(area)
            {}

            const Vector3& getPosition() const
            {
                return point_;
            }

            Vector3 point_;
            Vector3 normal_;
            f32 occlusion_;
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
            Color3 operator()(f32 sqrDistance) const;

            void set(
                f32 internalIOR,
                f32 externalIOR,
                const Vector3& diffuseMeanFreePath);

            /**
            @param albedo ... [0 1]
            @param diffuseMeanFreePath ...
            */
            void setVertexParameter(
                const Vector3& position,
                const Color3& albedo,
                const Vector3& diffuseMeanFreePath);

            //f32 internalIOR_;
            //f32 externalIOR_;
            f32 eta_;
            f32 maxSolidAngle_; //default 0.2

            f32 Fdr_; //fresnel diffuse reflectance
            f32 Fdt_; //fresnel diffuse transmittance
            f32 A_; //(1.0f+Fdr_)/(1.0f-Fdr_);

            SampleTable alphaPrimeTable_;

            Vector3 sigmaTr_;
            Vector3 sigmaTrPrime_;
            Vector3 zr_;
            Vector3 zv_;
            Vector3 meanFreePath_;

            Vector3 position_;
            Vector3 mo_;
        };

        /**
        @brief ポイントクラウドの八分木
        */
        class PointOctree
        {
        public:
            static const f32 Epsilon;
            static const s32 MaxNumNodePoints = 16;

            typedef IrradiancePoint value_type;

            //--------------------------------------------
            //--- Node
            //--------------------------------------------
            struct Node
            {
            public:
                inline void clear();

                inline s32 getOffset() const;
                inline void setOffset(s32 offset);

                inline s32 getNum() const;
                inline void setNum(s32 num);

                inline bool isLeaf() const;
                inline void setLeaf(bool leaf);

                u32 num_;
                s32 offset_;
                Vector3 point_;
                f32 occlusion_;
                f32 area_;
                s32 children_[8];
            };

            PointOctree();
            PointOctree(IrradiancePointVector& values);
            ~PointOctree();

            const IrradiancePointVector& getValues() const;

            Color3 Mo(const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const;

            void swap(PointOctree& rhs);

        protected:
            PointOctree(const PointOctree&);
            PointOctree& operator=(const PointOctree&);

            typedef lcore::vector_arena<Node, lcore::vector_arena_dynamic_inc_size, lcore::DefaultAllocator> NodeVector;

            void clear();
            void build();
            void initialize();

            void initialize(s32 nodeIndex);
            void split(s32 nodeIndex);
            void split(
                s32& numLeft,
                s32& numRight,
                s32& offsetRight,
                s32 offset,
                s32 num,
                value_type* values,
                s32 axis,
                const Vector3& pivot);

            Color3 Mo(s32 nodeIndex, const AABB& bbox, const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const;

            AABB bbox_;
            NodeVector nodes_;
            IrradiancePointVector values_;
        };

        TranslucencyMagnitudeIntegrator(
            const Vector3& diffuseMeanFreePath = Vector3(1.0f, 1.0f, 1.0f),
            const Vector3& g = Vector3(0.0f, 0.0f, 0.0f),
            s32 russianRouletteDepth=4,
            s32 maxDepth=5,
            f32 maxSolidAngle=0.1f,
            f32 minSampleDistance=0.01f,
            f32 maxSampleDistance=1.0f,
            s32 maxSamples=10000000);

        void set(
            const Vector3& diffuseMeanFreePath = Vector3(1.0f, 1.0f, 1.0f),
            const Vector3& g = Vector3(0.0f, 0.0f, 0.0f),
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
            f32 internalIOR = 1.01f,
            f32 externalIOR = 1.0f,
            s32 numOcclusionSamples = 128,
            f32 occlusionGamma = 1.0f,
            f32 occlusionRayLength = lcore::numeric_limits<f32>::maximum());

    protected:
        TranslucencyMagnitudeIntegrator(const TranslucencyMagnitudeIntegrator&);
        TranslucencyMagnitudeIntegrator& operator=(const TranslucencyMagnitudeIntegrator&);

        Vector3 diffuseMeanFreePath_;
        Vector3 g_;
        s32 russianRouletteDepth_;
        s32 maxDepth_;
        f32 maxSolidAngle_;
        f32 minSampleDistance_;
        f32 maxSampleDistance_;
        s32 maxSamples_;
        PointOctree octree_;
        DiffusionReflectance diffusion_;
    };


    //--------------------------------------------
    //--- DipoleIntegrator::PointOctree::Node
    //--------------------------------------------
    inline void TranslucencyMagnitudeIntegrator::PointOctree::Node::clear()
    {
        point_.zero();
        occlusion_ = 0.0f;
        area_ = 0.0f;
        for(s32 i = 0; i < 8; ++i){
            children_[i] = 0;
        }
    }

    inline s32 TranslucencyMagnitudeIntegrator::PointOctree::Node::getOffset() const
    {
        return offset_;
    }

    inline void TranslucencyMagnitudeIntegrator::PointOctree::Node::setOffset(s32 offset)
    {
        offset_ = offset;
    }

    inline s32 TranslucencyMagnitudeIntegrator::PointOctree::Node::getNum() const
    {
        return static_cast<s32>(num_ & 0x7FFFFFFFU);
    }

    inline void TranslucencyMagnitudeIntegrator::PointOctree::Node::setNum(s32 num)
    {
        num_ = (num_ & 0x80000000U) | (num & 0x7FFFFFFFU);
    }

    inline bool TranslucencyMagnitudeIntegrator::PointOctree::Node::isLeaf() const
    {
        return 0 != (num_ & 0x80000000U);
    }

    inline void TranslucencyMagnitudeIntegrator::PointOctree::Node::setLeaf(bool leaf)
    {
        if(leaf){
            num_ |= 0x80000000U;
        } else{
            num_ &= 0x7FFFFFFFU;
        }
    }
}
#endif //INC_LRENDER_TRANSLUCENCY_MAGNITUDE_INTEGRATOR_H__
