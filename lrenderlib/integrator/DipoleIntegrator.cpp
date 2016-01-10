/**
@file DipoleIntegrator.cpp
@author t-sakai
@date 2015/12/14 create
*/
#include "DipoleIntegrator.h"
#include "core/Ray.h"
#include "core/Intersection.h"
#include "sampler/Sampler.h"
#include "scene/Scene.h"
#include "bsdf/BSDF.h"
#include "bsdf/Microfacet.h"
#include "core/Visibility.h"

#include <render/RenderSurfacePointQuery.h>

namespace lrender
{
    const DipoleIntegrator::MeasuredMaterial DipoleIntegrator::MeasuredMaterialData[] =
    {
        //Henrik Wann Jensen, Stephen R. Marschner, Marc Levoy and Pat Hanrahan: "A Practical Model for Subsurface Light Transport". Proceedings of SIGGRAPH'2001. 
        {"Apple", {2.290000f, 2.390000f, 1.970000f, }, {0.003000f, 0.003400f, 0.046000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.850000f, 0.840000f, 0.530000f, }, {0.069611f, 0.064002f, 0.018959f, }},
        {"Chicken1", {0.150000f, 0.210000f, 0.380000f, }, {0.001500f, 0.077000f, 0.190000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.382990f, 0.038838f, 0.017544f, }},
        {"Chicken2", {0.190000f, 0.250000f, 0.320000f, }, {0.001800f, 0.088000f, 0.200000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.310727f, 0.033476f, 0.017903f, }},
        {"Cream", {7.380000f, 5.470000f, 3.150000f, }, {0.000200f, 0.002800f, 0.016300f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.150276f, 0.046640f, 0.025414f, }},
        {"Ketchup", {0.180000f, 0.070000f, 0.030000f, }, {0.061000f, 0.970000f, 1.450000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.047617f, 0.005748f, 0.003941f, }},
        {"Marble", {2.190000f, 2.620000f, 3.000000f, }, {0.002100f, 0.004100f, 0.007100f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.500000f, {0.000000f, 0.000000f, 0.000000f, }, {0.085094f, 0.055662f, 0.039513f, }},
        {"Potato", {0.680000f, 0.700000f, 0.550000f, }, {0.002400f, 0.009000f, 0.120000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.770000f, 0.620000f, 0.210000f, }, {0.142664f, 0.072276f, 0.020362f, }},
        {"Skimmilk", {0.700000f, 1.220000f, 1.900000f, }, {0.001400f, 0.002500f, 0.014200f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.184244f, 0.104435f, 0.035019f, }},
        {"Skin1", {0.740000f, 0.880000f, 1.010000f, }, {0.032000f, 0.170000f, 0.480000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.036733f, 0.013665f, 0.006827f, }},
        {"Skin2", {1.090000f, 1.590000f, 1.790000f, }, {0.013000f, 0.070000f, 0.145000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.048215f, 0.016937f, 0.010900f, }},
        {"Spectralon", {11.600000f, 20.400000f, 14.900000f, }, {0.000000f, 0.000000f, 0.000000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {10.000000f, 10.000000f, 10.000000f, }},
        {"Wholemilk", {2.550000f, 3.210000f, 3.770000f, }, {0.001100f, 0.002400f, 0.014000f, }, {0.000000f, 0.000000f, 0.000000f, }, 1.300000f, {0.000000f, 0.000000f, 0.000000f, }, {0.108988f, 0.065754f, 0.025084f, }},

        //S. G. Narasimhan, M. Gupta, C. Donner, R. Ramamoorthi, S. K. Nayar, and H. W. Jensen, "Acquiring Scattering Properties of Participating Media by Dilution,". ACM Trans. on Graphics (also Proc. of ACM SIGGRAPH), Jul, 2006.
        {"Milk (lowfat)", {13.115700f, 15.444500f, 17.957199f, }, {0.002870f, 0.005750f, 0.011500f, }, {0.932000f, 0.902000f, 0.859000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.113933f, 0.061771f, 0.033758f, }},
        {"Milk (reduced)", {13.733500f, 15.600300f, 17.800699f, }, {0.002560f, 0.005110f, 0.012780f, }, {0.819000f, 0.797000f, 0.746000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.072338f, 0.045349f, 0.023984f, }},
        {"Milk (regular)", {18.205200f, 20.382601f, 22.369801f, }, {0.001530f, 0.004600f, 0.019930f, }, {0.750000f, 0.714000f, 0.681000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.069176f, 0.035243f, 0.015288f, }},
        {"Coffee (espresso)", {7.782620f, 8.130500f, 8.538750f, }, {4.798380f, 6.575120f, 8.849250f, }, {0.907000f, 0.896000f, 0.880000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.001122f, 0.000827f, 0.000618f, }},
        {"Coffee (mint mocha)", {3.511330f, 4.143830f, 5.596670f, }, {3.772000f, 5.822830f, 7.820000f, }, {0.910000f, 0.907000f, 0.914000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.001470f, 0.000960f, 0.000717f, }},
        {"Soy Milk (lowfat)", {2.038380f, 2.328750f, 3.902810f, }, {0.001440f, 0.007190f, 0.035940f, }, {0.850000f, 0.853000f, 0.842000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.274505f, 0.115171f, 0.037699f, }},
        {"Soy Milk (regular)", {4.663250f, 5.201830f, 8.745750f, }, {0.001920f, 0.009580f, 0.065170f, }, {0.873000f, 0.858000f, 0.832000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.170938f, 0.068192f, 0.018257f, }},
        {"Chocolate Milk (lowfat)", {9.837100f, 11.495400f, 13.162900f, }, {0.011500f, 0.036800f, 0.156400f, }, {0.934000f, 0.927000f, 0.916000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.066233f, 0.032157f, 0.012995f, }},
        {"Chocolate Milk (regular)", {10.568500f, 13.141600f, 15.220200f, }, {0.010060f, 0.043130f, 0.143750f, }, {0.862000f, 0.838000f, 0.806000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.047501f, 0.018863f, 0.008654f, }},
        {"Soda (coke)", {0.002540f, 0.002990f, 0.000000f, }, {0.100140f, 0.165030f, 0.246800f, }, {0.965000f, 0.972000f, 0.000000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.057629f, 0.034976f, 0.023393f, }},
        {"Soda (pepsi)", {0.000830f, 0.002030f, 0.000000f, }, {0.091640f, 0.141580f, 0.207290f, }, {0.926000f, 0.979000f, 0.000000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.062981f, 0.040773f, 0.027852f, }},
        {"Soda (sprite)", {0.000110f, 0.000140f, 0.000140f, }, {0.001890f, 0.001830f, 0.002000f, }, {0.943000f, 0.953000f, 0.952000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {3.049709f, 3.149263f, 2.881914f, }},
        {"Sports Gatorade", {0.036680f, 0.044880f, 0.057420f, }, {0.024790f, 0.019290f, 0.008880f, }, {0.933000f, 0.933000f, 0.935000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.222145f, 0.278388f, 0.545551f, }},
        {"Wine (chardonnay)", {0.000210f, 0.000330f, 0.000480f, }, {0.010780f, 0.011860f, 0.024000f, }, {0.914000f, 0.958000f, 0.975000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.535127f, 0.486520f, 0.240502f, }},
        {"Wine (white zinfandel)", {0.000220f, 0.000330f, 0.000460f, }, {0.012070f, 0.016180f, 0.019840f, }, {0.919000f, 0.943000f, 0.972000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.477982f, 0.356622f, 0.290909f, }},
        {"Wine (merlot)", {0.000810f, 0.000000f, 0.000000f, }, {0.116320f, 0.251910f, 0.294340f, }, {0.974000f, 0.000000f, 0.000000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.049630f, 0.022919f, 0.019615f, }},
        {"Beer (budweiser)", {0.000290f, 0.000550f, 0.000590f, }, {0.011490f, 0.024910f, 0.057790f, }, {0.917000f, 0.956000f, 0.982000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.501955f, 0.231662f, 0.099896f, }},
        {"Beer (coorslight)", {0.000620f, 0.001270f, 0.000000f, }, {0.006160f, 0.013980f, 0.034980f, }, {0.918000f, 0.966000f, 0.000000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.933413f, 0.412347f, 0.165052f, }},
        {"Beer (yuengling)", {0.153500f, 0.332200f, 0.745200f, }, {0.049500f, 0.052100f, 0.059700f, }, {0.969000f, 0.969000f, 0.975000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.111405f, 0.101259f, 0.084428f, }},
        {"Detergent (Clorox)", {0.027310f, 0.033020f, 0.036950f, }, {0.003350f, 0.014890f, 0.026300f, }, {0.912000f, 0.905000f, 0.892000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {1.315101f, 0.352396f, 0.204554f, }},
        {"Detergent (Era)", {0.798700f, 0.574600f, 0.284900f, }, {0.055300f, 0.058600f, 0.090600f, }, {0.949000f, 0.950000f, 0.971000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.079226f, 0.080707f, 0.061004f, }},
        {"Apple Juice", {0.002570f, 0.003110f, 0.004130f, }, {0.012960f, 0.023740f, 0.052180f, }, {0.947000f, 0.949000f, 0.945000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.443164f, 0.242389f, 0.110406f, }},
        {"Cranberry Juice", {0.001960f, 0.002380f, 0.003010f, }, {0.039440f, 0.094220f, 0.124260f, }, {0.947000f, 0.951000f, 0.974000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.146195f, 0.061239f, 0.046448f, }},
        {"Grape Juice", {0.001380f, 0.000000f, 0.000000f, }, {0.104040f, 0.239580f, 0.293250f, }, {0.961000f, 0.000000f, 0.000000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.055479f, 0.024098f, 0.019688f, }},
        {"Ruby Grapefruit Juice", {0.154960f, 0.153910f, 0.159950f, }, {0.085870f, 0.183140f, 0.252620f, }, {0.929000f, 0.929000f, 0.931000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.063302f, 0.030625f, 0.022371f, }},
        {"White Grapefruit Juice", {0.504990f, 0.527420f, 0.752820f, }, {0.013800f, 0.018830f, 0.056780f, }, {0.548000f, 0.545000f, 0.565000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.099895f, 0.082704f, 0.039087f, }},
        {"Shampoo (balancing)", {0.007970f, 0.008740f, 0.011270f, }, {0.014110f, 0.045690f, 0.061720f, }, {0.910000f, 0.905000f, 0.920000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.399158f, 0.125230f, 0.092868f, }},
        {"Shampoo (strawberry)", {0.002150f, 0.002450f, 0.002530f, }, {0.014490f, 0.057960f, 0.075820f, }, {0.927000f, 0.935000f, 0.994000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.396307f, 0.099475f, 0.076140f, }},
        {"Head & Shoulders", {0.267470f, 0.276960f, 0.295740f, }, {0.084620f, 0.156880f, 0.203650f, }, {0.911000f, 0.896000f, 0.884000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.060275f, 0.033827f, 0.026227f, }},
        {"Lemon Tea Powder", {0.744890f, 0.838230f, 1.001580f, }, {2.428810f, 4.575730f, 7.212700f, }, {0.946000f, 0.946000f, 0.949000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.002358f, 0.001256f, 0.000798f, }},
        {"Orange Powder", {0.001930f, 0.002130f, 0.002260f, }, {0.001450f, 0.003440f, 0.007860f, }, {0.919000f, 0.918000f, 0.922000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {3.783014f, 1.637293f, 0.726441f, }},
        {"Pink Lemonade Powder", {0.001230f, 0.001330f, 0.001310f, }, {0.001160f, 0.002370f, 0.003200f, }, {0.902000f, 0.902000f, 0.904000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {4.737113f, 2.371732f, 1.769779f, }},
        {"Cappuccino Powder", {12.209400f, 16.465900f, 29.272699f, }, {35.844101f, 49.547001f, 61.084400f, }, {0.849000f, 0.843000f, 0.926000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.000157f, 0.000114f, 0.000093f, }},
        {"Salt Powder", {0.138050f, 0.156770f, 0.178650f, }, {0.284150f, 0.325700f, 0.341480f, }, {0.802000f, 0.793000f, 0.821000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.019407f, 0.016904f, 0.016167f, }},
        {"Sugar Powder", {0.002820f, 0.003150f, 0.003930f, }, {0.012640f, 0.031050f, 0.050120f, }, {0.921000f, 0.919000f, 0.931000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.452792f, 0.185183f, 0.114883f, }},
        {"Suisse Mocha Powder", {30.084801f, 33.445202f, 38.719101f, }, {17.502001f, 27.004400f, 35.433399f, }, {0.907000f, 0.894000f, 0.888000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.000306f, 0.000201f, 0.000154f, }},
        {"Pacific Ocean Surface Water", {0.001800f, 0.001830f, 0.002280f, }, {0.031840f, 0.031320f, 0.030150f, }, {0.902000f, 0.825000f, 0.914000f, }, 1.330000f, {0.000000f, 0.000000f, 0.000000f, }, {0.180828f, 0.183404f, 0.190873f, }},
        {NULL, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
    };

    void DipoleIntegrator::DiffusionReflectance::set(f32 internalIOR, f32 externalIOR, const Vector3& diffuseMeanFreePath)
    {
        Vector3 ld;
        ld.x_ = lcore::maximum(diffuseMeanFreePath.x_, 1.0e-5f);
        ld.y_ = lcore::maximum(diffuseMeanFreePath.y_, 1.0e-5f);
        ld.z_ = lcore::maximum(diffuseMeanFreePath.z_, 1.0e-5f);
        sigmaTr_ = Vector3(1.0f/ld.x_, 1.0f/ld.y_, 1.0f/ld.z_);

        eta_ = internalIOR/externalIOR;
        Fdr_ = lrender::fresnelDiffuseReflectance(eta_);
        Fdt_ = 1.0f - Fdr_;
        A_ = (1.0f+Fdr_)/Fdt_;

        alphaPrimeTable_.create<ClacDiffuseReflectance>(128, ClacDiffuseReflectance(A_), 0.0f, 1.0f, 1.0e-5f);
    }

    void DipoleIntegrator::DiffusionReflectance::setVertexParameter(
        const Vector3& position,
        const Color3& albedo,
        const Vector3& diffuseMeanFreePath)
    {
        position_ = position;
        mo_.zero();

        Vector3 rd;
        rd.x_ = lcore::minimum(albedo[0], 0.999f);
        rd.y_ = lcore::minimum(albedo[1], 0.999f);
        rd.z_ = lcore::minimum(albedo[2], 0.999f);

        Vector3 alphaPrime;
        alphaPrime.x_ = alphaPrimeTable_.interpolateLinear(rd.x_);
        alphaPrime.y_ = alphaPrimeTable_.interpolateLinear(rd.y_);
        alphaPrime.z_ = alphaPrimeTable_.interpolateLinear(rd.z_);

        for(s32 i=0; i<3; ++i){
            meanFreePath_[i] = diffuseMeanFreePath[i] * lmath::sqrt(3.0f*(1.0f-alphaPrime[i]));

            f32 sigmaTPrime = 1.0f/meanFreePath_[i];
            f32 sigmaSPrime = alphaPrime[i] * sigmaTPrime;
            f32 sigmaA = sigmaTPrime - sigmaSPrime;

            sigmaTrPrime_[i] = lmath::sqrt(3.0f*sigmaA*sigmaTPrime);
        }

        zr_ = meanFreePath_;
        zv_.mul(1.0f+4.0f/3.0f*A_, zr_);
    }

    Color3 DipoleIntegrator::DiffusionReflectance::operator()(f32 sqrDistance) const
    {
        //f32 sqrDmpf = meanFreePath_.lengthSqr();
        //sqrDistance = (sqrDistance<sqrDmpf)? sqrDmpf : sqrDistance;

        Color3 dr, dv, idr, idv;
        for(s32 i=0; i<3; ++i){
            dr[i] = lmath::sqrt(sqrDistance + zr_[i]*zr_[i]);
            dv[i] = lmath::sqrt(sqrDistance + zv_[i]*zv_[i]);

            idr[i] = 1.0f/dr[i];
            idv[i] = 1.0f/dv[i];
        }

        Color3 Rd;
        for(s32 i=0; i<3; ++i){
            f32 c0 = zr_[i] * (sigmaTrPrime_[i] + idr[i]) * lmath::exp(-sigmaTrPrime_[i] * dr[i]) * (idr[i] * idr[i]);
            f32 c1 = zv_[i] * (sigmaTrPrime_[i] + idv[i]) * lmath::exp(-sigmaTrPrime_[i] * dv[i]) * (idv[i] * idv[i]);
            Rd[i] = lcore::maximum(1.0f/(4.0f*PI)*(c0+c1), 0.0f);
        }
        return Rd;
    }

    //--------------------------------------------------------------
    //--- DipoleIntegrator::PointOctree
    //--------------------------------------------------------------
    const f32 DipoleIntegrator::PointOctree::Epsilon = 1.0e-6f;

    DipoleIntegrator::PointOctree::PointOctree()
    {
        clear();
    }

    DipoleIntegrator::PointOctree::PointOctree(IrradiancePointVector& values)
    {
        clear();
        values_.swap(values);
        build();
        initialize();
    }

    DipoleIntegrator::PointOctree::~PointOctree()
    {
    }

    const DipoleIntegrator::IrradiancePointVector&
        DipoleIntegrator::PointOctree::getValues() const
    {
        return values_;
    }

    void DipoleIntegrator::PointOctree::clear()
    {
        nodes_.clear();
        Node root;
        root.num_ = 0;
        root.offset_ = 0;
        nodes_.push_back(root);
    }

    void DipoleIntegrator::PointOctree::swap(PointOctree& rhs)
    {
        lcore::swap(bbox_, rhs.bbox_);
        values_.swap(rhs.values_);
        nodes_.swap(rhs.nodes_);
    }

    void DipoleIntegrator::PointOctree::initialize()
    {
        initialize(0);
    }

    Color3 DipoleIntegrator::PointOctree::Mo(const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const
    {
        return Mo(0, bbox_, point, diffusion, maxSolidAngle);
    }

    void DipoleIntegrator::PointOctree::build()
    {
        if(128 < values_.size()){
            nodes_.reserve(values_.size() >> 1);
            nodes_.setIncSize(values_.size() >> 2);
        } else{
            nodes_.setIncSize(128);
        }

        bbox_.setInvalid();
        for(s32 i=0; i<values_.size(); ++i){
            bbox_.extend(values_[i].getPosition());
        }
        Node& root = nodes_[0];
        root.offset_ = 0;
        root.setNum(values_.size());
        split(0);
    }

    void DipoleIntegrator::PointOctree::initialize(s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        node.point_.zero();
        node.E_ = Color3::black();
        node.area_ = 0.0f;

        f32 sumWeight = 0.0f;

        if(node.isLeaf()){
            s32 start = node.getOffset();
            s32 num = node.getNum();
            s32 end = start + num;
            for(s32 i=start; i<end; ++i){
                const IrradiancePoint& sp = values_[i];
                node.E_ += sp.E_ * sp.area_;
                node.area_ += sp.area_;

                f32 weight = sp.E_.getLuminance() * sp.area_;
                node.point_.muladd(weight, sp.point_, node.point_);
                sumWeight += weight;
            }

        } else{
            for(s32 i=0; i<8; ++i){
                if(0 == node.children_[i]){
                    continue;
                }

                initialize(node.children_[i]);
                Node& childNode = nodes_[node.children_[i]];

                node.E_ += childNode.E_ * childNode.area_;
                node.area_ += childNode.area_;

                f32 weight = childNode.E_.getLuminance() * childNode.area_;
                node.point_.muladd(weight, childNode.point_, node.point_);

                sumWeight += weight;
            }
        }

        if(PDF_EPSILON<sumWeight){
            node.point_ /= sumWeight;
        }
        if(PDF_EPSILON<node.area_){
            node.E_ /= node.area_;
        }
    }

    void DipoleIntegrator::PointOctree::split(s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        node.clear();

        s32 num = node.getNum();
        s32 offset = node.offset_;

        s32 end = offset + num;
        AABB bbox;
        bbox.setInvalid();
        for(s32 i=offset; i<end; ++i){
            bbox.extend(values_[i].getPosition());
        }

        Vector3 extent = bbox.extent();
        bool degenerate = (extent.x_ < Epsilon && extent.y_ < Epsilon && extent.z_ < Epsilon);
        bbox.expand(Epsilon);

        if(num <= MaxNumNodePoints || degenerate){
            node.setLeaf(true);
            return;
        }
        node.setLeaf(false);
        node.setNum(0);

        s32 numPoints[8];
        s32 offsets[8];
        Vector3 pivot = bbox.center();
        value_type* values = &values_[0];

        offsets[0] = offset;
        split(numPoints[0], numPoints[4], offsets[4], offsets[0], num, values, 0, pivot);
        split(numPoints[0], numPoints[2], offsets[2], offsets[0], numPoints[0], values, 1, pivot);
        split(numPoints[0], numPoints[1], offsets[1], offsets[0], numPoints[0], values, 2, pivot);
        split(numPoints[2], numPoints[3], offsets[3], offsets[2], numPoints[2], values, 2, pivot);

        split(numPoints[4], numPoints[6], offsets[6], offsets[4], numPoints[4], values, 1, pivot);
        split(numPoints[4], numPoints[5], offsets[5], offsets[4], numPoints[4], values, 2, pivot);
        split(numPoints[6], numPoints[7], offsets[7], offsets[6], numPoints[6], values, 2, pivot);

        for(s32 i = 0; i < 8; ++i){
            if(numPoints[i] <= 0){
                continue;
            }
            Node child;
            child.setOffset(offsets[i]);
            child.setNum(numPoints[i]);

            nodes_[nodeIndex].children_[i] = nodes_.size();
            nodes_.push_back(child);
        }
        for(s32 i=0; i<8; ++i){
            if(numPoints[i]<=0){
                continue;
            }
            split(nodes_[nodeIndex].children_[i]);
        }
    }

    void DipoleIntegrator::PointOctree::split(
        s32& numLeft,
        s32& numRight,
        s32& offsetRight,
        s32 offset,
        s32 num,
        value_type* values,
        s32 axis,
        const Vector3& pivot)
    {
        values += offset;
        s32 i0 = 0;
        s32 i1 = num - 1;

        for(;;){
            while(i0 < num && values[i0].getPosition()[axis] < pivot[axis]){
                ++i0;
            }

            while(0 <= i1 && pivot[axis] < values[i1].getPosition()[axis]){
                --i1;
            }

            if(i1 <= i0){
                break;
            }
            lcore::swap(values[i0], values[i1]);
            ++i0;
            --i1;
        }

        numLeft = lcore::minimum(i0, num);
        numRight = num - numLeft;
        offsetRight = offset + numLeft;

#if 1
        for(s32 i = 0; i < numLeft; ++i){
            if(pivot[axis] < values[i].getPosition()[axis]){
                lcore::Log("error");
            }
        }
        for(s32 i = numLeft; i < num; ++i){
            if(values[i].getPosition()[axis] < pivot[axis]){
                lcore::Log("error");
            }
        }
#endif
    }

    Color3 DipoleIntegrator::PointOctree::Mo(s32 nodeIndex, const AABB& bbox, const Vector3& point, const DiffusionReflectance& diffusion, f32 maxSolidAngle) const
    {
        const Node& node = nodes_[nodeIndex];

        f32 sqrDistance = point.distanceSqr(node.point_);
        f32 solidAngle = node.area_ / sqrDistance;

        if(!bbox.contains(point) && solidAngle<maxSolidAngle){
            return diffusion(sqrDistance) * node.E_ * node.area_;
        }

        Color3 mo = Color3::black();
        if(node.isLeaf()){
            s32 start = node.offset_;
            s32 end = start + node.getNum();
            for(s32 i=start; i<end; ++i){
                mo += diffusion(point.distanceSqr(values_[i].point_)) * values_[i].E_ * values_[i].area_;
            }

        } else{
            Vector3 pivot = bbox.center();
            for(s32 i=0; i<8; ++i){
                if(0 == node.children_[i]){
                    continue;
                }
                AABB childBBox = octreeChildBound(i, pivot, bbox);
                mo += Mo(node.children_[i], childBBox, point, diffusion, maxSolidAngle);
            }
        }
        return mo;
    }

    DipoleIntegrator::DipoleIntegrator(const Vector3& diffuseMeanFreePath, const Vector3& g, s32 russianRouletteDepth, s32 maxDepth, f32 maxSolidAngle, f32 minSampleDistance, f32 maxSampleDistance, s32 maxSamples)
        :diffuseMeanFreePath_(diffuseMeanFreePath)
        ,g_(g)
        ,russianRouletteDepth_(russianRouletteDepth)
        ,maxDepth_(maxDepth)
        ,maxSolidAngle_(maxSolidAngle)
        ,minSampleDistance_(minSampleDistance)
        ,maxSampleDistance_(maxSampleDistance)
        ,maxSamples_(maxSamples)
    {
    }

    DipoleIntegrator::~DipoleIntegrator()
    {
    }

    void DipoleIntegrator::requestSamples(Sampler* /*sampler*/)
    {
    }

    void DipoleIntegrator::preprocess(const Scene& scene)
    {
        if(scene.getEmitters().size()<=0){
            return;
        }

        diffusion_.set(1.3f, 1.0f, diffuseMeanFreePath_);

        const AABB& worldBound = scene.getWorldBound();
        lrender::Vector3 worldCenter = worldBound.center();

        f32 sampleDistance = lcore::numeric_limits<f32>::maximum();
        for(s32 i=0; i<3; ++i){
            sampleDistance = lcore::minimum(sampleDistance, diffuseMeanFreePath_[i] * 0.70710678118f);
        }
        sampleDistance = lcore::clamp(sampleDistance, minSampleDistance_, maxSampleDistance_);
        lrender::RenderSurfacePointQuery renderSurfacePointQuery(&scene, worldCenter, sampleDistance, maxSamples_);

        renderSurfacePointQuery.initialize();
        renderSurfacePointQuery.render();

        lcore::RandomWELL random(lcore::getDefaultSeed());

        const Emitter::EmitterVector& emitters = scene.getEmitters();
        const lrender::RenderSurfacePointQuery::SurfacePointVector& surfacePoints = renderSurfacePointQuery.getSurfacePoints();
        IrradiancePointVector irradiancePoints;
        irradiancePoints.reserve(surfacePoints.size());
        for(s32 i=0; i<surfacePoints.size(); ++i){
            const SurfacePoint& sp = surfacePoints[i];

            Color3 E=Color3::black();
            for(s32 j=0; j<emitters.size(); ++j){
                const Emitter::pointer& emitter = emitters[j];
                s32 numSamples = lcore::roundUpPow2(emitter->getNumSamples());
                u32 scramble[2] ={random.rand(), random.rand()};

                Color3 e = Color3::black();
                for(s32 k=0; k<numSamples; ++k){
                    EmitterSample emitterSample;
                    lcore::sobol02(emitterSample.u0_, emitterSample.u1_, k, scramble[0], scramble[1]);

                    Visibility visibility;
                    Vector3 wi;
                    f32 lightPdf;
                    Color3 Li = emitter->sample_L(sp.point_, sp.normal_, sp.normal_, LRENDER_INFINITY, emitterSample, wi, lightPdf, visibility);
                    f32 wiDotN = wi.dot(sp.normal_);
                    if(wiDotN<=0.0f){
                        continue;
                    }
                    if(Li.isZero() || lightPdf<PDF_EPSILON){
                        continue;
                    }
                    if(visibility.unoccluded(scene)){
                        e += Li * lcore::absolute(wiDotN)/lightPdf;
                    }
                } //for(s32 k=0;
                E += e/numSamples;

            } //for(s32 j=0;

            irradiancePoints.push_back(IrradiancePoint(sp.point_, sp.normal_, E, sp.area_));
        } //for(s32 i=0;

        //lcore::ofstream filePoints("points.ply");
        //filePoints.print(
        //    "ply\n"
        //    "format ascii 1.0\n"
        //    "element vertex %d\n"
        //    "property double x\n"
        //    "property double y\n"
        //    "property double z\n"
        //    "property uchar red\n"
        //    "property uchar green\n"
        //    "property uchar blue\n"
        //    "end_header\n\n", surfacePoints.size());

        //for(lrender::s32 i=0; i<irradiancePoints.size(); ++i){
        //    u8 r = static_cast<u8>(irradiancePoints[i].E_[0]*255);
        //    u8 g = static_cast<u8>(irradiancePoints[i].E_[1]*255);
        //    u8 b = static_cast<u8>(irradiancePoints[i].E_[2]*255);
        //    filePoints.print("%f %f %f %d %d %d\n", irradiancePoints[i].point_.x_, irradiancePoints[i].point_.y_, irradiancePoints[i].point_.z_, r, g, b);
        //}
        //filePoints.close();

        PointOctree ocree(irradiancePoints);

        octree_.swap(ocree);
    }

    Color3 DipoleIntegrator::Li(const Ray& r, IntegratorQuery& query)
    {
        Ray ray(r);
        query.intersect(ray);

        Color3 L = Color3::black();
        Color3 beta = Color3(1.0f);
        Intersection& insect = query.intersection_;

        EmitterSample emitterSample;
        BSDFSample emitterBsdfSample;
        BSDFSample bsdfBsdfSample;
        while(query.depth_<=maxDepth_ || maxDepth_<0){

            if(!insect.isIntersect()){
                break;
            }

            emitterSample = query.sampler_->next2D();
            emitterBsdfSample = BSDFSample(query.sampler_->next2D(), query.sampler_->next1D());
            bsdfBsdfSample = BSDFSample(query.sampler_->next2D(), query.sampler_->next1D());

            Vector3 wow = -ray.direction_;
            Vector3 wo = insect.worldToLocal(wow);
            const BSDF::pointer& bsdf = insect.shape_->getBSDF();

            if(0.0f<=ray.direction_.dot(insect.geometricNormal_) * LocalCoordinate::cosTheta(wo)){
                break;
            }

            {
                diffusion_.setVertexParameter(insect.point_, bsdf->albedo(insect), diffuseMeanFreePath_);
                Color3 mo = octree_.Mo(insect.point_, diffusion_, maxSolidAngle_);

                Fresnel fresnel(1.0f, diffusion_.eta_);
                Color3 Ft = Color3::white() - fresnel.evaluate(lcore::absolute(wow.dot(insect.shadingNormal_)));
                f32 Fdt = 1.0f - diffusion_.Fdr_;

                //L += (INV_PI * Ft * Fdt * mo);
                L += Fdt * mo;
                //L += mo;
            }

            L += query.sampleEmitterDirect(wow, emitterSample, emitterBsdfSample) * beta;

            Vector3 wi, wiw;
            f32 bsdfPdf;
            BSDF::Type sampledBsdfType = BSDF::Type_All;
            Color3 f = bsdf->sample_f(wow, wi, wiw, bsdfBsdfSample, bsdfPdf, sampledBsdfType, query.intersection_);
            if(f.isZero() || bsdfPdf<=PDF_EPSILON){
                break;
            }
            if(insect.geometricNormal_.dot(wiw)*LocalCoordinate::cosTheta(wi)<=0.0f){
                break;
            }

            beta *= f * lcore::absolute(wiw.dot(insect.shadingNormal_))/bsdfPdf;
            ray = insect.nextRay(wiw, query.scene_->getWorldMaxSize());

            ++query.depth_;

            if(russianRouletteDepth_<=query.depth_){
                f32 continueProbability = lcore::minimum(beta.getLuminance(), 0.9f);
                if(continueProbability <= query.sampler_->get1D()){
                    break;
                }
                beta /= continueProbability;
            }
            query.intersect(ray);
        }

        return L;
    }

    Color4 DipoleIntegrator::E(IntegratorQuery& /*query*/)
    {
        return Color4::black();
    }
}
