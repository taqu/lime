/**
@file ResourceTexture3D.cpp
@author t-sakai
@date 2017/07/29 create
*/
#include "resource/ResourceTexture3D.h"
#include <lcore/File.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOPNG.h>
#include <lgraphics/io/IOBMP.h>
#include <lgraphics/io/IOTGA.h>

namespace lfw
{
    ResourceTexture3D* ResourceTexture3D::load(const Char* path, s64 size, const u8* memory, const TextureParameter& param)
    {
        LASSERT(NULL != path);
        LASSERT(0<=size);
        LASSERT(NULL != memory);

        s32 length = lcore::strlen_s32(path);
        const Char* ext = lcore::Path::getExtension(length, path);
        lgfx::Texture3DRef texture;

        s32 width = 0;
        s32 height = 0;
        s32 depth = 0;
        lgfx::DataFormat format = (param.isSRGB())? lgfx::Data_R8G8B8A8_UNorm_SRGB : lgfx::Data_R8G8B8A8_UNorm;
        bool result = false;

        if(0==lcore::strncmp(ext, ImageExtension_DDS, 3)){
            result = lgfx::io::IODDS::read(
                texture,
                static_cast<s32>(size),
                memory,
                lgfx::Usage_Default,
                lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                width, height, depth, format);
        }
        if(!result){
            return NULL;
        }
        lgfx::SamplerStateRef sampler;
        
        if(param.isSampler()){
            sampler = lgfx::SamplerState::create(
                static_cast<lgfx::TextureFilterType>(param.filterType_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::CmpFunc>(param.compFunc_),
                param.borderColor_);
        }

        lgfx::Texture3DDesc desc;
        texture.getDesc(desc);

        lgfx::SRVDesc srvDesc;
        srvDesc.format_ = (lgfx::DataFormat)desc.Format;

        if(desc.MiscFlags & lgfx::ResourceMisc_TextureCube){
            LASSERT(false);

        } else{
            srvDesc.dimension_ = lgfx::SRVDimension_Texture3D;
            srvDesc.tex3D_.mostDetailedMip_ = 0;
            srvDesc.tex3D_.mipLevels_ = desc.MipLevels;
        }
        lgfx::ShaderResourceViewRef srv = texture.createSRView(srvDesc);

        return LNEW ResourceTexture3D(texture, sampler, srv);
    }

    ResourceTexture3D* ResourceTexture3D::create(u32 color, const TextureParameter& param)
    {
        u32 buffer[1] = {color};

        lgfx::SubResourceData initData;
        initData.pitch_ = 4;
        initData.slicePitch_ = 4;
        initData.mem_ = buffer;
        lgfx::Texture3DRef texture = lgfx::Texture::create3D(
            1,
            1,
            1, 1,
            (param.isSRGB())? lgfx::Data_R8G8B8A8_UNorm_SRGB : lgfx::Data_R8G8B8A8_UNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            &initData);

        lgfx::SamplerStateRef sampler;
        if(param.isSampler()){
            sampler = lgfx::SamplerState::create(
                static_cast<lgfx::TextureFilterType>(param.filterType_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::CmpFunc>(param.compFunc_),
                param.borderColor_);
        }

        lgfx::SRVDesc srvDesc;
        srvDesc.format_ = (param.isSRGB())? lgfx::Data_R8G8B8A8_UNorm_SRGB : lgfx::Data_R8G8B8A8_UNorm;
        srvDesc.dimension_ = lgfx::SRVDimension_Texture3D;
        srvDesc.tex3D_.mostDetailedMip_ = 0;
        srvDesc.tex3D_.mipLevels_ = 1;
        lgfx::ShaderResourceViewRef srv = texture.createSRView(srvDesc);
        return LNEW ResourceTexture3D(texture, sampler, srv);
    }

    ResourceTexture3D* ResourceTexture3D::create(u32 width, u32 height, u32 depth, const u32* colors, const TextureParameter& param)
    {
        LASSERT(NULL != colors);

        lgfx::SubResourceData initData;
        initData.pitch_ = sizeof(u32)*width;
        initData.slicePitch_ = initData.pitch_*height;
        initData.mem_ = colors;
        lgfx::Texture3DRef texture = lgfx::Texture::create3D(
            width,
            height,
            depth,
            1,
            (param.isSRGB())? lgfx::Data_R8G8B8A8_UNorm_SRGB : lgfx::Data_R8G8B8A8_UNorm,
            lgfx::Usage_Default,
            lgfx::BindFlag_ShaderResource,
            lgfx::CPUAccessFlag_None,
            lgfx::ResourceMisc_None,
            &initData);

        lgfx::SamplerStateRef sampler;
        if(param.isSampler()){
            sampler = lgfx::SamplerState::create(
                static_cast<lgfx::TextureFilterType>(param.filterType_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::TextureAddress>(param.addressUVW_),
                static_cast<lgfx::CmpFunc>(param.compFunc_),
                param.borderColor_);
        }

        lgfx::SRVDesc srvDesc;
        srvDesc.format_ = (param.isSRGB())? lgfx::Data_R8G8B8A8_UNorm_SRGB : lgfx::Data_R8G8B8A8_UNorm;
        srvDesc.dimension_ = lgfx::SRVDimension_Texture3D;
        srvDesc.tex3D_.mostDetailedMip_ = 0;
        srvDesc.tex3D_.mipLevels_ = 1;
        lgfx::ShaderResourceViewRef srv = texture.createSRView(srvDesc);
        return LNEW ResourceTexture3D(texture, sampler, srv);
    }
}
