/**
@file load.cpp
@author t-sakai
@date 2015/01/15 create
*/
#include "load.h"
#include <lcore/utility.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/io11/IODDS.h>
#include <lgraphics/io11/IOPNG.h>
#include <lgraphics/io11/IOBMP.h>
#include "../file/File.h"

namespace lscene
{
namespace lload
{
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
        lscene::lfile::SharedFile& file,
        FileType type,
        u32 usage,
        u32 filterType,
        u32 address)
    {
        u32 width = 0;
        u32 height = 0;
        u32 rowBytes = 0;
        lgraphics::DataFormat format = lgraphics::Data_R8G8B8A8_UNorm_SRGB;

        switch(type)
        {
        case FileType_DDS:
            {
                u32 size = file.getSize(0);
                u8* buffer = reinterpret_cast<u8*>( LSCENE_MALLOC(size) );
                lcore::io::read(file, buffer, size);
                bool ret = lgraphics::io::IODDS::read(texture, buffer, size, (lgraphics::Usage)usage, (lgraphics::TextureFilterType)filterType, (lgraphics::TextureAddress)address);
                LSCENE_FREE(buffer);
                return ret;
            }
            break;

        case FileType_PNG:
            if(false == lgraphics::io::IOPNG::read(file, NULL, width, height, rowBytes, format, lgraphics::io::IOPNG::Swap_BGR)){
                return false;
            }
            break;

        case FileType_BMP:
            if(false == lgraphics::io::IOBMP::read(file, NULL, width, height, format)){
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
            ret = lgraphics::io::IOPNG::read(file, buffer, width, height, rowBytes, format, lgraphics::io::IOPNG::Swap_BGR);
            break;

        case FileType_BMP:
            ret = lgraphics::io::IOBMP::read(file, buffer, width, height, format);
            break;
        };

        if(ret){
            lgraphics::SubResourceData initData;
            initData.mem_ = buffer;
            initData.pitch_ = rowBytes;
            initData.slicePitch_ = 0;

            lgraphics::ResourceViewDesc viewDesc;
            viewDesc.dimension_ = lgraphics::ViewSRVDimension_Texture2D;
            viewDesc.format_ = format;
            viewDesc.tex2D_.mostDetailedMip_ = 0;
            viewDesc.tex2D_.mipLevels_ = 1;

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
                (lgraphics::TextureFilterType)filterType,
                (lgraphics::TextureAddress)address,
                lgraphics::Cmp_Never,
                0.0f,
                &initData,
                &viewDesc);
            ret = texture.valid();
        }
        LSCENE_FREE(buffer);
        return ret;
    }
}
}
