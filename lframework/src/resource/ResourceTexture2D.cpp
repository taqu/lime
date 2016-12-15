/**
@file ResourceTexture2D.cpp
@author t-sakai
@date 2016/11/22 create
*/
#include "resource/ResourceTexture2D.h"
#include <lcore/liostream.h>
#include <lgraphics/io/IODDS.h>
#include <lgraphics/io/IOPNG.h>
#include <lgraphics/io/IOBMP.h>
#include <lgraphics/io/IOTGA.h>

namespace lfw
{
    ResourceTexture2D* ResourceTexture2D::load(const Char* path, s64 size, const u8* memory, const TextureParameter& param)
    {
        LASSERT(NULL != path);
        LASSERT(0<=size);
        LASSERT(NULL != memory);

        s32 length = lcore::strlen_s32(path);
        const Char* ext = lcore::getExtension(length, path);
        lgfx::Texture2DRef texture;

        u32 width = 0;
        u32 height = 0;
        lcore::lsize_t rowBytes = 0;
        lgfx::DataFormat format = lgfx::Data_R8G8B8A8_UNorm_SRGB;
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
                true,
                width, height, format);

        }else if(0==lcore::strncmp(ext, ImageExtension_PNG, 3)){
            lcore::ibstream stream(static_cast<s32>(size), memory);
            //result = lgfx::io::IOPNG::read(
            //    texture,
            //    stream,
            //    lgfx::Usage_Default,
            //    lgfx::BindFlag_ShaderResource,
            //    lgfx::CPUAccessFlag_None,
            //    lgfx::ResourceMisc_None,
            //    true,
            //    lgfx::io::IOPNG::Swap_RGB,
            //    width, height, rowBytes, format);

        }else if(0==lcore::strncmp(ext, ImageExtension_BMP, 3)){
            lcore::ibstream stream(static_cast<s32>(size), memory);
            result = lgfx::io::IOBMP::read(
                texture,
                stream,
                lgfx::Usage_Default,
                lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                true,
                true,
                width, height, format);

        }else if(0==lcore::strncmp(ext, ImageExtension_TGA, 3)){
            lcore::ibstream stream(static_cast<s32>(size), memory);
            result = lgfx::io::IOTGA::read(
                texture,
                stream,
                lgfx::Usage_Default,
                lgfx::BindFlag_ShaderResource,
                lgfx::CPUAccessFlag_None,
                lgfx::ResourceMisc_None,
                true,
                true,
                width, height, format);

        }
        if(!result){
            return NULL;
        }
        lgfx::SamplerStateRef sampler = lgfx::SamplerState::create(
            static_cast<lgfx::TextureFilterType>(param.filterType_),
            static_cast<lgfx::TextureAddress>(param.addressUVW_),
            static_cast<lgfx::TextureAddress>(param.addressUVW_),
            static_cast<lgfx::TextureAddress>(param.addressUVW_),
            static_cast<lgfx::CmpFunc>(param.compFunc_),
            param.borderColor_);

        lgfx::Texture2DDesc desc;
        texture.getDesc(desc);

        lgfx::SRVDesc srvDesc;
        srvDesc.format_ = (lgfx::DataFormat)desc.Format;

        if(desc.MiscFlags & lgfx::ResourceMisc_TextureCube){
            //if(1<desc.ArraySize){
            //    dst.dimension_ = lgraphics::ViewSRVDimension_CubeArray;
            //    dst.texCubeArray_.mostDetailedMip_ = 0;
            //    dst.texCubeArray_.mipLevels_ = desc.MipLevels;
            //    dst.texCubeArray_.first2DArraySlice_ = 0;
            //    dst.texCubeArray_.numCubes_ = desc.ArraySize;
            //} else{
            srvDesc.dimension_ = lgfx::ViewSRVDimension_Cube;
            srvDesc.texCube_.mostDetailedMip_ = 0;
            srvDesc.texCube_.mipLevels_ = desc.MipLevels;
            //}

        } else{
            if(1<desc.ArraySize){
                srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2DArray;
                srvDesc.tex2DArray_.mostDetailedMip_ = 0;
                srvDesc.tex2DArray_.mipLevels_ = desc.MipLevels;
                srvDesc.tex2DArray_.firstArraySlice_ = 0;
                srvDesc.tex2DArray_.arraySize_ = desc.ArraySize;
            } else{
                srvDesc.dimension_ = lgfx::ViewSRVDimension_Texture2D;
                srvDesc.tex2D_.mostDetailedMip_ = 0;
                srvDesc.tex2D_.mipLevels_ = desc.MipLevels;
            }
        }
        lgfx::ShaderResourceViewRef srv = texture.createSRView(srvDesc);

        return LNEW ResourceTexture2D(texture, sampler, srv);
    }
}
