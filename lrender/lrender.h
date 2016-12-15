#ifndef INC_LRENDER_LRENDER_H__
#define INC_LRENDER_LRENDER_H__
/**
@file lrender.h
@author t-sakai
@date 2013/05/09 create
*/
#include <lcore/lcore.h>
#include <lcore/intrusive_ptr.h>
#include <lmath/lmath.h>
#include <lmath/Vector2.h>
#include <lmath/Vector3.h>
#include <lmath/Vector4.h>

namespace lrender
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    using lcore::Char;

    using lmath::Vector2;
    using lmath::Vector3;
    using lmath::Vector4;

    using lmath::lm64;
    using lmath::lm128;
    using lmath::lm128i;

    using lcore::CharNull;

#define LRENDER_INFINITY lcore::numeric_limits<f32>::maximum()

#define ANGLE_EPSILON (1.0e-6f)
#define RAY_EPSILON (1.0e-5f)
#define FILTER_EPSILON (1.0e-5f)
#define TONEMAPPING_DELTA (1.0e-5f)
#define DOT_EPSILON (1.0e-6f)
#define PDF_EPSILON (1.0e-6f)
#define SHADOW_EPSILON (1.0e-7f)
#define RAY_MAX_RATIO_OF_WORLD_SIZE (1.5f)

    struct Align16Allocator
    {
        static inline void* malloc(u32 size)
        {
            return LALIGNED_MALLOC(size, 16);
        }

#if defined(_DEBUG)
        static inline void* malloc(u32 size, const char* file, int line)
        {
            return LALIGNED_MALLOC_DEBUG(size, 16, file, line);
        }

        static inline void* malloc(u32 size, u32 alignment, const char* file, int line)
        {
            return LALIGNED_MALLOC_DEBUG(size, alignment, file, line);
        }
#else
        static inline void* malloc(u32 size, const char* /*file*/, int /*line*/)
        {
            return LALIGNED_MALLOC(size, 16);
        }

        static inline void* malloc(u32 size, u32 alignment, const char* /*file*/, int /*line*/)
        {
            return LALIGNED_MALLOC(size, alignment);
        }
#endif
        static inline void free(void* mem)
        {
            LALIGNED_FREE(mem, 16);
        }

        static inline void* malloc(u32 size, u32 alignment)
        {
            return LALIGNED_MALLOC(size, alignment);
        }

        static inline void free(void* mem, u32 alignment)
        {
            LALIGNED_FREE(mem, alignment);
        }
    };

    class ReferenceCounted
    {
    public:
        inline void addRef()
        {
            ++referenceCount_;
        }

        inline void release()
        {
            if(0 == --referenceCount_){
                LDELETE_RAW(this);
            }
        }
    protected:
        friend inline void intrusive_ptr_addref(ReferenceCounted* ptr);
        friend inline void intrusive_ptr_release(ReferenceCounted* ptr);

        ReferenceCounted()
            :referenceCount_(0)
        {}

        ~ReferenceCounted()
        {}

        s32 referenceCount_;
    };

    inline void intrusive_ptr_addref(ReferenceCounted* ptr)
    {
        ptr->addRef();
    }

    inline void intrusive_ptr_release(ReferenceCounted* ptr)
    {
        ptr->release();
    }


    enum LightType
    {
        Light_Direction =0,
        Light_Num,
    };

    enum CameraType
    {
        Camera_Normal =0,
        Camera_Num,
    };

    class Color3;
    class Color4;
    class AABB;
    
    void getBBox(AABB& bbox, const Vector3& p0, const Vector3& p1, const Vector3& p2);
    void getNormal(Vector3& normal, const Vector3& p0, const Vector3& p1, const Vector3& p2);

    /**
    @brief v0ÇÃå¸Ç´Çv1Ç…çáÇÌÇπÇÈ
    */
    Vector3 faceForward(const Vector3& v0, const Vector3& v1);
    Vector4 faceForward(const Vector4& v0, const Vector4& v1);

    void orthonormalBasis(Vector3& binormal0, Vector3& binormal1, const Vector3& normal);
    void orthonormalBasis(Vector4& binormal0, Vector4& binormal1, const Vector4& normal);

    Vector2 weightAverage(f32 w0, f32 w1, f32 w2, const Vector2& v0, const Vector2& v1, const Vector2& v2);
    Vector3 weightAverage(f32 w0, f32 w1, f32 w2, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    s32 octreeChildIndex(const Vector3& point, const Vector3& center);
    s32 octreeChildIndex(AABB& childBBox, const Vector3& point, const Vector3& center, const AABB& parentBBox);
    AABB octreeChildBound(s32 childIndex, const Vector3& center, const AABB& bbox);

    //fresnel
    //------------------------------------------------------------------------------
    f32 fresnelForDielectic(f32 csI, f32 csT, f32 intIOR, f32 extIOR);

    /**
    @param
    @param
    @param intIOR ... internal index of refraction
    @param extIOR ... external index of refraction
    */
    f32 fresnelForDielecticRefract(f32& csOut, f32 cs, f32 intIOR, f32 extIOR);

    /**
    @param eta ... (internal index of refraction)/(external index of refraction)
    */
    f32 fresnelDiffuseReflectance(f32 eta);

    //Monte Carlo
    //------------------------------------------------------------------------------
    inline f32 balanceHeuristic(s32 nf, f32 fPdf, s32 ng, f32 gPdf)
    {
        return (nf*fPdf)/(nf*fPdf + ng*gPdf);
    }

    inline f32 powerHeuristic(s32 nf, f32 fPdf, s32 ng, f32 gPdf)
    {
        f32 f = nf*fPdf;
        f32 g = ng*gPdf;
        f32 f2 = f*f;
        return f2/(f2+g*g);
    }

    void directionToLightProbeCoord(f32& u, f32& v, f32 x, f32 y, f32 z);

    /**
    @param theta ... rotation angle around y axis
    @param phi ... rotation angle around -z axis
    @param u
    @param v

    corresponding direction is obtained by rotating (0,0,-1)
    */
    void lightProbeUVToAngle(f32& theta, f32& phi, f32 u, f32 v);

    f32 calcLuminance(f32 r, f32 g, f32 b);
    f32 calcCb(f32 r, f32 g, f32 b);
    f32 calcCr(f32 r, f32 g, f32 b);

    f32 calcR(f32 y, f32 cb, f32 cr);
    f32 calcG(f32 y, f32 cb, f32 cr);
    f32 calcB(f32 y, f32 cb, f32 cr);

    /**
    @brief Linear RGB to standard RGB
    */
    void linearToStandardRGB(s32 width, s32 height, Color3* rgbs);
    void linearToStandardRGB(s32 width, s32 height, Color4* rgbs);

    /**
    @brief Standard RGB to linear RGB
    */
    void standardToLinearRGB(s32 width, s32 height, Color3* rgbs);
    void standardToLinearRGB(s32 width, s32 height, Color4* rgbs);

    /**
    @brief tone-mapping
    */
    void toneMapping(s32 width, s32 height, Color3* rgbs, f32 scale);

    /**
    @brief tone-mapping
    */
    void toneMapping(s32 width, s32 height, Color4* rgbs, f32 scale);

    /**
    @brief tone-mapping
    */
    void toneMapping(s32 width, s32 height, lmath::Vector4* data, f32 scale);


    //ãÖñ í≤òaä÷êî
namespace sh
{
    /**
    @param l ... band [0 N]
    @param m ... [-l l]
    @param theta ... [0 Pi]
    @param phi ... [0 2*Pi]
    */
    f64 SH(s32 l, s32 m, f64 theta, f64 phi);

    /**
    @param index ... l*(l+1)+m
    */
    f32 SHCartesianCoefficient(s32 index);

    void calcSHMatrix(lmath::Matrix44& mat, const f32* coefficient);
}

    bool savePPM(const char* filename, const Color3* rgb, s32 width, s32 height);
    bool savePPM(const char* filename, const Color4* rgba, s32 width, s32 height);


    struct Sample2D
    {
        inline Sample2D()
        {}

        inline Sample2D(f32 x, f32 y)
            :x_(x)
            ,y_(y)
        {}

        f32 x_;
        f32 y_;
    };

    struct BSDFSample
    {
        BSDFSample()
        {}

        BSDFSample(f32 u0, f32 u1, f32 component)
            :u0_(u0)
            ,u1_(u1)
            ,component_(component)
        {}

        BSDFSample(const Sample2D& sample, f32 component)
            :u0_(sample.x_)
            ,u1_(sample.y_)
            ,component_(component)
        {}

        f32 u0_;
        f32 u1_;
        f32 component_;
    };

    struct EmitterSample
    {
        EmitterSample()
        {}

        EmitterSample(f32 u0, f32 u1)
            :u0_(u0)
            ,u1_(u1)
        {}

        EmitterSample(const Sample2D& sample)
            :u0_(sample.x_)
            ,u1_(sample.y_)
        {}

        f32 u0_;
        f32 u1_;
    };

    struct VertexSample
    {
        Vector3 position_;
        Vector3 normal_;
        Vector2 uv_;
        Vector4 color_;
    };

    struct PrimitiveSample
    {
        Vector3 positions_[3];
        Vector3 normals_[3];
        Vector2 uvs_[3];
    };

    struct MapperSample
    {
        s32 texcoordu_;
        s32 texcoordv_;
        f32 b0_;
        f32 b1_;
        f32 b2_;
    };
}
#endif //INC_LRENDER_LRENDER_H__
