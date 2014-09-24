/**
@file IODDS.cpp
@author t-sakai
@date 2012/07/26 create
*/
#include "../lgraphicsAPIInclude.h"
#include "../lgraphicscore.h"

#include "IODDS.h"
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

        // old format
        enum D3DFMT
        {
            D3DFMT_A16B16G16R16 = 36,
            D3DFMT_R16F = 111,
            D3DFMT_G16R16F = 112,
            D3DFMT_A16B16G16R16F = 113,
            D3DFMT_R32F = 114,
            D3DFMT_G32R32F = 115,
            D3DFMT_A32B32G32R32F = 116,
        };

#define IsBitMask(r,g,b,a) ((r) == ddpf.RBitMask_ && (g) == ddpf.GBitMask_ && (b) == ddpf.BBitMask_)

        /**
        @param ddpf ... ピクセルフォーマット

        対応フォーマットはBufferFormatに入っているもののみ
        */
        DataFormat selectFormat(const IODDS::DDS_PIXELFORMAT& ddpf)
        {
            if(ddpf.flags_ & DDPF_RGB){
                switch (ddpf.RGBBitCount_)
                {
                case 32:
                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) ){
                        return Data_B8G8R8A8_UNorm_SRGB;
                    }

                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) ){
                        return Data_B8G8R8X8_UNorm_SRGB;
                    }
                    
                    if( IsBitMask(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) ){
                        return Data_R8G8B8A8_UNorm_SRGB;
                    }
                    
                    if( IsBitMask(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) ){
                        return Data_R8G8B8A8_UNorm_SRGB;
                    }

                    // Not support
                    if( IsBitMask(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000) ){
                        return Data_Unknown;
                    }

                    if( IsBitMask(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) ){
                        return Data_R10G10B10A2_UNorm;
                    }

                    if( IsBitMask(0x0000ffff,0xffff0000,0x00000000,0x00000000) ){
                        return Data_R16G16_UNorm;
                    }

                    if( IsBitMask(0xffffffff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R32_Float;
                    }
                    break;

                case 24:
                    // Not support B8G8A8
                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) ){
                        return Data_Unknown;
                    }
                    break;

                case 16:
                    if( IsBitMask(0x0000f800,0x000007e0,0x0000001f,0x00000000) ){
                        return Data_B5G6R5_UNorm;
                    }

                    if( IsBitMask(0x00007c00,0x000003e0,0x0000001f,0x00008000) ){
                        return Data_B5G5R5A1_UNorm;
                    }

                    if( IsBitMask(0x00007c00,0x000003e0,0x0000001f,0x00000000) ){
                        return Data_B5G5R5A1_UNorm;
                    }

                    // Not support B4G4R4A4
                    if( IsBitMask(0x00000f00,0x000000f0,0x0000000f,0x0000f000) ){
                        return Data_Unknown;
                    }

                    // Not support B4G4R4X4
                    if( IsBitMask(0x00000f00,0x000000f0,0x0000000f,0x00000000) ){
                        return Data_Unknown;
                    }

                    // Not support B2G3R3A8
                    if( IsBitMask(0x000000e0,0x0000001c,0x00000003,0x0000ff00) ){
                        return Data_Unknown;
                    }
                    break;
                }

            }else if(ddpf.flags_ & DDPF_LUMINANCE){
                switch(ddpf.RGBBitCount_)
                {
                case 16:
                    if( IsBitMask(0x0000ffff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R16_UNorm;
                    }

                    if( IsBitMask(0x000000ff,0x00000000,0x00000000,0x0000ff00) ){
                        return Data_R8G8_UNorm;
                    }
                    break;

                case 8:
                    //Not support A4L4
                    if( IsBitMask(0x0000000f,0x00000000,0x00000000,0x000000f0) ){
                        return Data_Unknown;
                    }

                    if( IsBitMask(0x000000ff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R8_UNorm;
                    }
                    break;
                }

            }else if(ddpf.flags_ & DDPF_ALPHA){
                switch(ddpf.RGBBitCount_)
                {
                case 8:
                    return Data_A8_UNorm;
                    break;
                }

            }else if(ddpf.flags_ & DDPF_FOURCC){
                //圧縮形式
                switch(ddpf.fourCC_)
                {
                case DXFORCC_DXT1:
                    return Data_BC1_UNorm_SRGB;

                case DXFORCC_DXT3:
                    return Data_BC2_UNorm_SRGB;

                case DXFORCC_DXT5:
                    return Data_BC3_UNorm_SRGB;

                case D3DFMT_A16B16G16R16:
                    return Data_R16G16B16A16_UNorm;

                case D3DFMT_R16F:
                    return Data_R16_Float;

                case D3DFMT_G16R16F:
                    return Data_R16G16_Float;

                case D3DFMT_A16B16G16R16F:
                    return Data_R16G16B16A16_Float;

                case D3DFMT_R32F:
                    return Data_R32_Float;

                case D3DFMT_G32R32F:
                    return Data_R32G32_Float;

                case D3DFMT_A32B32G32R32F:
                    return Data_R32G32B32A32_Float;
                };
            }

            return Data_Unknown;
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

    bool IODDS::checkSignature(lcore::istream& is)
    {
        s32 pos = is.tellg();
        u32 magic;
        u32 count = lcore::io::read(is, magic);
        is.seekg(pos, lcore::ios::beg);
        if(count<1){
            return false;
        }
        return (magic == DDS_MAGIC);
    }

    bool IODDS::read(Texture2DRef& texture, const u8* data, u32 size, Usage usage, TextureFilterType filter, TextureAddress adress)
    {
        static const u32 MaxNumSubResourceData = 256;
        SubResourceData initData[MaxNumSubResourceData];

        MemoryStream memStream(reinterpret_cast<const s8*>(data), size);

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

            if((header.caps2_ & DDSCAPS2_CUBEMAP) != 0){
                arraySize = 0;
                u32 cubeMapFlags[] =
                {
                    DDSCAPS2_CUBEMAP_POSITIVEX,
                    DDSCAPS2_CUBEMAP_NEGATIVEX,
                    DDSCAPS2_CUBEMAP_POSITIVEY,
                    DDSCAPS2_CUBEMAP_NEGATIVEY,
                    DDSCAPS2_CUBEMAP_POSITIVEZ,
                    DDSCAPS2_CUBEMAP_NEGATIVEZ,
                };
                for(s32 i=0; i<6; ++i){
                    if((header.caps2_ & cubeMapFlags[i]) != 0){
                        ++arraySize;
                    }
                }

            }else{
                arraySize = 1;
            }
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

        //u32 fileSize = (u32)mem - (u32)data;

        lgraphics::ResourceMisc miscFlag = ResourceMisc_None;
        ResourceViewDesc viewDesc;
        viewDesc.format_ = format;

        if(arraySize>1){
            viewDesc.dimension_ = ViewSRVDimension_Cube;
            viewDesc.texCube_.mostDetailedMip_ = 0;
            viewDesc.texCube_.mipLevels_ = mipmapLevel;
            miscFlag = lgraphics::ResourceMisc_TextureCube;
        }else{
            viewDesc.dimension_ = ViewSRVDimension_Texture2D;
            viewDesc.tex2D_.mostDetailedMip_ = 0;
            viewDesc.tex2D_.mipLevels_ = mipmapLevel;
        }

        texture = Texture::create2D(
            header.width_,
            header.height_,
            mipmapLevel,
            arraySize,
            format,
            usage,
            BindFlag_ShaderResource,
            CPUAccessFlag_None,
            miscFlag,
            filter,
            adress,
            lgraphics::Cmp_Never,
            0.0f,
            initData,
            &viewDesc);

        return true;
    }

}
}
