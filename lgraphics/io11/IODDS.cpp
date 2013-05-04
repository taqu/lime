/**
@file IODDS.cpp
@author t-sakai
@date 2012/07/26 create
*/
#include "../lgraphicsAPIInclude.h"
#include "../lgraphicscore.h"

#include "IODDS.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"
#include "MemoryStream.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        enum DDSD
        {
            DDSD_CAPS = 0x00000001,
            DDSD_HEIGHT = 0x00000002,
            DDSD_WIDTH = 0x00000004,
            DDSD_PITCH = 0x00000008,
            DDSD_PIXELFORMAT = 0x00001000,
            DDSD_MIPMAPCOUNT = 0x00020000,
            DDSD_LINEARSIZE = 0x00080000,
            DDSD_DEPTH = 0x00800000,
        };

        enum DDPF
        {
            DDPF_ALPHAPIXELS = 0x00000001,
            DDPF_ALPHA = 0x00000002,
            DDPF_FOURCC = 0x00000004,
            DDPF_RGB = 0x00000040,
            DDPF_RGBA = 0x00000041,
            DDPF_YUV = 0x200,
            DDPF_LUMINANCE = 0x20000,
        };

        enum DDSCAPS
        {
            DDSCAPS_COMPLEX = 0x00000008,
            DDSCAPS_TEXTURE = 0x00001000,
            DDSCAPS_MIPMAP = 0x00400000,
        };

        enum DDSCAPS2
        {
            DDSCAPS2_CUBEMAP = 0x00000200,
            DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
            DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
            DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
            DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
            DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
            DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
            DDSCAPS2_VOLUME = 0x00200000,
        };

        enum DXFORCC
        {
            DXFORCC_DXT1 = LIME_MAKE_FOURCC('D', 'X', 'T', '1'),
            DXFORCC_DXT3 = LIME_MAKE_FOURCC('D', 'X', 'T', '3'),
            DXFORCC_DXT5 = LIME_MAKE_FOURCC('D', 'X', 'T', '5'),
        };

        struct DDS_HEADER_DXT10
        {
            DXGI_FORMAT format_;
            D3D11_RESOURCE_DIMENSION dimension_;
            UINT miscFlag_;
            UINT arraySize_;
            UINT reserved_;
        };

        /**
        @param ddpf ... ピクセルフォーマット

        対応フォーマットはBufferFormatに入っているもののみ
        */
        DataFormat selectFormat(const IODDS::DDS_PIXELFORMAT& ddpf)
        {
            DataFormat format = Data_Unknown;
            switch(ddpf.flags_)
            {
            case DDPF_RGBA:
                {
                    if((ddpf.RBitMask_ == 0x3ffU)
                        && (ddpf.GBitMask_ == 0xffc00U)
                        && (ddpf.BBitMask_ == 0x3ff00000U))
                    {
                        format = Data_R10G10B10A2_UInt;

                    }else if((ddpf.RBitMask_ == 0xffU)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xff0000U))
                    {
                        format = Data_R8G8B8A8_UNorm;

                    }else if((ddpf.RBitMask_ == 0xff0000U)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xffU))
                    {
                        format = Data_B8G8R8A8_UNorm;
                    }
                }
                break;

            case DDPF_RGB:
                {
                    if((ddpf.RBitMask_ == 0xffffU)
                        && (ddpf.GBitMask_ == 0xffff0000U))
                    {
                        format = Data_R16G16_UNorm;

                    }else if((ddpf.RBitMask_ == 0xff0000U)
                        && (ddpf.GBitMask_ == 0xff00U)
                        && (ddpf.BBitMask_ == 0xffU))
                    {
                        if(ddpf.RGBBitCount_ == 32){
                            format = Data_B8G8R8X8_UNorm;
                        }

                    }
                }
                break;

            case DDPF_ALPHA:
                {
                    format = Data_A8_UNorm;
                }
                break;

            case DDPF_LUMINANCE:
                {
                    if(ddpf.RGBBitCount_ == 8){
                        format = Data_R8_UNorm;

                    }else if(ddpf.RGBBitCount_ == 16){
                        format = Data_R16_UNorm;
                    }
                }
                break;

            case DDPF_FOURCC:
                {
                    //圧縮形式
                    switch(ddpf.fourCC_)
                    {
                    case DXFORCC_DXT1:
                        format = Data_BC1_UNorm;
                        break;
                    case DXFORCC_DXT3:
                        format = Data_BC2_UNorm;
                        break;
                    case DXFORCC_DXT5:
                        format = Data_BC3_UNorm;
                        break;
                    };
                }
                break;
            }

            return format;
        }

        void getSize(DataFormat format, u32 width, u32 height, u32& size, u32& pitch, u32& numRows)
        {
            bool compress = false;
            u32 minSize = 0;
            switch(format)
            {
            case Data_BC1_TypeLess:
            case Data_BC1_UNorm:
            case Data_BC1_UNorm_SRGB:

            case Data_BC4_TypeLess:
            case Data_BC4_UNorm:
            case Data_BC4_SNorm:
                compress = true;
                minSize = 8;
                break;

            case Data_BC2_TypeLess:
            case Data_BC2_UNorm:
            case Data_BC2_UNorm_SRGB:

            case Data_BC3_TypeLess:
            case Data_BC3_UNorm:
            case Data_BC3_UNorm_SRGB:

            case Data_BC5_TypeLess:
            case Data_BC5_UNorm:
            case Data_BC5_SNorm:

            case Data_BC6H_TypeLess:
            case Data_BC6H_UF16:
            case Data_BC6H_SF16:

            case Data_BC7_TypeLess:
            case Data_BC7_UNorm:
            case Data_BC7_UNorm_SRGB:
                compress = true;
                minSize = 16;
                break;
            };
            if(compress){
                pitch = minSize * lcore::maximum<u32>(1, width>>2);
                numRows = lcore::maximum<u32>(1, height>>2);
            }else{
                pitch = (width * getBitsPerPixel(format) + 7) >> 3;
                numRows = height;
            }
            size = numRows * pitch;
        }

    }

    bool IODDS::read(const s8* data, u32 size, Texture2DRef& texture)
    {
        static const u32 MaxNumSubResourceData = 64;
        SubResourceData initData[MaxNumSubResourceData];

        MemoryStream memStream(data, size);

        u32 magic = 0;
        memStream.read(magic);
        if(magic != DDS_MAGIC){
            return false;
        }

        DDS_HEADER header;
        memStream.read(header);

        if(memStream.eof()){
            return false;
        }

        if(header.size_ != DDS_HEADER_SIZE){
            return false;
        }

        DataFormat format = Data_Unknown;
        u32 arraySize = 0;
        DDS_HEADER_DXT10 header10;
        if((header.ddpf_.flags_ & DDPF_FOURCC) != 0
            && header.ddpf_.fourCC_ == DDS_DX10)
        {
            memStream.read(header10);
            format = static_cast<DataFormat>(header10.format_);
            arraySize = header10.arraySize_;
        }else{
            format = selectFormat(header.ddpf_);
            arraySize = 1;
        }

        if(format == Data_Unknown){
            return false;
        }


        u32 mipmapLevel = (header.mipmap_ > 0)? header.mipmap_ : 1;

        u32 numSubResources = arraySize * mipmapLevel;
        if(MaxNumSubResourceData<numSubResources){
            return false;
        }

        u32 bytes = 0;
        u32 pitch = 0;
        u32 numRows = 0;
        u32 w, h;
        const s8* mem = memStream.getTop();
        u32 index = 0;
        for(u32 i=0; i<arraySize; ++i){
            w = header.width_;
            h = header.height_;
            for(u32 j=0; j<mipmapLevel; ++j, ++index){
                getSize(format, w, h, bytes, pitch, numRows);
                initData[index].mem_ = mem;
                initData[index].pitch_ = pitch;
                initData[index].slicePitch_ = 0;
                mem += bytes;

                w = lcore::maximum<u32>(1, w>>1);
                h = lcore::maximum<u32>(1, h>>1);
            }
        }

        ResourceViewDesc viewDesc;
        viewDesc.format_ = format;
        viewDesc.tex2D_.mostDetailedMip_ = 0;
        viewDesc.tex2D_.mipLevels_ = mipmapLevel;

        texture = Texture::create2D(
            header.width_,
            header.height_,
            mipmapLevel,
            arraySize,
            format,
            Usage_Default,
            BindFlag_ShaderResource,
            CPUAccessFlag_None,
            ResourceMisc_None,
            TexFilter_MinMagMipLinear,
            TexAddress_Wrap,
            0.0f,
            initData,
            &viewDesc);

        return true;
    }

}
}
