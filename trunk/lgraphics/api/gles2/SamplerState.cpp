/**
@file SamplerState.cpp
@author t-sakai
@date 2010/06/14 create

*/
#include "../../lgraphics.h"
#include "../SamplerState.h"

#include "../../lgraphicsAPIInclude.h"

using namespace lcore;

namespace lgraphics
{

    SamplerState::SamplerState()
        :addressU_(TexAddress_Wrap)
        ,addressV_(TexAddress_Wrap)
        ,magFilter_(TexFilter_Linear)
        ,minFilter_(TexFilter_Linear)
        ,mipFilter_(TexFilter_Linear)
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
        glActiveTexture(GL_TEXTURE0 + sampler);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter_);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, addressU_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, addressV_);

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
