#ifndef INC_LFW_SAMPLERSET_H__
#define INC_LFW_SAMPLERSET_H__
/**
@file SamplerSet.h
@author t-sakai
@date 2016/12/17 create
*/
#include "../lframework.h"
#include <lgraphics/SamplerStateRef.h>

namespace lgfx
{
    class ContextRef;
}

namespace lfw
{
    template<s32 N>
    class SamplerSet
    {
    public:
        SamplerSet();
        ~SamplerSet();

        inline s32 getNumSamplers() const;
        inline lgfx::SamplerStateRef::pointer_type const * getSamplers();

        bool create(
            s32 index,
            lgfx::TextureFilterType filter,
            lgfx::TextureAddress addressU,
            lgfx::TextureAddress addressV,
            lgfx::TextureAddress addressW,
            lgfx::CmpFunc compFunc,
            f32 borderColor);
        void swap(SamplerSet<N>& rhs);
    private:
        lgfx::SamplerStateRef::pointer_type samplers_[N];
    };

    template<s32 N>
    SamplerSet<N>::SamplerSet()
    {
        for(s32 i=0; i<N; ++i){
            samplers_[i] = NULL;
        }
    }

    template<s32 N>
    SamplerSet<N>::~SamplerSet()
    {
        for(s32 i=0; i<N; ++i){
            LDXSAFE_RELEASE(samplers_[i]);
        }
    }

    template<s32 N>
    inline s32 SamplerSet<N>::getNumSamplers() const
    {
        return N;
    }

    template<s32 N>
    inline lgfx::SamplerStateRef::pointer_type const * SamplerSet<N>::getSamplers()
    {
        return samplers_;
    }

    template<s32 N>
    bool SamplerSet<N>::create(
        s32 index,
        lgfx::TextureFilterType filter,
        lgfx::TextureAddress addressU,
        lgfx::TextureAddress addressV,
        lgfx::TextureAddress addressW,
        lgfx::CmpFunc compFunc,
        f32 borderColor)
    {
        LASSERT(0<=index && index<N);
        LDXSAFE_RELEASE(samplers_[index]);

        D3D11_SAMPLER_DESC desc;
        desc.Filter = static_cast<D3D11_FILTER>(filter);
        desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressU);
        desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressV);
        desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(addressW);
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(compFunc);
        desc.BorderColor[0] = borderColor;
        desc.BorderColor[1] = borderColor;
        desc.BorderColor[2] = borderColor;
        desc.BorderColor[3] = borderColor;
        desc.MinLOD = -D3D11_FLOAT32_MAX;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = lgfx::getDevice().getD3DDevice()->CreateSamplerState(
            &desc,
            &samplers_[index]);
        return SUCCEEDED(hr);
    }

    template<s32 N>
    void SamplerSet<N>::swap(SamplerSet<N>& rhs)
    {
        for(s32 i=0; i<N; ++i){
            lcore::swap(samplers_[i], rhs.samplers_[i]);
        }
    }
}
#endif //INC_LFW_SAMPLERSET_H__
