#ifndef INC_LRENDER_BSDF_H__
#define INC_LRENDER_BSDF_H__
/**
@file BSDF.h
@author t-sakai
@date 2015/09/15 create
*/
#include "../lrender.h"
#include <lcore/String.h>
#include "core/Spectrum.h"

namespace lrender
{
    class Intersection;
    class BSDFResampler;

    //---------------------------------------------------------------------
    //---
    //--- Fresnel
    //---
    //---------------------------------------------------------------------
    f32 fresnelDielectic(f32 cosi, f32 cost, const f32& etai, const f32& etat);
    Color3 fresnelConductor(f32 cosi, const Color3& eta, const Color3& k);

    class Fresnel
    {
    public:
        static const s32 Type_None = 0;
        static const s32 Type_Conductor = 1;
        static const s32 Type_Dielectic = 2;

        inline Fresnel();
        inline Fresnel(const Color3& eta, const Color3& k);
        inline Fresnel(f32 eta_i, f32 eta_t);
        inline ~Fresnel();

        inline s32 getType() const;

        inline void setNone();
        inline void setConductor(const Color3& eta, const Color3& k);
        inline void setDielectic(f32 eta_i, f32 eta_t);

        Color3 evaluate(f32 cosi) const;

        Color3 evalConductor(f32 cosi) const;
        f32 evalDielectic(f32 cosi) const;
        inline f32 getEtaI() const{ return eta_i_;}
        inline f32 getEtaT() const{ return eta_t_;}
    private:
        s32 type_;
        Color3 eta_;
        Color3 k_;
        f32 eta_i_; //ì¸éÀë§
        f32 eta_t_; //èoéÀë§

    };

    inline Fresnel::Fresnel()
        :type_(Type_Dielectic)
        ,eta_i_(1.00028f)
        ,eta_t_(1.43f)
    {
    }

    inline Fresnel::Fresnel(const Color3& eta, const Color3& k)
        :type_(Type_Conductor)
        ,eta_(eta)
        ,k_(k)
        ,eta_i_(1.0f)
        ,eta_t_(1.0f)
    {
    }

    inline Fresnel::Fresnel(f32 eta_i, f32 eta_t)
        :type_(Type_Dielectic)
        ,eta_i_(eta_i)
        ,eta_t_(eta_t)
    {
    }

    inline Fresnel::~Fresnel()
    {
    }

    inline s32 Fresnel::getType() const
    {
        return type_;
    }

    inline void Fresnel::setNone()
    {
        type_ = Type_None;
    }

    inline void Fresnel::setConductor(const Color3& eta, const Color3& k)
    {
        type_ = Type_Conductor;
        eta_ = eta;
        k_ = k;
    }

    inline void Fresnel::setDielectic(f32 eta_i, f32 eta_t)
    {
        type_ = Type_Dielectic;
        eta_i_ = eta_i;
        eta_t_ = eta_t;
    }

    //---------------------------------------------------------------------
    //---
    //--- BSDF
    //---
    //---------------------------------------------------------------------
    class BSDF : public ReferenceCounted
    {
    public:
        typedef lcore::intrusive_ptr<BSDF> pointer;

        enum Type
        {
            Type_None = 0x00U,
            Type_Reflection = (0x01U<<0),
            Type_Transmission = (0x01U<<1),
            Type_Diffuse = (0x01U<<2),
            Type_Specular = (0x01U<<3),

            Type_All_Glossy = (Type_Diffuse | Type_Specular),
            Type_All_Reflection = (Type_All_Glossy | Type_Reflection),
            Type_All_Transmission = (Type_All_Glossy | Type_Transmission),
            Type_All = (Type_All_Reflection | Type_All_Transmission),
            Type_All_Emitter = (Type_All & ~Type_Specular),
        };

        virtual ~BSDF()
        {
        }

        virtual void initialize() =0;

        virtual Color3 f(const Vector3& wo, const Vector3& wi, const Intersection& intersection) const=0;
        Color3 f(const Vector3& wow, const Vector3& wiw, Type type, const Intersection& intersection) const;

        virtual Color3 sample_f(const Vector3& wo, Vector3& wi, f32& pdf, const BSDFSample& sample, Type& sampledType, const Intersection& intersection) const;
        Color3 sample_f(const Vector3& wow, Vector3& wi, Vector3& wiw, const BSDFSample& sample, f32& pdf, Type& sampledType, const Intersection& intersection) const;

        virtual Color3 rho(const Vector3& wo, s32 numSamples, const BSDFSample* samples, Type type, const Intersection& intersection) const;
        virtual Color3 rho(s32 numSamples, const f32* samples0, const BSDFSample* samples1, Type type, const Intersection& intersection) const;

        virtual f32 getPdf(const Vector3& wo, const Vector3& wi, Type type) const;

        inline const lcore::DynamicString& getName() const;
        inline void setName(const Char* name);

        inline Type getType() const;
        inline void setType(Type type);
        inline void unsetType(Type type);
        inline bool checkType(Type type) const;

        inline static bool checkType(Type lhs, Type rhs);
    protected:
        BSDF(const BSDF&);
        BSDF& operator=(const BSDF&);

        BSDF()
            :type_(Type_None)
        {}

        explicit BSDF(s32 type)
            :type_(type)
        {}

        lcore::DynamicString name_;
        s32 type_;
    };

    inline const lcore::DynamicString& BSDF::getName() const
    {
        return name_;
    }

    inline void BSDF::setName(const Char* name)
    {
        name_.assign(name);
    }

    inline BSDF::Type BSDF::getType() const
    {
        return static_cast<Type>(type_);
    }

    inline void BSDF::setType(Type type)
    {
        type_ = static_cast<Type>(type_ | type);
    }

    inline void BSDF::unsetType(Type type)
    {
        type_ = static_cast<Type>(type_ & ~type);
    }

    inline bool BSDF::checkType(Type type) const
    {
        return 0 != (type_&type);
    }

    inline bool BSDF::checkType(Type lhs, Type rhs)
    {
        return 0 != (lhs & rhs);
    }
}
#endif //INC_LRENDER_BSDF_H__
