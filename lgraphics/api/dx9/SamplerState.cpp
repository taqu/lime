/**
@file SamplerState.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"
#include "../SamplerState.h"

using namespace lcore;

namespace lgraphics
{

    SamplerState::SamplerState()
        :addressU_(TexAddress_Wrap)
        ,addressV_(TexAddress_Wrap)
        ,magFilter_(TexFilter_Linear)
        ,minFilter_(TexFilter_Linear)
        ,mipFilter_(TexFilter_Point)
        ,mipmapLODBias_(0)
        ,maxMipLevel_(0)
        ,maxAnisotropy_(1)
    {
    }


    SamplerState::SamplerState(const SamplerState& rhs)
        :addressU_(rhs.addressU_)
        ,addressV_(rhs.addressV_)
        ,magFilter_(rhs.magFilter_)
        ,minFilter_(rhs.minFilter_)
        ,mipFilter_(rhs.mipFilter_)
        ,mipmapLODBias_(rhs.mipmapLODBias_)
        ,maxMipLevel_(rhs.maxMipLevel_)
        ,maxAnisotropy_(rhs.maxAnisotropy_)
    {
    }


    SamplerState::~SamplerState()
    {
    }

    void SamplerState::apply(u32 sampler) const
    {
        lgraphics::GraphicsDeviceRef &device = Graphics::getDevice();

        device.setSamplerState(sampler, D3DSAMP_ADDRESSU, addressU_);
        device.setSamplerState(sampler, D3DSAMP_ADDRESSV, addressV_);
        device.setSamplerState(sampler, D3DSAMP_MAGFILTER, magFilter_);
        device.setSamplerState(sampler, D3DSAMP_MINFILTER, minFilter_);
        device.setSamplerState(sampler, D3DSAMP_MIPFILTER, mipFilter_);
        device.setSamplerState(sampler, D3DSAMP_MIPMAPLODBIAS, mipmapLODBias_);
        device.setSamplerState(sampler, D3DSAMP_MAXMIPLEVEL, maxMipLevel_);
        device.setSamplerState(sampler, D3DSAMP_MAXANISOTROPY, maxAnisotropy_);
    }

    void SamplerState::swap(SamplerState& rhs)
    {
        lcore::swap(addressU_, rhs.addressU_);
        lcore::swap(addressV_, rhs.addressV_);
        lcore::swap(magFilter_, rhs.magFilter_);
        lcore::swap(minFilter_, rhs.minFilter_);
        lcore::swap(mipFilter_, rhs.mipFilter_);
        lcore::swap(mipmapLODBias_, rhs.mipmapLODBias_);
        lcore::swap(maxMipLevel_, rhs.maxMipLevel_);
        lcore::swap(maxAnisotropy_, rhs.maxAnisotropy_);
    }

}
