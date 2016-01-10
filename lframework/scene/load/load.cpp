/**
@file load.cpp
@author t-sakai
@date 2015/01/15 create
*/
#include "load.h"
#include <lcore/lcore.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/io11/IODDS.h>
#include <lgraphics/io11/IOPNG.h>
#include <lgraphics/io11/IOBMP.h>
#include "../file/File.h"
#include "load_material.h"

namespace lscene
{
namespace lload
{
namespace
{
    void createSRVDesc(lgraphics::SRVDesc& dst, lgraphics::Texture2DDesc& desc)
    {
        dst.format_ = (lgraphics::DataFormat)desc.Format;

        if(desc.MiscFlags & lgraphics::ResourceMisc_TextureCube){
            //if(1<desc.ArraySize){
            //    dst.dimension_ = lgraphics::ViewSRVDimension_CubeArray;
            //    dst.texCubeArray_.mostDetailedMip_ = 0;
            //    dst.texCubeArray_.mipLevels_ = desc.MipLevels;
            //    dst.texCubeArray_.first2DArraySlice_ = 0;
            //    dst.texCubeArray_.numCubes_ = desc.ArraySize;
            //} else{
                dst.dimension_ = lgraphics::ViewSRVDimension_Cube;
                dst.texCube_.mostDetailedMip_ = 0;
                dst.texCube_.mipLevels_ = desc.MipLevels;
            //}

        } else{
            if(1<desc.ArraySize){
                dst.dimension_ = lgraphics::ViewSRVDimension_Texture2DArray;
                dst.tex2DArray_.mostDetailedMip_ = 0;
                dst.tex2DArray_.mipLevels_ = desc.MipLevels;
                dst.tex2DArray_.firstArraySlice_ = 0;
                dst.tex2DArray_.arraySize_ = desc.ArraySize;
            } else{
                dst.dimension_ = lgraphics::ViewSRVDimension_Texture2D;
                dst.tex2D_.mostDetailedMip_ = 0;
                dst.tex2D_.mipLevels_ = desc.MipLevels;
            }
        }
    }
}

    FileType getFileType(const Char* path)
    {
        LASSERT(NULL != path);
        lload::FileType type = lload::FileType_Unknown;

        u32 len = lcore::strlen(path);
        const Char* ext = lcore::rFindChr(path, '.', len);
        if(NULL == ext){
            return type;
        }

        if(0==lcore::strncmp(ext, ".dds", 4)){
            type = lload::FileType_DDS;
        }else if(0==lcore::strncmp(ext, ".png", 4)){
            type = lload::FileType_PNG;
        }else if(0==lcore::strncmp(ext, ".bmp", 4)){
            type = lload::FileType_BMP;
        }
        return type;
    }

    bool load(
        lgraphics::Texture2DRef& texture,
        lgraphics::SamplerStateRef& sampler,
        lgraphics::ShaderResourceViewRef& srv,
        lscene::lfile::SharedFile& file,
        FileType type,
        u32 usage,
        u32 filterType,
        u32 address,
        bool sRGB)
    {
        u32 width = 0;
        u32 height = 0;
        u32 rowBytes = 0;
        lgraphics::DataFormat format = (sRGB)? lgraphics::Data_R8G8B8A8_UNorm_SRGB : lgraphics::Data_R8G8B8A8_UNorm;
        sampler = lgraphics::SamplerState::create(
            (lgraphics::TextureFilterType)filterType,
            (lgraphics::TextureAddress)address,
            (lgraphics::TextureAddress)address,
            (lgraphics::TextureAddress)address,
            lgraphics::Cmp_Never);

        lgraphics::SRVDesc srvDesc;
        switch(type)
        {
        case FileType_DDS:
            {
                u32 size = file.getSize(0);
                u8* buffer = reinterpret_cast<u8*>( LSCENE_MALLOC(size) );
                lcore::io::read(file, buffer, size);
                bool ret = lgraphics::io::IODDS::read(
                    texture,
                    buffer,
                    size,
                    (lgraphics::Usage)usage,
                    lgraphics::BindFlag_ShaderResource,
                    lgraphics::CPUAccessFlag_None,
                    lgraphics::ResourceMisc_None,
                    sRGB,
                    width, height, format);
                LSCENE_FREE(buffer);

                if(ret){
                    lgraphics::Texture2DDesc desc;
                    texture.getDesc(desc);
                    createSRVDesc(srvDesc, desc);
                    srv = texture.createSRView(srvDesc);
                }
                return ret;
            }
            break;

        case FileType_PNG:
            if(false == lgraphics::io::IOPNG::read(file, NULL, width, height, rowBytes, format, sRGB, lgraphics::io::IOPNG::Swap_BGR)){
                return false;
            }
            break;

        case FileType_BMP:
            if(false == lgraphics::io::IOBMP::read(file, NULL, width, height, format, sRGB)){
                return false;
            }
            rowBytes = 4*width;
            break;
        };

        u32 size = rowBytes * height;
        u8* buffer = reinterpret_cast<u8*>(LSCENE_MALLOC(size));
        bool ret = false;
        switch(type)
        {
        case FileType_PNG:
            ret = lgraphics::io::IOPNG::read(file, buffer, width, height, rowBytes, format, sRGB, lgraphics::io::IOPNG::Swap_BGR);
            break;

        case FileType_BMP:
            ret = lgraphics::io::IOBMP::read(file, buffer, width, height, format, sRGB);
            break;
        };

        if(ret){
            lgraphics::SubResourceData initData;
            initData.mem_ = buffer;
            initData.pitch_ = rowBytes;
            initData.slicePitch_ = 0;

            texture = lgraphics::Texture::create2D(
                width,
                height,
                1,
                1,
                format,
                (lgraphics::Usage)usage,
                lgraphics::BindFlag_ShaderResource,
                lgraphics::CPUAccessFlag_None,
                lgraphics::ResourceMisc_None,
                &initData);
            ret = texture.valid();
            if(ret){
                srvDesc.dimension_ = lgraphics::ViewSRVDimension_Texture2D;
                srvDesc.format_ = format;
                srvDesc.tex2D_.mostDetailedMip_ = 0;
                srvDesc.tex2D_.mipLevels_ = 1;
                srv = texture.createSRView(srvDesc);
            }
        }
        LSCENE_FREE(buffer);
        return ret;
    }

    Material* loadMaterials(s32& numMaterials, u8* memory)
    {
        Header& header = *reinterpret_cast<Header*>(memory);
        numMaterials = header.elems_[Elem_Material].number_;

        Material* materials = reinterpret_cast<Material*>(memory + header.elems_[Elem_Material].offset_);
        return materials;
    }
}
}
