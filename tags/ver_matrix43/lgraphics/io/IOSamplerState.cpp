/**
@file IOSamplerState.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "IOSamplerState.h"

#include "SectionID.h"

#include "../api/SamplerState.h"
#include "../api/EnumerationCodes.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        struct SamplerStateInfo
        {
            u32 id_;
            u32 sampler_;

            u32 addressU_;
            u32 addressV_;

            u32 magFilter_;
            u32 minFilter_;
            u32 mipFilter_;

            u32 mipLODBias_;
            u32 maxMipLevel_;
            u32 maxAnisotropy_;
        };
    }

    bool IOSamplerState::read(lcore::istream& is, SamplerState& state)
    {
        SamplerStateInfo info;
        lcore::io::read(is, info);

        state.setAddressU( decodeTextureAddress( static_cast<CodeTextureAddress>(info.addressU_ ) ) );
        state.setAddressV( decodeTextureAddress( static_cast<CodeTextureAddress>(info.addressV_ ) ) );
        state.setMagFilter( decodeTextureFilter( static_cast<CodeTextureFilterType>(info.magFilter_ ) ) );
        state.setMinFilter( decodeTextureFilter( static_cast<CodeTextureFilterType>(info.minFilter_ ) ) );
        state.setMipFilter( decodeTextureFilter( static_cast<CodeTextureFilterType>(info.mipFilter_ ) ) );

        state.setMipLODBias( info.mipLODBias_ );
        state.setMaxMipLevel( info.maxMipLevel_ );
        state.setMaxAnisotropy( info.maxAnisotropy_ );

        return true;
    }

    bool IOSamplerState::write(lcore::ostream& os, const SamplerState& state)
    {
        SamplerStateInfo info;

        info.id_ = format::SamplerStateID;

        info.addressU_ = static_cast<u32>( encodeTextureAddress( state.getAddressU() ) );
        info.addressV_ = static_cast<u32>( encodeTextureAddress( state.getAddressV() ) );

        info.magFilter_ = static_cast<u32>( encodeTextureFilter( state.getMagFilter() ) );
        info.minFilter_ = static_cast<u32>( encodeTextureFilter( state.getMinFilter() ) );
        info.mipFilter_ = static_cast<u32>( encodeTextureFilter( state.getMipFilter() ) );

        info.mipLODBias_ = state.getMipLODBias();
        info.maxMipLevel_ = state.getMaxMipLevel();
        info.maxAnisotropy_ = state.getMaxAnisotropy();

        lcore::io::write(os, info);

        return true;
    }
}
}
